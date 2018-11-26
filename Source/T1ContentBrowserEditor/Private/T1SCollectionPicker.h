// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "T1ContentBrowserEditor/Public/IT1ContentBrowserSingleton.h"

/**
 * A sources view designed for collection picking
 */
class T1SCollectionPicker : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( T1SCollectionPicker ){}

		/** A struct containing details about how the collection picker should behave */
		SLATE_ARGUMENT(FT1CollectionPickerConfig, CollectionPickerConfig)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct( const FArguments& InArgs );
};
