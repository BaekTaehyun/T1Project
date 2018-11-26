// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetData.h"
#include "UObject/GCObject.h"
#include "Misc/Paths.h"
#include "T1ContentBrowserUtils.h"

class UFactory;

namespace EAssetItemType
{
	enum Type
	{
		Normal,
		Folder,
		Creation,
		Duplication
	};
}

/** Base class for items displayed in the asset view */
struct FT1AssetViewItem
{
	FT1AssetViewItem()
		: bRenameWhenScrolledIntoview(false)
	{
	}

	virtual ~FT1AssetViewItem() {}

	/** Get the type of this asset item */
	virtual EAssetItemType::Type GetType() const = 0;

	/** Get whether this is a temporary item */
	virtual bool IsTemporaryItem() const = 0;

	/** Updates cached custom column data, does nothing by default */
	virtual void CacheCustomColumns(const TArray<FT1AssetViewCustomColumn>& CustomColumns, bool bUpdateSortData, bool bUpdateDisplayText, bool bUpdateExisting) {}

	/** Broadcasts whenever a rename is requested */
	FSimpleDelegate RenamedRequestEvent;

	/** An event to fire when the asset data for this item changes */
	DECLARE_MULTICAST_DELEGATE( FOnAssetDataChanged );
	FOnAssetDataChanged OnAssetDataChanged;

	/** True if this item will enter inline renaming on the next scroll into view */
	bool bRenameWhenScrolledIntoview;
};

/** Item that represents an asset */
struct FT1AssetViewAsset : public FT1AssetViewItem
{
	/** The asset registry data associated with this item */
	FAssetData Data;

	/** Map of values for custom columns */
	TMap<FName, FString> CustomColumnData;

	/** Map of display text for custom columns */
	TMap<FName, FText> CustomColumnDisplayText;

	explicit FT1AssetViewAsset(const FAssetData& AssetData)
		: Data(AssetData)
	{}

	void SetAssetData(const FAssetData& NewData)
	{
		Data = NewData;
		OnAssetDataChanged.Broadcast();
	}

	bool GetTagValue(FName Tag, FString& OutString) const
	{
		const FString* FoundString = CustomColumnData.Find(Tag);

		if (FoundString)
		{
			OutString = *FoundString;
			return true;
		}

		return Data.GetTagValue(Tag, OutString);
	}

	// FT1AssetViewItem interface
	virtual EAssetItemType::Type GetType() const override
	{
		return EAssetItemType::Normal;
	}

	virtual bool IsTemporaryItem() const override
	{
		return false;
	}

	virtual void CacheCustomColumns(const TArray<FT1AssetViewCustomColumn>& CustomColumns, bool bUpdateSortData, bool bUpdateDisplayText, bool bUpdateExisting) override
	{
		for (const FT1AssetViewCustomColumn& Column : CustomColumns)
		{
			if (bUpdateSortData)
			{
				if (bUpdateExisting ? CustomColumnData.Contains(Column.ColumnName) : !CustomColumnData.Contains(Column.ColumnName))
				{
					CustomColumnData.Add(Column.ColumnName, Column.OnGetColumnData.Execute(Data, Column.ColumnName));
				}
			}

			if (bUpdateDisplayText)
			{
				if (bUpdateExisting ? CustomColumnDisplayText.Contains(Column.ColumnName) : !CustomColumnDisplayText.Contains(Column.ColumnName))
				{
					if (Column.OnGetColumnDisplayText.IsBound())
					{
						CustomColumnDisplayText.Add(Column.ColumnName, Column.OnGetColumnDisplayText.Execute(Data, Column.ColumnName));
					}
					else
					{
						CustomColumnDisplayText.Add(Column.ColumnName, FText::AsCultureInvariant(Column.OnGetColumnData.Execute(Data, Column.ColumnName)));
					}
				}
			}
		}
	}
};

/** Item that represents a folder */
struct FT1AssetViewFolder : public FT1AssetViewItem
{
	/** The folder this item represents */
	FString FolderPath;

	/** The folder this item represents, minus the preceding path */
	FText FolderName;

	/** Whether this is a developer folder */
	bool bDeveloperFolder;

	/** Whether this is a collection folder */
	bool bCollectionFolder;

	/** Whether this folder is a new folder */
	bool bNewFolder;

	FT1AssetViewFolder(const FString& InPath)
		: FolderPath(InPath)
		, bNewFolder(false)
	{
		FolderName = FText::FromString(FPaths::GetBaseFilename(FolderPath));
		bDeveloperFolder = T1ContentBrowserUtils::IsDevelopersFolder(FolderPath);
		bCollectionFolder = T1ContentBrowserUtils::IsCollectionPath(FolderPath);
	}

	/** Set the name of this folder (without path) */
	void SetFolderName(const FString& InName)
	{
		FolderPath = FPaths::GetPath(FolderPath) / InName;
		FolderName = FText::FromString(InName);
		OnAssetDataChanged.Broadcast();
	}

	// FT1AssetViewItem interface
	virtual EAssetItemType::Type GetType() const override
	{
		return EAssetItemType::Folder;
	}

	virtual bool IsTemporaryItem() const override
	{
		return false;
	}
};

/** Item that represents an asset that is being created */
struct FAssetViewCreation : public FT1AssetViewAsset, public FGCObject
{
	/** The class to use when creating the asset */
	UClass* AssetClass;

	/** The factory to use when creating the asset. */
	UFactory* Factory;

	FAssetViewCreation(const FAssetData& AssetData, UClass* InAssetClass, UFactory* InFactory)
		: FT1AssetViewAsset(AssetData)
		, AssetClass(InAssetClass)
		, Factory(InFactory)
	{}

	// FT1AssetViewItem interface
	virtual EAssetItemType::Type GetType() const override
	{
		return EAssetItemType::Creation;
	}

	virtual bool IsTemporaryItem() const override
	{
		return true;
	}

	// FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObject(AssetClass);
		Collector.AddReferencedObject(Factory);
	}
};

/** Item that represents an asset that is being duplicated */
struct FT1AssetViewDuplication : public FT1AssetViewAsset
{
	/** The context to use when creating the asset. Used when initializing an asset with another related asset. */
	TWeakObjectPtr<UObject> SourceObject;

	FT1AssetViewDuplication(const FAssetData& AssetData, const TWeakObjectPtr<UObject>& InSourceObject = NULL)
		: FT1AssetViewAsset(AssetData)
		, SourceObject(InSourceObject)
	{}

	// FT1AssetViewItem interface
	virtual EAssetItemType::Type GetType() const override
	{
		return EAssetItemType::Duplication;
	}

	virtual bool IsTemporaryItem() const override
	{
		return true;
	}
};
