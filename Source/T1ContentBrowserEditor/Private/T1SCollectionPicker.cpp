// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "T1SCollectionPicker.h"
#include "T1SCollectionView.h"

#define LOCTEXT_NAMESPACE "T1ContentBrowserEditor"

void T1SCollectionPicker::Construct( const FArguments& InArgs )
{
	ChildSlot
	[
		SNew(SCollectionView)
		.AllowCollectionButtons(InArgs._CollectionPickerConfig.AllowCollectionButtons)
		.OnCollectionSelected(InArgs._CollectionPickerConfig.OnCollectionSelected)
		.AllowCollapsing( false )
		.AllowContextMenu( InArgs._CollectionPickerConfig.AllowRightClickMenu )
	];
}

#undef LOCTEXT_NAMESPACE
