// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "T1AssetViewSortManager.h"
#include "T1AssetViewTypes.h"

struct FT1CompareFAssetItemBase
{
public:
	/** Default constructor */
	FT1CompareFAssetItemBase(bool bInAscending, const FName& InTag) : bAscending(bInAscending), Tag(InTag) {}

	virtual ~FT1CompareFAssetItemBase() {}

	/** Sort function */
	FORCEINLINE bool operator()(const TSharedPtr<FT1AssetViewItem>& A, const TSharedPtr<FT1AssetViewItem>& B) const
	{
		bool bFolderComparisonResult = false;
		if (CompareFolderFirst(A, B, bFolderComparisonResult))
		{
			return bFolderComparisonResult;
		}
		return Compare(A, B);
	}

	/** Get the next comparisons array */
	FORCEINLINE TArray<TUniquePtr<FT1CompareFAssetItemBase>>& GetNextComparisons() const
	{
		return NextComparisons;
	}

	/** Set the next comparisons array */
	FORCEINLINE void SetNextComparisons(TArray<TUniquePtr<FT1CompareFAssetItemBase>>& InNextComparisons) const
	{
		check(NextComparisons.Num() == 0);
		NextComparisons = MoveTemp(InNextComparisons);
	}

protected:
	/** Comparison function, needs to be implemented by inheriting structs - by default just forwards to the next comparitor */
	FORCEINLINE virtual bool Compare(const TSharedPtr<FT1AssetViewItem>& A, const TSharedPtr<FT1AssetViewItem>& B) const { return CompareNext(A, B); }

private:
	/** Compare the folders of both assets */
	FORCEINLINE bool CompareFolderFirst(const TSharedPtr<FT1AssetViewItem>& A, const TSharedPtr<FT1AssetViewItem>& B, bool& bComparisonResult) const
	{
		if (A->GetType() == EAssetItemType::Folder)
		{
			if (B->GetType() == EAssetItemType::Folder)
			{
				const FText& ValueA = StaticCastSharedPtr<FT1AssetViewFolder>(A)->FolderName;
				const FText& ValueB = StaticCastSharedPtr<FT1AssetViewFolder>(B)->FolderName;
				const int32 Result = ValueA.CompareTo(ValueB);
				bComparisonResult = bAscending ? Result < 0 : Result > 0;
			}
			else
			{
				bComparisonResult = bAscending;
			}
			return true;
		}
		else if (B->GetType() == EAssetItemType::Folder)
		{
			bComparisonResult = !bAscending;
			return true;
		}
		return false;
	}

	/** Comparison function in the event of a tie*/
	FORCEINLINE bool CompareNext(const TSharedPtr<FT1AssetViewItem>& A, const TSharedPtr<FT1AssetViewItem>& B) const
	{
		if (NextComparisons.Num() > 0)
		{
			TUniquePtr<FT1CompareFAssetItemBase> CompareNext = MoveTemp(NextComparisons[0]);
			check(CompareNext);
			NextComparisons.RemoveAt(0);

			// Move all the remaining comparisons to the next comparitor
			CompareNext->SetNextComparisons(GetNextComparisons());
			const bool Result = CompareNext->Compare(A, B);

			// Move the comparisons back for further ties
			SetNextComparisons(CompareNext->GetNextComparisons());
			NextComparisons.Insert(MoveTemp(CompareNext), 0);

			return Result;
		}
		return true;	// default to true if a tie
	}

protected:
	/** Whether to sort ascending or descending */
	bool bAscending;

	/** The tag type we need to compare next */
	FName Tag;

private:
	/** The follow up sorting methods in the event of a tie */
	mutable TArray<TUniquePtr<FT1CompareFAssetItemBase>> NextComparisons;
};

