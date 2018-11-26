// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/** The struct representing an item in the asset tree */
struct FT1TreeItem : public TSharedFromThis<FT1TreeItem>
{
	/** The display name of the tree item (typically the same as FolderName, but may be localized for known folder types) */
	FText DisplayName;
	/** The leaf-name of the tree item folder */
	FString FolderName;
	/** The path of the tree item including the name */
	FString FolderPath;
	/** If true, this folder is in the process of being named */
	bool bNamingFolder;

	/** The children of this tree item */
	TArray< TSharedPtr<FT1TreeItem> > Children;

	/** The parent folder for this item */
	TWeakPtr<FT1TreeItem> Parent;

	/** Broadcasts whenever renaming a tree item is requested */
	DECLARE_MULTICAST_DELEGATE( FRenamedRequestEvent )

	/** Broadcasts whenever a rename is requested */
	FRenamedRequestEvent OnRenamedRequestEvent;

	/** Constructor */
	FT1TreeItem(FText InDisplayName, FString InFolderName, FString InFolderPath, TSharedPtr<FT1TreeItem> InParent, bool InNamingFolder = false)
		: DisplayName(MoveTemp(InDisplayName))
		, FolderName(MoveTemp(InFolderName))
		, FolderPath(MoveTemp(InFolderPath))
		, bNamingFolder(InNamingFolder)
		, Parent(MoveTemp(InParent))
		, bChildrenRequireSort(false)
	{}

	/** Returns true if this item is a child of the specified item */
	bool IsChildOf(const FT1TreeItem& InParent)
	{
		auto CurrentParent = Parent.Pin();
		while (CurrentParent.IsValid())
		{
			if (CurrentParent.Get() == &InParent)
			{
				return true;
			}

			CurrentParent = CurrentParent->Parent.Pin();
		}

		return false;
	}

	/** Returns the child item by name or NULL if the child does not exist */
	TSharedPtr<FT1TreeItem> GetChild(const FString& InChildFolderName) const
	{
		for (const auto& Child : Children)
		{
			if (Child->FolderName == InChildFolderName)
			{
				return Child;
			}
		}

		return nullptr;
	}

	/** Finds the child who's path matches the one specified */
	TSharedPtr<FT1TreeItem> FindItemRecursive(const FString& InFullPath)
	{
		if (InFullPath == FolderPath)
		{
			return SharedThis(this);
		}

		for (const auto& Child : Children)
		{
			const TSharedPtr<FT1TreeItem>& Item = Child->FindItemRecursive(InFullPath);
			if (Item.IsValid())
			{
				return Item;
			}
		}

		return nullptr;
	}

	/** Request that the children be sorted the next time someone calls SortChildrenIfNeeded */
	void RequestSortChildren()
	{
		bChildrenRequireSort = true;
	}

	/** Sort the children by name (but only if they need to be) */
	void SortChildrenIfNeeded()
	{
		if (bChildrenRequireSort)
		{
			Children.Sort([](TSharedPtr<FT1TreeItem> A, TSharedPtr<FT1TreeItem> B) -> bool
			{
				return A->DisplayName.ToString() < B->DisplayName.ToString();
			});

			bChildrenRequireSort = false;
		}
	}

private:
	/** If true, the children of this item need sorting */
	bool bChildrenRequireSort;
};
