// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateFwd.h"
#include "Misc/Attribute.h"
#include "Layout/Geometry.h"
#include "Input/Reply.h"
#include "Layout/Visibility.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Styling/SlateColor.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetData.h"
#include "AssetThumbnail.h"
#include "Fonts/SlateFontInfo.h"
#include "T1ContentBrowserEditor/Public/T1ContentBrowserDelegates.h"
#include "Widgets/Views/STableViewBase.h"
#include "Types/SlateStructs.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Views/STableRow.h"
#include "T1AssetViewSortManager.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorStyleSet.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STileView.h"

class ST1AssetListItem;
class ST1AssetTileItem;
struct FT1AssetViewItem;

template <typename ItemType> class SListView;

DECLARE_DELEGATE_ThreeParams( FT1OnRenameBegin, const TSharedPtr<FT1AssetViewItem>& /*AssetItem*/, const FString& /*OriginalName*/, const FSlateRect& /*MessageAnchor*/)
DECLARE_DELEGATE_FourParams( FT1OnRenameCommit, const TSharedPtr<FT1AssetViewItem>& /*AssetItem*/, const FString& /*NewName*/, const FSlateRect& /*MessageAnchor*/, ETextCommit::Type /*CommitType*/ )
DECLARE_DELEGATE_RetVal_FourParams( bool, FT1OnVerifyRenameCommit, const TSharedPtr<FT1AssetViewItem>& /*AssetItem*/, const FText& /*NewName*/, const FSlateRect& /*MessageAnchor*/, FText& /*OutErrorMessage*/)
DECLARE_DELEGATE_OneParam( FT1OnItemDestroyed, const TSharedPtr<FT1AssetViewItem>& /*AssetItem*/);

class ST1AssetListItem;
class ST1AssetTileItem;

namespace FT1AssetViewModeUtils 
{
	FReply OnViewModeKeyDown( const TSet< TSharedPtr<FT1AssetViewItem> >& SelectedItems, const FKeyEvent& InKeyEvent );
}

struct FT1AssetViewItemHelper
{
public:
	static TSharedRef<SWidget> CreateListItemContents(ST1AssetListItem* const InListItem, const TSharedRef<SWidget>& InThumbnail, FName& OutItemShadowBorder);
	static TSharedRef<SWidget> CreateTileItemContents(ST1AssetTileItem* const InTileItem, const TSharedRef<SWidget>& InThumbnail, FName& OutItemShadowBorder);

private:
	template <typename T>
	static TSharedRef<SWidget> CreateListTileItemContents(T* const InTileOrListItem, const TSharedRef<SWidget>& InThumbnail, FName& OutItemShadowBorder);
};

/** The tile view mode of the asset view */
class ST1AssetTileView : public STileView<TSharedPtr<FT1AssetViewItem>>
{
public:
	virtual bool SupportsKeyboardFocus() const override { return true; }
	virtual FReply OnKeyDown( const FGeometry& InGeometry, const FKeyEvent& InKeyEvent ) override;
	virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;
};

/** The list view mode of the asset view */
class ST1AssetListView : public SListView<TSharedPtr<FT1AssetViewItem>>
{
public:
	virtual bool SupportsKeyboardFocus() const override { return true; }
	virtual FReply OnKeyDown( const FGeometry& InGeometry, const FKeyEvent& InKeyEvent ) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
};

/** The columns view mode of the asset view */
class ST1AssetColumnView : public SListView<TSharedPtr<FT1AssetViewItem>>
{
public:
	virtual bool SupportsKeyboardFocus() const override { return true; }
	virtual FReply OnKeyDown( const FGeometry& InGeometry, const FKeyEvent& InKeyEvent ) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
};

/** A base class for all asset view items */
class ST1AssetViewItem : public SCompoundWidget
{
	friend class ST1AssetViewItemToolTip;

public:
	DECLARE_DELEGATE_ThreeParams( FT1OnAssetsOrPathsDragDropped, const TArray<FAssetData>& /*AssetList*/, const TArray<FString>& /*AssetPaths*/, const FString& /*DestinationPath*/);
	DECLARE_DELEGATE_TwoParams( FT1OnFilesDragDropped, const TArray<FString>& /*FileNames*/, const FString& /*DestinationPath*/);