struct FT1CompareFAssetItemByName : public FT1CompareFAssetItemBase
{
public:
	FT1CompareFAssetItemByName(bool bInAscending, const FName& InTag) : FT1CompareFAssetItemBase(bInAscending, InTag) {}
	virtual ~FT1CompareFAssetItemByName() {}

protected:
	FORCEINLINE virtual bool Compare(const TSharedPtr<FT1AssetViewItem>& A, const TSharedPtr<FT1AssetViewItem>& B) const override
	{
		const FName& ValueA = StaticCastSharedPtr<FT1AssetViewAsset>(A)->Data.AssetName;
		const FName& ValueB = StaticCastSharedPtr<FT1AssetViewAsset>(B)->Data.AssetName;
		const int32 Result = ValueA.Compare(ValueB);
		if (Result < 0)
		{
			return bAscending;
		}
		else if (Result > 0)
		{
			return !bAscending;
		}
		return FT1CompareFAssetItemBase::Compare(A, B);
	}
};

struct FT1CompareFAssetItemByClass : public FT1CompareFAssetItemBase
{
public:
	FT1CompareFAssetItemByClass(bool bInAscending, const FName& InTag) : FT1CompareFAssetItemBase(bInAscending, InTag) {}
	virtual ~FT1CompareFAssetItemByClass() {}

protected:
	FORCEINLINE virtual bool Compare(const TSharedPtr<FT1AssetViewItem>& A, const TSharedPtr<FT1AssetViewItem>& B) const override
	{
		const FName& ValueA = StaticCastSharedPtr<FT1AssetViewAsset>(A)->Data.AssetClass;
		const FName& ValueB = StaticCastSharedPtr<FT1AssetViewAsset>(B)->Data.AssetClass;
		const int32 Result = ValueA.Compare(ValueB);
		if (Result < 0)
		{
			return bAscending;
		}
		else if (Result > 0)
		{
			return !bAscending;
		}
		return FT1CompareFAssetItemBase::Compare(A, B);
	}
};

struct FT1CompareFAssetItemByPath : public FT1CompareFAssetItemBase
{
public:
	FT1CompareFAssetItemByPath(bool bInAscending, const FName& InTag) : FT1CompareFAssetItemBase(bInAscending, InTag) {}
	virtual ~FT1CompareFAssetItemByPath() {}

protected:
	FORCEINLINE virtual bool Compare(const TSharedPtr<FT1AssetViewItem>& A, const TSharedPtr<FT1AssetViewItem>& B) const override
	{
		const FName& ValueA = StaticCastSharedPtr<FT1AssetViewAsset>(A)->Data.PackagePath;
		const FName& ValueB = StaticCastSharedPtr<FT1AssetViewAsset>(B)->Data.PackagePath;
		const int32 Result = ValueA.Compare(ValueB);
		if (Result < 0)
		{
			return bAscending;
		}
		else if (Result > 0)
		{
			return !bAscending;
		}
		return FT1CompareFAssetItemBase::Compare(A, B);
	}
};

struct FT1CompareFAssetItemByTag : public FT1CompareFAssetItemBase
{
public:
	FT1CompareFAssetItemByTag(bool bInAscending, const FName& InTag) : FT1CompareFAssetItemBase(bInAscending, InTag) {}
	virtual ~FT1CompareFAssetItemByTag() {}

protected:
	FORCEINLINE virtual bool Compare(const TSharedPtr<FT1AssetViewItem>& A, const TSharedPtr<FT1AssetViewItem>& B) const override
	{
		// Depending if we're sorting ascending or descending it's quicker to flip the compares incase tags are missing
		FString Value1;
		const bool bFoundValue1 = bAscending ? StaticCastSharedPtr<FT1AssetViewAsset>(A)->GetTagValue(Tag, Value1) : StaticCastSharedPtr<FT1AssetViewAsset>(B)->GetTagValue(Tag, Value1);
		if (!bFoundValue1)
		{
			return true;
		}

		FString Value2;
		const bool bFoundValue2 = bAscending ? StaticCastSharedPtr<FT1AssetViewAsset>(B)->GetTagValue(Tag, Value2) : StaticCastSharedPtr<FT1AssetViewAsset>(A)->GetTagValue(Tag, Value2);
		if (!bFoundValue2)
		{
			return false;
		}

		const int32 Result = Value1.Compare(Value2, ESearchCase::IgnoreCase);
		if (Result < 0)
		{
			return true;
		}
		else if (Result > 0)
		{
			return false;
		}
		return FT1CompareFAssetItemBase::Compare(A, B);
	}
};

