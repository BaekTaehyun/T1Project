// Fill out your copyright notice in the Description page of Project Settings.

#include "GsPartsLocal.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Container/GsPartsDataContainerBase.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"

FGsPartsLocal::FGsPartsLocal()
{
}

FGsPartsLocal::~FGsPartsLocal()
{
}

void FGsPartsLocal::Initialize(UGsGameObjectBase* owner)
{
	Super::Initialize(owner);

	Local = Cast<UGsGameObjectLocal>(owner);
    if (Local)
    {
        ActorComponent = Local->GetLocalCharacter()->FindComponentByClass<UActorComponent>();

		for (uint8 i = (uint8)EGsPartsType::HAIR; i < (uint8)EGsPartsType::FOOT + 1; ++i)
		{
			auto newMeshComponent = NewObject<USkeletalMeshComponent>(ActorComponent);
			InitSkeletalMeshComponent(newMeshComponent);
			MapMeshComponent.Emplace((EGsPartsType)i, newMeshComponent);
		}
    }
}

void FGsPartsLocal::Finalize()
{
	Super::Finalize();
	
	/*for (auto pair : MapMeshComponent)
	{
	}*/
	MapMeshComponent.Reset();
}

void FGsPartsLocal::InitSkeletalMeshComponent(USkeletalMeshComponent* Mesh)
{
	Mesh->AlwaysLoadOnClient = true;
	Mesh->bOwnerNoSee = false;
	//스킨 메쉬 애니메이션 Update 옵션
	Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	//primitive 쉐도우 연산 관련 플레그
	Mesh->bCastDynamicShadow = true;
	//primitive 라이트 연산 관련 플레그
	Mesh->bAffectDynamicIndirectLighting = true;
	//컴퍼넌트의 Tick 간격 설정 : http://api.unrealengine.com/KOR/Programming/UnrealArchitecture/Actors/Ticking/index.html#%ED%8B%B1%EA%B7%B8%EB%A3%B9
	Mesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;

	static FName CollisionProfileName(TEXT("CharacterMesh"));
	//콜리 전 오브젝트 유형 설정
	Mesh->SetCollisionObjectType(ECC_Pawn);
	Mesh->SetCollisionProfileName(CollisionProfileName);

	//컴퍼넌트 붙이기 작업
	auto actor = Local->GetLocalCharacter();
	//기준이 되는 SkeletalMeshComponent
	if (USkeletalMeshComponent* MeshComponent = actor->GetMesh())
	{
		Mesh->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		Mesh->RegisterComponent();
		Mesh->SetMasterPoseComponent(MeshComponent);
		Mesh->bUseBoundsFromMasterPoseComponent = true;
	}
}

void FGsPartsLocal::Attached()
{
	Super::Attached();

	auto MeshComponent = Local->GetLocalCharacter()->GetMesh();
	MasterPose();

	/*if (USkeletalMesh* mesh = MergeParts())
	{
		MeshComponent->SetSkeletalMesh(mesh);
	}*/
}

void FGsPartsLocal::Detached()
{
	Super::Detached();

	if (USkeletalMesh* mesh = MergeParts())
	{
		USkeletalMeshComponent* MeshComponent = Local->GetLocalCharacter()->GetMesh();
		MeshComponent->SetSkeletalMesh(mesh);
	}
}

void FGsPartsLocal::MasterPose()
{
	if (ListParts.Num() > 0)
	{
		for (auto el : ListParts)
		{
			if (auto meshComponent = MapMeshComponent.Find(el.Get()->Type))
			{
				(*meshComponent)->SetSkeletalMesh(el.Get()->Mesh);
				//(*meshComponent)->SetMasterPoseComponent(SkeletaMesh);
			}
		}
	}
}

USkeletalMesh* FGsPartsLocal::MergeParts() const
{
	if (ListParts.Num() > 0)
	{
		TArray<USkeletalMesh*> mergeMeshes;
		mergeMeshes.Empty(ListParts.Num());

		for (auto el : ListParts)
		{
			mergeMeshes.Add(el.Get()->Mesh);
		}

		if (mergeMeshes.Num() > 1)
		{
			USkeletalMesh* targetMesh = NewObject<USkeletalMesh>(ActorComponent);
			TArray<FSkelMeshMergeSectionMapping> sectionMappings;
			FSkeletalMeshMerge merger(targetMesh, mergeMeshes, sectionMappings, 0);
			if (!merger.DoMerge())
			{
				UE_LOG(LogTemp, Warning, TEXT("Merge failed!"));
				return NULL;
			}

			targetMesh->Skeleton = mergeMeshes[0]->Skeleton;
			return targetMesh;
		}
		else if (mergeMeshes.Num() == 1)
		{
			return mergeMeshes[0];
		}
	}
	return NULL;
}

void FGsPartsLocal::ToMergeParams(const TArray<FSkelMeshMergeSectionMapping_BP>& InSectionMappings,
	TArray<FSkelMeshMergeSectionMapping>& OutSectionMappings)
{
	if (InSectionMappings.Num() > 0)
	{
		OutSectionMappings.AddUninitialized(InSectionMappings.Num());
		for (int32 i = 0; i < InSectionMappings.Num(); ++i)
		{
			OutSectionMappings[i].SectionIDs = InSectionMappings[i].SectionIDs;
		}
	}
}