	SLATE_BEGIN_ARGS( ST1AssetViewItem )
		: _ShouldAllowToolTip(true)
		, _ThumbnailEditMode(false)
		{}

		/** Data for the asset this item represents */
		SLATE_ARGUMENT( TSharedPtr<FT1AssetViewItem>, AssetItem )

		/** Delegate for when an asset name has entered a rename state */
		SLATE_EVENT( FT1OnRenameBegin, OnRenameBegin )

		/** Delegate for when an asset name has been entered for an item that is in a rename state */
		SLATE_EVENT( FT1OnRenameCommit, OnRenameCommit )

		/** Delegate for when an asset name has been entered for an item to verify the name before commit */
		SLATE_EVENT( FT1OnVerifyRenameCommit, OnVerifyRenameCommit )

		/** Called when any asset item is destroyed. Used in thumbnail management */
		SLATE_EVENT( FT1OnItemDestroyed, OnItemDestroyed )

		/** If false, the tooltip will not be displayed */
		SLATE_ATTRIBUTE( bool, ShouldAllowToolTip )

		/** If true, display the thumbnail edit mode UI */
		SLATE_ATTRIBUTE( bool, ThumbnailEditMode )

		/** The string in the title to highlight (used when searching by string) */
		SLATE_ATTRIBUTE(FText, HighlightText)

		/** Delegate for when assets or asset paths are dropped on this item, if it is a folder */
		SLATE_EVENT( FT1OnAssetsOrPathsDragDropped, OnAssetsOrPathsDragDropped )

		/** Delegate for when a list of files is dropped on this folder (if it is a folder) from an external source */
		SLATE_EVENT( FT1OnFilesDragDropped, OnFilesDragDropped )

		/** Delegate to call (if bound) to check if it is valid to get a custom tooltip for this view item */
		SLATE_EVENT(FT1OnIsAssetValidForCustomToolTip, OnIsAssetValidForCustomToolTip)

		/** Delegate to call (if bound) to get a custom tooltip for this view item */
		SLATE_EVENT( FT1OnGetCustomAssetToolTip, OnGetCustomAssetToolTip )

		/** Delegate for when an item is about to show a tool tip */
		SLATE_EVENT( FT1OnVisualizeAssetToolTip, OnVisualizeAssetToolTip)

		/** Delegate for when an item's tooltip is about to close */
		SLATE_EVENT( FT1OnAssetToolTipClosing, OnAssetToolTipClosing )

	SLATE_END_ARGS()

	/** Virtual destructor */
	virtual ~ST1AssetViewItem();

	/** Performs common initialization logic for all asset view items */
	void Construct( const FArguments& InArgs );

	/** NOTE: Any functions overridden from the base widget classes *must* also be overridden by ST1AssetColumnViewRow and forwarded on to its internal item */
	virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;
	virtual TSharedPtr<IToolTip> GetToolTip() override;
	virtual void OnDragEnter( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent ) override;
	virtual void OnDragLeave( const FDragDropEvent& DragDropEvent ) override;
	virtual FReply OnDragOver( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent ) override;
	virtual FReply OnDrop( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent ) override;
	virtual bool OnVisualizeTooltip( const TSharedPtr<SWidget>& TooltipContent ) override;
	virtual void OnToolTipClosing() override;

	/** Returns the color this item should be tinted with */
	virtual FSlateColor GetAssetColor() const;

	/** Get the border image to display */
	const FSlateBrush* GetBorderImage() const;

	/** Get the name text to be displayed for this item */
	FText GetNameText() const;

	/** Delegate handling when an asset is loaded */
	void HandleAssetLoaded(UObject* InAsset) const;

protected:
	/** Used by OnDragEnter, OnDragOver, and OnDrop to check and update the validity of the drag operation */
	bool ValidateDragDrop( const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, bool& OutIsKnownDragOperation ) const;

	/** Check to see if the name should be read-only */
	bool IsNameReadOnly() const;

	/** Handles starting a name change */
	virtual void HandleBeginNameChange( const FText& OriginalText );

	/** Handles committing a name change */
	virtual void HandleNameCommitted( const FText& NewText, ETextCommit::Type CommitInfo );

	/** Handles verifying a name change */
	virtual bool HandleVerifyNameChanged( const FText& NewText, FText& OutErrorMessage );

	/** Handles committing a name change */
	virtual void OnAssetDataChanged();