struct FT1CompareFAssetItemByTagNumerical : public FT1CompareFAssetItemBase
{
public:
	FT1CompareFAssetItemByTagNumerical(bool bInAscending, const FName& InTag) : FT1CompareFAssetItemBase(bInAscending, InTag) {}
	virtual ~FT1CompareFAssetItemByTagNumerical() {}

protected:
	FORCEINLINE virtual bool Compare(const TSharedPtr<FT1AssetViewItem>& A, const TSharedPtr<FT1AssetViewItem>& B) const override
	{
		// Depending if we're sorting ascending or descending it's quicker to flip the compares incase tags are missing
		FString Value1;
		const bool bFoundValue1 = bAscending ? StaticCastSharedPtr<FT1AssetViewAsset>(A)->GetTagValue(Tag, Value1) : StaticCastSharedPtr<FT1AssetViewAsset>(B)->GetTagValue(Tag, Value1);
		if (!bFoundValue1)
		{
			return true;
		}

		FString Value2;
		const bool bFoundValue2 = bAscending ? StaticCastSharedPtr<FT1AssetViewAsset>(B)->GetTagValue(Tag, Value2) : StaticCastSharedPtr<FT1AssetViewAsset>(A)->GetTagValue(Tag, Value2);
		if (!bFoundValue2)
		{
			return false;
		}

		float FloatValue1 = 0.0f, FloatValue2 = 0.0f;
		LexFromString(FloatValue1, *Value1);
		LexFromString(FloatValue2, *Value2);

		if (FloatValue1 < FloatValue2)
		{
			return true;
		}
		else if (FloatValue1 > FloatValue2)
		{
			return false;
		}
		return FT1CompareFAssetItemBase::Compare(A, B);
	}
};

struct FT1CompareFAssetItemByTagDimensional : public FT1CompareFAssetItemBase
{
public:
	FT1CompareFAssetItemByTagDimensional(bool bInAscending, const FName& InTag) : FT1CompareFAssetItemBase(bInAscending, InTag) {}
	virtual ~FT1CompareFAssetItemByTagDimensional() {}

protected:
	FORCEINLINE virtual bool Compare(const TSharedPtr<FT1AssetViewItem>& A, const TSharedPtr<FT1AssetViewItem>& B) const override
	{
		// Depending if we're sorting ascending or descending it's quicker to flip the compares incase tags are missing
		FString Value1;
		const bool bHasFoundValue1 = bAscending ? StaticCastSharedPtr<FT1AssetViewAsset>(A)->GetTagValue(Tag, Value1) : StaticCastSharedPtr<FT1AssetViewAsset>(B)->GetTagValue(Tag, Value1);
		if (!bHasFoundValue1)
		{
			return true;
		}

		FString Value2;
		const bool bHasFoundValue2 = bAscending ? StaticCastSharedPtr<FT1AssetViewAsset>(B)->GetTagValue(Tag, Value2) : StaticCastSharedPtr<FT1AssetViewAsset>(A)->GetTagValue(Tag, Value2);
		if (!bHasFoundValue2)
		{
			return false;
		}

		float Num1 = 1.f;
		{
			TArray<FString> Tokens1;
			Value1.ParseIntoArray(Tokens1, TEXT("x"), true);
			for (auto TokenIt1 = Tokens1.CreateConstIterator(); TokenIt1; ++TokenIt1)
			{
				Num1 *= FCString::Atof(**TokenIt1);
			}
		}
		float Num2 = 1.f;
		{
			TArray<FString> Tokens2;
			Value2.ParseIntoArray(Tokens2, TEXT("x"), true);
			for (auto TokenIt2 = Tokens2.CreateConstIterator(); TokenIt2; ++TokenIt2)
			{
				Num2 *= FCString::Atof(**TokenIt2);
			}
		}
		if (Num1 < Num2)
		{
			return true;
		}
		else if (Num1 > Num2)
		{
			return false;
		}
		return FT1CompareFAssetItemBase::Compare(A, B);
	}
};

