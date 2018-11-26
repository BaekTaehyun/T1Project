// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class ITableRow;
class STableViewBase;

struct FT1MetaDataLine;

/**
 * The widget to display metadata as a table of tag/value rows
 */
class T1SMetaDataView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(T1SMetaDataView)	{}
	SLATE_END_ARGS()

	/**
	 * Construct this widget.  Called by the SNew() Slate macro.
	 *
	 * @param	InArgs				Declaration used by the SNew() macro to construct this widget
	 * @param	InMetaData			The metadata tags/values to display in the table view widget
	 */
	void Construct(const FArguments& InArgs, const TMap<FName, FString>& InMetadata);

private:
	TArray< TSharedPtr< FT1MetaDataLine > > MetaDataLines;

	TSharedRef< ITableRow > OnGenerateRow(const TSharedPtr< FT1MetaDataLine > Item, const TSharedRef< STableViewBase >& OwnerTable);
};