	/** Notification for when the dirty flag changes */
	virtual void DirtyStateChanged();

	/** Gets the name of the class of this asset */
	FText GetAssetClassText() const;

	/** Gets the brush for the source control indicator image */
	const FSlateBrush* GetSCCStateImage() const;

	/** Gets the brush for the dirty indicator image */
	const FSlateBrush* GetDirtyImage() const;

	/** Gets the visibility for the thumbnail edit mode UI */
	EVisibility GetThumbnailEditModeUIVisibility() const;

	/** Creates a tooltip widget for this item */
	TSharedRef<SWidget> CreateToolTipWidget() const;

	/** Gets the visibility of the checked out by other text block in the tooltip */
	EVisibility GetCheckedOutByOtherTextVisibility() const;

	/** Gets the text for the checked out by other text block in the tooltip */
	FText GetCheckedOutByOtherText() const;

	/** Helper function for CreateToolTipWidget. Gets the user description for the asset, if it exists. */
	FText GetAssetUserDescription() const;

	/** Helper function for CreateToolTipWidget. Adds a key value pair to the info box of the tooltip */
	void AddToToolTipInfoBox(const TSharedRef<SVerticalBox>& InfoBox, const FText& Key, const FText& Value, bool bImportant) const;

	/** Updates the bPackageDirty flag */
	void UpdatePackageDirtyState();

	/** Returns true if the package that contains the asset that this item represents is dirty. */
	bool IsDirty() const;

	/** Update the source control state of this item if required */
	void UpdateSourceControlState(float InDeltaTime);

	/** Cache the package name from the asset we are representing */
	void CachePackageName();

	/** Cache the display tags for this item */
	void CacheDisplayTags();

	/** Whether this item is a folder */
	bool IsFolder() const;

	/** Set mips to be resident while this (loaded) asset is visible */
	void SetForceMipLevelsToBeResident(bool bForce) const;

	/** Delegate handler for when the source control provider changes */
	void HandleSourceControlProviderChanged(class ISourceControlProvider& OldProvider, class ISourceControlProvider& NewProvider);

	/** Delegate handler for when source control state changes */
	void HandleSourceControlStateChanged();

	/** Returns the width at which the name label will wrap the name */
	virtual float GetNameTextWrapWidth() const { return 0.0f; }

protected:
	/** Data for a cached display tag for this item (used in the tooltip, and also as the display string in column views) */
	struct FTagDisplayItem
	{
		FTagDisplayItem(FName InTagKey, FText InDisplayKey, FText InDisplayValue, const bool InImportant)
			: TagKey(InTagKey)
			, DisplayKey(MoveTemp(InDisplayKey))
			, DisplayValue(MoveTemp(InDisplayValue))
			, bImportant(InImportant)
		{
		}

		FName TagKey;
		FText DisplayKey;
		FText DisplayValue;
		bool bImportant;
	};

	TSharedPtr< SInlineEditableTextBlock > InlineRenameWidget;

	/** The data for this item */
	TSharedPtr<FT1AssetViewItem> AssetItem;

	/** The package containing the asset that this item represents */
	TWeakObjectPtr<UPackage> AssetPackage;

	/** The asset type actions associated with the asset that this item represents */
	TWeakPtr<IAssetTypeActions> AssetTypeActions;

	/** The cached name of the package containing the asset that this item represents */
	FString CachedPackageName;

	/** The cached filename of the package containing the asset that this item represents */
	FString CachedPackageFileName;

	/** The cached display tags for this item */
	TArray<FTagDisplayItem> CachedDisplayTags;

	/** Delegate for when an asset name has entered a rename state */
	FT1OnRenameBegin OnRenameBegin;

	/** Delegate for when an asset name has been entered for an item that is in a rename state */
	FT1OnRenameCommit OnRenameCommit;

	/** Delegate for when an asset name has been entered for an item to verify the name before commit */
	FT1OnVerifyRenameCommit OnVerifyRenameCommit;

	/** Called when any asset item is destroyed. Used in thumbnail management */
	FT1OnItemDestroyed OnItemDestroyed;

	/** Called to test if it is valid to make a custom tool tip for that asset */
	FT1OnIsAssetValidForCustomToolTip OnIsAssetValidForCustomToolTip;

	/** Called if bound to get a custom asset item tooltip */
	FT1OnGetCustomAssetToolTip OnGetCustomAssetToolTip;