const FName FT1AssetViewSortManager::NameColumnId = "Name";
const FName FT1AssetViewSortManager::ClassColumnId = "Class";
const FName FT1AssetViewSortManager::PathColumnId = "Path";

FT1AssetViewSortManager::FT1AssetViewSortManager()
{
	ResetSort();
}

void FT1AssetViewSortManager::ResetSort()
{
	SortColumnId[EColumnSortPriority::Primary] = NameColumnId;
	SortMode[EColumnSortPriority::Primary] = EColumnSortMode::Ascending;
	for (int32 PriorityIdx = 1; PriorityIdx < EColumnSortPriority::Max; PriorityIdx++)
	{
		SortColumnId[PriorityIdx] = NAME_None;
		SortMode[PriorityIdx] = EColumnSortMode::None;
	}
}

bool FT1AssetViewSortManager::FindAndRefreshCustomColumn(TArray<TSharedPtr<FT1AssetViewItem>>& AssetItems, FName ColumnName, const TArray<FT1AssetViewCustomColumn>& CustomColumns, UObject::FAssetRegistryTag::ETagType& TagType) const
{
	TagType = UObject::FAssetRegistryTag::ETagType::TT_Hidden;

	// Look in custom columns list
	for (const FT1AssetViewCustomColumn& Column : CustomColumns)
	{
		if (Column.ColumnName == ColumnName)
		{
			// Refresh the custom data now so it can sort

			for (TSharedPtr<struct FT1AssetViewItem> AssetItem : AssetItems)
			{
				if (!AssetItem.IsValid() || AssetItem->GetType() == EAssetItemType::Folder)
				{
					continue;
				}

				FT1AssetViewAsset* Asset = StaticCastSharedPtr<FT1AssetViewAsset>(AssetItem).Get();

				if (!Asset->CustomColumnData.Find(Column.ColumnName))
				{
					Asset->CustomColumnData.Add(Column.ColumnName, Column.OnGetColumnData.Execute(Asset->Data, Column.ColumnName));
				}
			}
			TagType = Column.DataType;
			return true;
		}
	}
	return false;
}

