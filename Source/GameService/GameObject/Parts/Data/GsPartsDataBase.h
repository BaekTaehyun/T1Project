// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObject/Define/GsGameObjectDefine.h"
#include "GsPartsDataBase.generated.h"

class USkeletalMesh;

/**
 *
 */
USTRUCT(BlueprintType)
struct GAMESERVICE_API FGsPartsDataBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	EGsPartsType Type;	//PartsType

	UPROPERTY(EditAnywhere, meta = (AllowedClasses="SkeletalMesh"))
	FSoftObjectPath Path;
};

//게임에서 사용될 데이터 구조체
struct GAMESERVICE_API FGsCPartsData
{
	EGsPartsType Type;
	FSoftObjectPath Path;
	TSharedPtr<USkeletalMesh> Mesh;

	FGsCPartsData(EGsPartsType type, FSoftObjectPath path)
	{
		Type = type;
		Path = path;
	}

	void SetMesh(TSharedPtr<USkeletalMesh> mesh)
	{
		Mesh = mesh;
	}
};

//Mesh Merge 정보 구조체
USTRUCT(BlueprintType)
struct FSkelMeshMergeSectionMapping_BP
{
	GENERATED_BODY()
	/** Indices to final section entries of the merged skeletal mesh */
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<int32> SectionIDs;
};

USTRUCT(BlueprintType)
struct FSkelMeshMergeUVTransform
{
	GENERATED_BODY()
	/** A list of how UVs should be transformed on a given mesh, where index represents a specific UV channel. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FTransform> UVTransforms;
};


USTRUCT(BlueprintType)
struct FSkelMeshMergeUVTransformMapping
{
	GENERATED_BODY()
	/** For each UV channel on each mesh, how the UVS should be transformed. */
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FSkelMeshMergeUVTransform> UVTransformsPerMesh;
};


USTRUCT(BlueprintType)
struct FSkeletalMeshMergeParams
{
	GENERATED_BODY()
	FSkeletalMeshMergeParams()
	{
		MeshSectionMappings = TArray<FSkelMeshMergeSectionMapping_BP>();
		UVTransformsPerMesh = TArray<FSkelMeshMergeUVTransformMapping>();
		StripTopLODS = 0;
		bNeedsCpuAccess = false;
		bSkeletonBefore = false;
		Skeleton = nullptr;
	}
	// An optional array to map sections from the source meshes to merged section entries
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkelMeshMergeSectionMapping_BP> MeshSectionMappings;
	// An optional array to transform the UVs in each mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkelMeshMergeUVTransformMapping> UVTransformsPerMesh;
	// The list of skeletal meshes to merge.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USkeletalMesh*> MeshesToMerge;
	// The number of high LODs to remove from input meshes
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StripTopLODS;
	// Whether or not the resulting mesh needs to be accessed by the CPU for any reason (e.g. for spawning particle effects).
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 bNeedsCpuAccess : 1;
	// Update skeleton before merge. Otherwise, update after.
	// Skeleton must also be provided.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 bSkeletonBefore : 1;
	// Skeleton that will be used for the merged mesh.
	// Leave empty if the generated skeleton is OK.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeleton* Skeleton;
};