	/** Called if bound when about to show a tooltip */
	FT1OnVisualizeAssetToolTip OnVisualizeAssetToolTip;

	/** Called if bound when a tooltip is closing */
	FT1OnAssetToolTipClosing OnAssetToolTipClosing;

	/** The geometry last frame. Used when telling popup messages where to appear. */
	FGeometry LastGeometry;

	/** If false, the tooltip will not be displayed */
	TAttribute<bool> ShouldAllowToolTip;

	/** If true, display the thumbnail edit mode UI */
	TAttribute<bool> ThumbnailEditMode;

	/** The substring to be highlighted in the name and tooltip path */
	TAttribute<FText> HighlightText;

	/** Cached brushes for the dirty state */
	const FSlateBrush* AssetDirtyBrush;

	/** Cached flag describing if the package is dirty */
	bool bPackageDirty;

	/** Flag indicating whether we have requested initial source control state */
	bool bSourceControlStateRequested;

	/** Delay timer before we request a source control state update, to prevent spam */
	float SourceControlStateDelay;

	/** Delegate for when a list of assets or asset paths are dropped on this item, if it is a folder */
	FT1OnAssetsOrPathsDragDropped OnAssetsOrPathsDragDropped;

	/** Delegate for when a list of files is dropped on this item (if it is a folder) from an external source */
	FT1OnFilesDragDropped OnFilesDragDropped;

	/** True when a drag is over this item with a drag operation that we know how to handle. The operation itself may not be valid to drop. */
	bool bDraggedOver;

	/** Cached brush for the source control state */
	const FSlateBrush* SCCStateBrush;

	/** Delegate handle for the HandleSourceControlStateChanged function callback */
	FDelegateHandle SourceControlStateChangedDelegateHandle;
};


/** An item in the asset list view */
class ST1AssetListItem : public ST1AssetViewItem
{
	friend struct FT1AssetViewItemHelper;

public:
	SLATE_BEGIN_ARGS( ST1AssetListItem )
		: _ThumbnailPadding(0)
		, _ThumbnailLabel( EThumbnailLabel::ClassName )
		, _ThumbnailHintColorAndOpacity( FLinearColor( 0.0f, 0.0f, 0.0f, 0.0f ) )
		, _ItemHeight(16)
		, _ShouldAllowToolTip(true)
		, _ThumbnailEditMode(false)
		, _AllowThumbnailHintLabel(false)
		{}

		/** The handle to the thumbnail this item should render */
		SLATE_ARGUMENT( TSharedPtr<FAssetThumbnail>, AssetThumbnail)

		/** Data for the asset this item represents */
		SLATE_ARGUMENT( TSharedPtr<FT1AssetViewItem>, AssetItem )

		/** How much padding to allow around the thumbnail */
		SLATE_ARGUMENT( float, ThumbnailPadding )

		/** The contents of the label displayed on the thumbnail */
		SLATE_ARGUMENT( EThumbnailLabel::Type, ThumbnailLabel )

		/**  */
		SLATE_ATTRIBUTE( FLinearColor, ThumbnailHintColorAndOpacity )

		/** The height of the list item */
		SLATE_ATTRIBUTE( float, ItemHeight )

		/** Delegate for when an asset name has entered a rename state */
		SLATE_EVENT( FT1OnRenameBegin, OnRenameBegin )

		/** Delegate for when an asset name has been entered for an item that is in a rename state */
		SLATE_EVENT( FT1OnRenameCommit, OnRenameCommit )

		/** Delegate for when an asset name has been entered for an item to verify the name before commit */
		SLATE_EVENT( FT1OnVerifyRenameCommit, OnVerifyRenameCommit )

		/** Called when any asset item is destroyed. Used in thumbnail management */
		SLATE_EVENT( FT1OnItemDestroyed, OnItemDestroyed )

		/** If false, the tooltip will not be displayed */
		SLATE_ATTRIBUTE( bool, ShouldAllowToolTip )

		/** The string in the title to highlight (used when searching by string) */
		SLATE_ATTRIBUTE( FText, HighlightText )

		/** If true, the thumbnail in this item can be edited */
		SLATE_ATTRIBUTE( bool, ThumbnailEditMode )

		/** Whether the thumbnail should ever show it's hint label */
		SLATE_ARGUMENT( bool, AllowThumbnailHintLabel )