void FT1AssetViewSortManager::SortList(TArray<TSharedPtr<FT1AssetViewItem>>& AssetItems, const FName& MajorityAssetType, const TArray<FT1AssetViewCustomColumn>& CustomColumns) const
{
	//double SortListStartTime = FPlatformTime::Seconds();

	TArray<TUniquePtr<FT1CompareFAssetItemBase>> SortMethod;
	for (int32 PriorityIdx = 0; PriorityIdx < EColumnSortPriority::Max; PriorityIdx++)
	{
		const bool bAscending(SortMode[PriorityIdx] == EColumnSortMode::Ascending);
		const FName& Tag(SortColumnId[PriorityIdx]);

		if (Tag == NAME_None)
		{
			break;
		}

		if (Tag == NameColumnId)
		{
			SortMethod.Add(MakeUnique<FT1CompareFAssetItemByName>(bAscending, Tag));
		}
		else if (Tag == ClassColumnId)
		{
			SortMethod.Add(MakeUnique<FT1CompareFAssetItemByClass>(bAscending, Tag));
		}
		else if (Tag == PathColumnId)
		{
			SortMethod.Add(MakeUnique<FT1CompareFAssetItemByPath>(bAscending, Tag));
		}
		else
		{
			UObject::FAssetRegistryTag::ETagType TagType;
			bool bFoundCustomColumn = FindAndRefreshCustomColumn(AssetItems, Tag, CustomColumns, TagType);
			
			// Since this SortData.Tag is not one of preset columns, sort by asset registry tag	
			if ((!bFoundCustomColumn || TagType == UObject::FAssetRegistryTag::ETagType::TT_Hidden) && MajorityAssetType != NAME_None)
			{
				UClass* Class = FindObject<UClass>(ANY_PACKAGE, *MajorityAssetType.ToString());
				if (Class)
				{
					UObject* CDO = Class->GetDefaultObject();
					if (CDO)
					{
						TArray<UObject::FAssetRegistryTag> TagList;
						CDO->GetAssetRegistryTags(TagList);

						for (auto TagIt = TagList.CreateConstIterator(); TagIt; ++TagIt)
						{
							if (TagIt->Name == Tag)
							{
								TagType = TagIt->Type;
								break;
							}
						}
					}
				}
			}

			if (TagType == UObject::FAssetRegistryTag::TT_Numerical)
			{
				// The property is a Number, compare using atof
				SortMethod.Add(MakeUnique<FT1CompareFAssetItemByTagNumerical>(bAscending, Tag));
			}
			else if (TagType == UObject::FAssetRegistryTag::TT_Dimensional)
			{
				// The property is a series of Numbers representing dimensions, compare by using atof for each Number, delimited by an "x"
				SortMethod.Add(MakeUnique<FT1CompareFAssetItemByTagDimensional>(bAscending, Tag));
			}
			else if (TagType != UObject::FAssetRegistryTag::ETagType::TT_Hidden)
			{
				// Unknown or alphabetical, sort alphabetically either way
				SortMethod.Add(MakeUnique<FT1CompareFAssetItemByTag>(bAscending, Tag));
			}
		}
	}

	// Sort the list...
	if (SortMethod.Num() > 0)
	{
		TUniquePtr<FT1CompareFAssetItemBase> PrimarySortMethod = MoveTemp(SortMethod[EColumnSortPriority::Primary]);
		check(PrimarySortMethod);
		SortMethod.RemoveAt(0);

		// Move all the comparisons to the primary sort method
		PrimarySortMethod->SetNextComparisons(SortMethod);
		AssetItems.Sort(*(PrimarySortMethod.Get()));

		// Move the comparisons back for ease of cleanup
		SortMethod = MoveTemp(PrimarySortMethod->GetNextComparisons());
		SortMethod.Insert(MoveTemp(PrimarySortMethod), 0);
	}

	// Cleanup the methods we no longer need.
	for (int32 PriorityIdx = 0; PriorityIdx < SortMethod.Num(); PriorityIdx++)
	{
		SortMethod[PriorityIdx].Reset();
	}
	SortMethod.Empty();

	//UE_LOG(LogContentBrowser, Warning/*VeryVerbose*/, TEXT("FT1AssetViewSortManager Sort Time: %0.4f seconds."), FPlatformTime::Seconds() - SortListStartTime);
}

