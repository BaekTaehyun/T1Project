﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "GsPartsLocal.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Container/GsPartsDataContainerBase.h"
#include "GameObject/ObjectClass/GsGameObjectLocal.h"
#include "GameObject/ActorExtend/GsLocalCharacter.h"

void FGsPartsLocal::Initialize(UGsGameObjectBase* owner)
{
	Super::Initialize(owner);

	Local = Cast<UGsGameObjectLocal>(owner);
    if (Local)
    {
        ActorComponent = Local->GetLocalCharacter()->FindComponentByClass<UActorComponent>();
    }
}

void FGsPartsLocal::Attached()
{
	Super::Attached();

	if (USkeletalMesh* mesh = MergeParts())
	{
		USkeletalMeshComponent* MeshComponent = Local->GetLocalCharacter()->GetMesh();
		MeshComponent->SetSkeletalMesh(mesh);
	}
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

USkeletalMesh* FGsPartsLocal::MergeParts() const
{
	if (Parts.Num() > 0)
	{
		TArray<USkeletalMesh*> mergeMeshes;
		mergeMeshes.Empty(Parts.Num());

		for (auto el : Parts)
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

void FGsPartsLocal::ToMergeParams(const TArray<FSkelMeshMergeSectionMapping_BP>& InSectionMappings, TArray<FSkelMeshMergeSectionMapping>& OutSectionMappings)
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

void FGsPartsLocal::ToMergeParams(const TArray<FSkelMeshMergeUVTransformMapping>& InUVTransformsPerMesh, TArray<FSkelMeshMergeUVTransforms>& OutUVTransformsPerMesh)
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
	FSkeletalMeshMerge Merger(BaseMesh, MeshesToMergeCopy, SectionMappings, Params.StripTopLODS, BufferAccess, UvTransforms.GetData());
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
		UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), SkelMeshSockets.Num(), SkelSockets.Num(), Total);
		UE_LOG(LogTemp, Warning, TEXT("SkelMeshSocketCount: %d | SkelSocketCount: %d | Combined: %d"), UniqueSkelMeshSockets.Num(), UniqueSkelSockets.Num(), UniqueTotal);
		UE_LOG(LogTemp, Warning, TEXT("Found Duplicates: %s"), *((Total != UniqueTotal) ? FString("True") : FString("False")));
	}
	return BaseMesh;
}