		/** Whether the item is selected in the view */
		SLATE_ARGUMENT( FIsSelected, IsSelected )

		/** Delegate for when assets or asset paths are dropped on this item, if it is a folder */
		SLATE_EVENT( FT1OnAssetsOrPathsDragDropped, OnAssetsOrPathsDragDropped )

		/** Delegate for when a list of files is dropped on this folder (if it is a folder) from an external source */
		SLATE_EVENT( FT1OnFilesDragDropped, OnFilesDragDropped )

		/** Delegate to call (if bound) to check if it is valid to get a custom tooltip for this view item */
		SLATE_EVENT(FT1OnIsAssetValidForCustomToolTip, OnIsAssetValidForCustomToolTip)

		/** Delegate to request a custom tool tip if necessary */
		SLATE_EVENT(FT1OnGetCustomAssetToolTip, OnGetCustomAssetToolTip)

		/* Delegate to signal when the item is about to show a tooltip */
		SLATE_EVENT(FT1OnVisualizeAssetToolTip, OnVisualizeAssetToolTip)

		/** Delegate for when an item's tooltip is about to close */
		SLATE_EVENT( FT1OnAssetToolTipClosing, OnAssetToolTipClosing )

	SLATE_END_ARGS()

	/** Destructor */
	~ST1AssetListItem();

	/** Constructs this widget with InArgs */
	void Construct( const FArguments& InArgs );

	/** Handles committing a name change */
	virtual void OnAssetDataChanged() override;

	/** Whether the widget should allow primitive tools to be displayed */
	bool CanDisplayPrimitiveTools() const { return false; }

private:
	/** Returns the size of the thumbnail widget */
	FOptionalSize GetThumbnailBoxSize() const;

	/** Returns the size of the source control state box widget */
	FOptionalSize GetSCCImageSize() const;

private:
	/** The handle to the thumbnail that this item is rendering */
	TSharedPtr<FAssetThumbnail> AssetThumbnail;

	/** The height allowed for this item */
	TAttribute<float> ItemHeight;

	/** The text block containing the class name */
	TSharedPtr<STextBlock> ClassText;
};

/** An item in the asset tile view */
class ST1AssetTileItem : public ST1AssetViewItem
{
	friend struct FT1AssetViewItemHelper;

public:
	SLATE_BEGIN_ARGS( ST1AssetTileItem )
		: _ThumbnailPadding(0)
		, _ThumbnailLabel( EThumbnailLabel::ClassName )
		, _ThumbnailHintColorAndOpacity( FLinearColor( 0.0f, 0.0f, 0.0f, 0.0f ) )
		, _AllowThumbnailHintLabel(true)
		, _ItemWidth(16)
		, _ShouldAllowToolTip(true)
		, _ThumbnailEditMode(false)
	{}

		/** The handle to the thumbnail this item should render */
		SLATE_ARGUMENT( TSharedPtr<FAssetThumbnail>, AssetThumbnail)

		/** Data for the asset this item represents */
		SLATE_ARGUMENT( TSharedPtr<FT1AssetViewItem>, AssetItem )

		/** How much padding to allow around the thumbnail */
		SLATE_ARGUMENT( float, ThumbnailPadding )

		/** The contents of the label displayed on the thumbnail */
		SLATE_ARGUMENT( EThumbnailLabel::Type, ThumbnailLabel )

		/**  */
		SLATE_ATTRIBUTE( FLinearColor, ThumbnailHintColorAndOpacity )

		/** Whether the thumbnail should ever show it's hint label */
		SLATE_ARGUMENT( bool, AllowThumbnailHintLabel )

		/** The width of the item */
		SLATE_ATTRIBUTE( float, ItemWidth )

		/** Delegate for when an asset name has entered a rename state */
		SLATE_EVENT( FT1OnRenameBegin, OnRenameBegin )

		/** Delegate for when an asset name has been entered for an item that is in a rename state */
		SLATE_EVENT( FT1OnRenameCommit, OnRenameCommit )

		/** Delegate for when an asset name has been entered for an item to verify the name before commit */
		SLATE_EVENT( FT1OnVerifyRenameCommit, OnVerifyRenameCommit )

		/** Called when any asset item is destroyed. Used in thumbnail management */
		SLATE_EVENT( FT1OnItemDestroyed, OnItemDestroyed )

