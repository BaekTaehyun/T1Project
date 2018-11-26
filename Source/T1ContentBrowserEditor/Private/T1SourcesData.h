// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ARFilter.h"
#include "CollectionManagerTypes.h"
#include "ICollectionManager.h"
#include "CollectionManagerModule.h"

struct FT1SourcesData
{
	TArray<FName> PackagePaths;
	TArray<FCollectionNameType> Collections;

	FT1SourcesData()
		: PackagePaths()
		, Collections()
	{
	}

	explicit FT1SourcesData(FName InPackagePath)
		: PackagePaths()
		, Collections()
	{
		PackagePaths.Add(InPackagePath);
	}

	explicit FT1SourcesData(FCollectionNameType InCollection)
		: PackagePaths()
		, Collections()
	{
		Collections.Add(InCollection);
	}

	FT1SourcesData(TArray<FName> InPackagePaths, TArray<FCollectionNameType> InCollections)
		: PackagePaths(MoveTemp(InPackagePaths))
		, Collections(MoveTemp(InCollections))
	{
	}

	FT1SourcesData(const FT1SourcesData& Other)
		: PackagePaths(Other.PackagePaths)
		, Collections(Other.Collections)
	{
	}

	FT1SourcesData(FT1SourcesData&& Other)
		: PackagePaths(MoveTemp(Other.PackagePaths))
		, Collections(MoveTemp(Other.Collections))
	{
	}

	FT1SourcesData& operator=(const FT1SourcesData& Other)
	{
		if (this != &Other)
		{
			PackagePaths = Other.PackagePaths;
			Collections = Other.Collections;
		}
		return *this;
	}

	FT1SourcesData& operator=(FT1SourcesData&& Other)
	{
		if (this != &Other)
		{
			PackagePaths = MoveTemp(Other.PackagePaths);
			Collections = MoveTemp(Other.Collections);
		}
		return *this;
	}

	FORCEINLINE bool IsEmpty() const
	{
		return PackagePaths.Num() == 0 && Collections.Num() == 0;
	}

	FORCEINLINE bool HasPackagePaths() const
	{
		return PackagePaths.Num() > 0;
	}

	FORCEINLINE bool HasCollections() const
	{
		return Collections.Num() > 0;
	}

	bool IsDynamicCollection() const
	{
		if ( Collections.Num() == 1 && FCollectionManagerModule::IsModuleAvailable() )
		{
			// Collection manager module should already be loaded since it may cause a hitch on the first search
			FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();

			ECollectionStorageMode::Type StorageMode = ECollectionStorageMode::Static;
			return (CollectionManagerModule.Get().GetCollectionStorageMode(Collections[0].Name, Collections[0].Type, StorageMode) && StorageMode == ECollectionStorageMode::Dynamic);
		}

		return false;
	}

	FARFilter MakeFilter(bool bRecurse, bool bUsingFolders) const
	{
		FARFilter Filter;

		// Package Paths
		Filter.PackagePaths = PackagePaths;
		Filter.bRecursivePaths = bRecurse || !bUsingFolders;

		// If we have a dynamic source, then we need to make sure that the root path is searched for matching objects as the dynamic filter will sort through them
		if (IsDynamicCollection())
		{
			Filter.PackagePaths.AddUnique(TEXT("/"));
		}

		// Collections
		TArray<FName> ObjectPathsFromCollections;
		if ( Collections.Num() && FCollectionManagerModule::IsModuleAvailable() )
		{
			// Collection manager module should already be loaded since it may cause a hitch on the first search
			FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();

			// Include objects from child collections if we're recursing
			const ECollectionRecursionFlags::Flags CollectionRecursionMode = (Filter.bRecursivePaths) ? ECollectionRecursionFlags::SelfAndChildren : ECollectionRecursionFlags::Self;

			for ( int32 CollectionIdx = 0; CollectionIdx < Collections.Num(); ++CollectionIdx )
			{
				// Find the collection
				const FCollectionNameType& CollectionNameType = Collections[CollectionIdx];
				CollectionManagerModule.Get().GetObjectsInCollection(CollectionNameType.Name, CollectionNameType.Type, ObjectPathsFromCollections, CollectionRecursionMode);
			}
		}
		Filter.ObjectPaths = ObjectPathsFromCollections;

		return Filter;
	}
};