void FGsPartsLocal::ToMergeParams(const TArray<FSkelMeshMergeUVTransformMapping>& InUVTransformsPerMesh,
	TArray<FSkelMeshMergeUVTransforms>& OutUVTransformsPerMesh)
{
	if (InUVTransformsPerMesh.Num() > 0)
	{
		OutUVTransformsPerMesh.Empty();
		OutUVTransformsPerMesh.AddUninitialized(InUVTransformsPerMesh.Num());
		for (int32 i = 0; i < InUVTransformsPerMesh.Num(); ++i)
		{
			TArray<TArray<FTransform>>& OutUVTransforms = OutUVTransformsPerMesh[i].UVTransformsPerMesh;
			const TArray<FSkelMeshMergeUVTransform>& InUVTransforms = InUVTransformsPerMesh[i].UVTransformsPerMesh;
			if (InUVTransforms.Num() > 0)
			{
				OutUVTransforms.Empty();
				OutUVTransforms.AddUninitialized(InUVTransforms.Num());
				for (int32 j = 0; j < InUVTransforms.Num(); j++)
				{
					OutUVTransforms[i] = InUVTransforms[i].UVTransforms;
				}
			}
		}
	}
}

USkeletalMesh* FGsPartsLocal::MergeToParams(const FSkeletalMeshMergeParams& Params)
{
	TArray<USkeletalMesh*> MeshesToMergeCopy = Params.MeshesToMerge;
	MeshesToMergeCopy.RemoveAll([](USkeletalMesh* InMesh)
	{
		return InMesh == nullptr;
	});
	if (MeshesToMergeCopy.Num() <= 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Must provide multiple valid Skeletal Meshes in order to perform a merge."));
		return nullptr;
	}
	EMeshBufferAccess BufferAccess = Params.bNeedsCpuAccess ?
		EMeshBufferAccess::ForceCPUAndGPU :
		EMeshBufferAccess::Default;
	TArray<FSkelMeshMergeSectionMapping> SectionMappings;
	TArray<FSkelMeshMergeUVTransforms> UvTransforms;
	ToMergeParams(Params.MeshSectionMappings, SectionMappings);
	ToMergeParams(Params.UVTransformsPerMesh, UvTransforms);
	bool bRunDuplicateCheck = false;
	USkeletalMesh* BaseMesh = NewObject<USkeletalMesh>();
	if (Params.Skeleton && Params.bSkeletonBefore)
	{
		BaseMesh->Skeleton = Params.Skeleton;
		bRunDuplicateCheck = true;
		for (USkeletalMeshSocket* Socket : BaseMesh->GetMeshOnlySocketList())
		{
			if (Socket)
			{
				UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocket: %s"), *(Socket->SocketName.ToString()));
			}
		}
		for (USkeletalMeshSocket* Socket : BaseMesh->Skeleton->Sockets)
		{
			if (Socket)
			{
				UE_LOG(LogTemp, Warning, TEXT("SkelSocket: %s"), *(Socket->SocketName.ToString()));
			}
		}
	}
	FSkeletalMeshMerge Merger(BaseMesh, MeshesToMergeCopy, SectionMappings,
		Params.StripTopLODS, BufferAccess, UvTransforms.GetData());
	if (!Merger.DoMerge())
	{
		UE_LOG(LogTemp, Warning, TEXT("Merge failed!"));
		return nullptr;
	}
	if (Params.Skeleton && !Params.bSkeletonBefore)
	{
		BaseMesh->Skeleton = Params.Skeleton;
	}
	if (bRunDuplicateCheck)
	{
		TArray<FName> SkelMeshSockets;
		TArray<FName> SkelSockets;
		for (USkeletalMeshSocket* Socket : BaseMesh->GetMeshOnlySocketList())
		{
			if (Socket)
			{
				SkelMeshSockets.Add(Socket->GetFName());
				UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocket: %s"), *(Socket->SocketName.ToString()));
			}
		}
		for (USkeletalMeshSocket* Socket : BaseMesh->Skeleton->Sockets)
		{
			if (Socket)
			{
				SkelSockets.Add(Socket->GetFName());
				UE_LOG(LogTemp, Warning, TEXT("SkelSocket: %s"), *(Socket->SocketName.ToString()));
			}
		}
		TSet<FName> UniqueSkelMeshSockets;
		TSet<FName> UniqueSkelSockets;
		UniqueSkelMeshSockets.Append(SkelMeshSockets);
		UniqueSkelSockets.Append(SkelSockets);
		int32 Total = SkelSockets.Num() + SkelMeshSockets.Num();
		int32 UniqueTotal = UniqueSkelMeshSockets.Num() + UniqueSkelSockets.Num();
		UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"),
			SkelMeshSockets.Num(), SkelSockets.Num(), Total);
		UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"),
			UniqueSkelMeshSockets.Num(), UniqueSkelSockets.Num(), UniqueTotal);
		UE_LOG(LogTemp, Warning, TEXT("Found Duplicates: %s"),
			*((Total != UniqueTotal) ? FString("True") : FString("False")));
	}
	return BaseMesh;
}