		/** If false, the tooltip will not be displayed */
		SLATE_ATTRIBUTE( bool, ShouldAllowToolTip )

		/** The string in the title to highlight (used when searching by string) */
		SLATE_ATTRIBUTE( FText, HighlightText )

		/** If true, the thumbnail in this item can be edited */
		SLATE_ATTRIBUTE( bool, ThumbnailEditMode )

		/** Whether the item is selected in the view */
		SLATE_ARGUMENT( FIsSelected, IsSelected )

		/** Delegate for when assets or asset paths are dropped on this item, if it is a folder */
		SLATE_EVENT( FT1OnAssetsOrPathsDragDropped, OnAssetsOrPathsDragDropped )

		/** Delegate for when a list of files is dropped on this folder (if it is a folder) from an external source */
		SLATE_EVENT( FT1OnFilesDragDropped, OnFilesDragDropped )

		/** Delegate to call (if bound) to check if it is valid to get a custom tooltip for this view item */
		SLATE_EVENT(FT1OnIsAssetValidForCustomToolTip, OnIsAssetValidForCustomToolTip)

		/** Delegate to request a custom tool tip if necessary */
		SLATE_EVENT(FT1OnGetCustomAssetToolTip, OnGetCustomAssetToolTip)

		/* Delegate to signal when the item is about to show a tooltip */
		SLATE_EVENT(FT1OnVisualizeAssetToolTip, OnVisualizeAssetToolTip)

		/** Delegate for when an item's tooltip is about to close */
		SLATE_EVENT( FT1OnAssetToolTipClosing, OnAssetToolTipClosing )

	SLATE_END_ARGS()

	/** Destructor */
	~ST1AssetTileItem();

	/** Constructs this widget with InArgs */
	void Construct( const FArguments& InArgs );

	/** Handles committing a name change */
	virtual void OnAssetDataChanged() override;

	/** Whether the widget should allow primitive tools to be displayed */
	bool CanDisplayPrimitiveTools() const { return true; }

protected:
	/** ST1AssetViewItem interface */
	virtual float GetNameTextWrapWidth() const override { return LastGeometry.GetLocalSize().X - 2.f; }

	/** Returns the size of the thumbnail box widget */
	FOptionalSize GetThumbnailBoxSize() const;

	/** Returns the font to use for the thumbnail label */
	FSlateFontInfo GetThumbnailFont() const;

	/** Returns the size of the source control state box widget */
	FOptionalSize GetSCCImageSize() const;

private:
	/** The handle to the thumbnail that this item is rendering */
	TSharedPtr<FAssetThumbnail> AssetThumbnail;

	/** The width of the item. Used to enforce a square thumbnail. */
	TAttribute<float> ItemWidth;

	/** The padding allotted for the thumbnail */
	float ThumbnailPadding;
};

/** An item in the asset column view */
class ST1AssetColumnItem : public ST1AssetViewItem
{
public:
	SLATE_BEGIN_ARGS( ST1AssetColumnItem ) {}

		/** Data for the asset this item represents */
		SLATE_ARGUMENT( TSharedPtr<FT1AssetViewItem>, AssetItem )

		/** Delegate for when an asset name has entered a rename state */
		SLATE_EVENT( FT1OnRenameBegin, OnRenameBegin )

		/** Delegate for when an asset name has been entered for an item that is in a rename state */
		SLATE_EVENT( FT1OnRenameCommit, OnRenameCommit )

		/** Delegate for when an asset name has been entered for an item to verify the name before commit */
		SLATE_EVENT( FT1OnVerifyRenameCommit, OnVerifyRenameCommit )

		/** Called when any asset item is destroyed. Used in thumbnail management, though it may be used for more so It is in column items for consistency. */
		SLATE_EVENT( FT1OnItemDestroyed, OnItemDestroyed )

		/** The string in the title to highlight (used when searching by string) */
		SLATE_ATTRIBUTE( FText, HighlightText )

		/** Delegate for when assets or asset paths are dropped on this item, if it is a folder */
		SLATE_EVENT( FT1OnAssetsOrPathsDragDropped, OnAssetsOrPathsDragDropped )

		/** Delegate for when a list of files is dropped on this folder (if it is a folder) from an external source */
		SLATE_EVENT( FT1OnFilesDragDropped, OnFilesDragDropped )