void FT1AssetViewSortManager::ExportColumnsToCSV(TArray<TSharedPtr<struct FT1AssetViewItem>>& AssetItems, TArray<FName>& ColumnList, const TArray<FT1AssetViewCustomColumn>& CustomColumns, FString& OutString) const
{
	// Write column headers
	for (FName Column : ColumnList)
	{
		OutString += Column.ToString();
		OutString += TEXT(",");

		UObject::FAssetRegistryTag::ETagType TagType;
		FindAndRefreshCustomColumn(AssetItems, Column, CustomColumns, TagType);
	}
	OutString += TEXT("\n");

	// Write each asset
	for (TSharedPtr<struct FT1AssetViewItem> AssetItem : AssetItems)
	{
		if (!AssetItem.IsValid() || AssetItem->GetType() == EAssetItemType::Folder)
		{
			continue;
		}

		FT1AssetViewAsset* Asset = StaticCastSharedPtr<FT1AssetViewAsset>(AssetItem).Get();

		for (FName Column : ColumnList)
		{
			FString ValueString;

			if (Column == NameColumnId)
			{
				ValueString = Asset->Data.AssetName.ToString();
			}
			else if (Column == ClassColumnId)
			{
				ValueString = Asset->Data.AssetClass.ToString();
			}
			else if (Column == PathColumnId)
			{
				ValueString = Asset->Data.PackagePath.ToString();
			}
			else
			{
				Asset->GetTagValue(Column, ValueString);
			}
			
			OutString += TEXT("\"");
			OutString += ValueString.Replace(TEXT("\""), TEXT("\"\""));
			OutString += TEXT("\",");
		}

		OutString += TEXT("\n");
	}
}

void FT1AssetViewSortManager::SetSortColumnId(const EColumnSortPriority::Type InSortPriority, const FName& InColumnId)
{
	check(InSortPriority < EColumnSortPriority::Max);
	SortColumnId[InSortPriority] = InColumnId;

	// Prevent the same ColumnId being assigned to multiple columns
	bool bOrderChanged = false;
	for (int32 PriorityIdxA = 0; PriorityIdxA < EColumnSortPriority::Max; PriorityIdxA++)
	{
		for (int32 PriorityIdxB = 0; PriorityIdxB < EColumnSortPriority::Max; PriorityIdxB++)
		{
			if (PriorityIdxA != PriorityIdxB)
			{
				if (SortColumnId[PriorityIdxA] == SortColumnId[PriorityIdxB] && SortColumnId[PriorityIdxB] != NAME_None)
				{
					SortColumnId[PriorityIdxB] = NAME_None;
					bOrderChanged = true;
				}
			}
		}
	}
	if (bOrderChanged)
	{
		// If the order has changed, we need to remove any unneeded sorts by bumping the priority of the remaining valid ones
		for (int32 PriorityIdxA = 0, PriorityNum = 0; PriorityNum < EColumnSortPriority::Max - 1; PriorityNum++, PriorityIdxA++)
		{
			if (SortColumnId[PriorityIdxA] == NAME_None)
			{
				for (int32 PriorityIdxB = PriorityIdxA; PriorityIdxB < EColumnSortPriority::Max - 1; PriorityIdxB++)
				{
					SortColumnId[PriorityIdxB] = SortColumnId[PriorityIdxB + 1];
					SortMode[PriorityIdxB] = SortMode[PriorityIdxB + 1];
				}
				SortColumnId[EColumnSortPriority::Max - 1] = NAME_None;
				PriorityIdxA--;
			}
		}
	}
}

void FT1AssetViewSortManager::SetSortMode(const EColumnSortPriority::Type InSortPriority, const EColumnSortMode::Type InSortMode)
{
	check(InSortPriority < EColumnSortPriority::Max);
	SortMode[InSortPriority] = InSortMode;
}

bool FT1AssetViewSortManager::SetOrToggleSortColumn(const EColumnSortPriority::Type InSortPriority, const FName& InColumnId)
{
	check(InSortPriority < EColumnSortPriority::Max);
	if (SortColumnId[InSortPriority] != InColumnId)
	{
		// Clicked a new column, default to ascending
		SortColumnId[InSortPriority] = InColumnId;
		SortMode[InSortPriority] = EColumnSortMode::Ascending;
		return true;
	}
	else
	{
		// Clicked the current column, toggle sort mode
		if (SortMode[InSortPriority] == EColumnSortMode::Ascending)
		{
			SortMode[InSortPriority] = EColumnSortMode::Descending;
		}
		else
		{
			SortMode[InSortPriority] = EColumnSortMode::Ascending;
		}
		return false;
	}
}