		/** Delegate to call (if bound) to check if it is valid to get a custom tooltip for this view item */
		SLATE_EVENT(FT1OnIsAssetValidForCustomToolTip, OnIsAssetValidForCustomToolTip)

		/** Delegate to request a custom tool tip if necessary */
		SLATE_EVENT( FT1OnGetCustomAssetToolTip, OnGetCustomAssetToolTip)

		/* Delegate to signal when the item is about to show a tooltip */
		SLATE_EVENT( FT1OnVisualizeAssetToolTip, OnVisualizeAssetToolTip)

		/** Delegate for when an item's tooltip is about to close */
		SLATE_EVENT( FT1OnAssetToolTipClosing, OnAssetToolTipClosing )

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct( const FArguments& InArgs );

	/** Generates a widget for a particular column */
	virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& ColumnName, FIsSelected InIsSelected );

	/** Handles committing a name change */
	virtual void OnAssetDataChanged() override;

private:
	/** Gets the tool tip text for the name */
	FString GetAssetNameToolTipText() const;

	/** Gets the path to this asset */
	FText GetAssetPathText() const;

	/** Gets the value for the specified tag in this asset */
	FText GetAssetTagText(FName Tag) const;

private:
	TAttribute<FText> HighlightText;

	TSharedPtr<STextBlock> ClassText;
	TSharedPtr<STextBlock> PathText;
};

class ST1AssetColumnViewRow : public SMultiColumnTableRow< TSharedPtr<FT1AssetViewItem> >
{
public:
	SLATE_BEGIN_ARGS( ST1AssetColumnViewRow )
		{}

		SLATE_EVENT( FOnDragDetected, OnDragDetected )
		SLATE_ARGUMENT( TSharedPtr<ST1AssetColumnItem>, AssetColumnItem )

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct( const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView )
	{
		this->AssetColumnItem = InArgs._AssetColumnItem;
		ensure(this->AssetColumnItem.IsValid());

		SMultiColumnTableRow< TSharedPtr<FT1AssetViewItem> >::Construct( 
			FSuperRowType::FArguments()
				.Style(FEditorStyle::Get(), "ContentBrowser.AssetListView.TableRow")
				.OnDragDetected(InArgs._OnDragDetected), 
			InOwnerTableView);
		Content = this->AssetColumnItem;
	}

	virtual TSharedRef<SWidget> GenerateWidgetForColumn( const FName& ColumnName ) override
	{
		if ( this->AssetColumnItem.IsValid() )
		{
			return this->AssetColumnItem->GenerateWidgetForColumn(ColumnName, FIsSelected::CreateSP( this, &ST1AssetColumnViewRow::IsSelectedExclusively ));
		}
		else
		{
			return SNew(STextBlock) .Text( NSLOCTEXT("AssetView", "InvalidColumnId", "Invalid Column Item") );
		}
		
	}

	virtual FVector2D GetRowSizeForColumn(const FName& InColumnName) const override
	{
		const TSharedRef<SWidget>* ColumnWidget = GetWidgetFromColumnId(InColumnName);

		if (ColumnWidget != nullptr)
		{
			return (*ColumnWidget)->GetDesiredSize();
		}

		return FVector2D::ZeroVector; 
	}

	virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override
	{
		this->AssetColumnItem->Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	}

	virtual TSharedPtr<IToolTip> GetToolTip() override
	{
		return AssetColumnItem->GetToolTip();
	}

	virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override
	{
		AssetColumnItem->OnDragEnter(MyGeometry, DragDropEvent);
	}

	virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override
	{
		AssetColumnItem->OnDragLeave(DragDropEvent);
	}

	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override
	{
		return AssetColumnItem->OnDragOver(MyGeometry, DragDropEvent);
	}

	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override
	{
		return AssetColumnItem->OnDrop(MyGeometry, DragDropEvent);
	}

	virtual bool OnVisualizeTooltip(const TSharedPtr<SWidget>& TooltipContent) override
	{
		// We take the content from the asset column item during construction,
		// so let the item handle the tooltip callback
		return AssetColumnItem->OnVisualizeTooltip(TooltipContent);
	}

	virtual void OnToolTipClosing() override
	{
		AssetColumnItem->OnToolTipClosing();
	}

	TSharedPtr<ST1AssetColumnItem> AssetColumnItem;
};
