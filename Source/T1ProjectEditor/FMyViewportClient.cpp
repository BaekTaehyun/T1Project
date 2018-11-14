
#include "FMyViewportClient.h"
#include "SMyViewport.h"
#include "FMyEditor.h"
#include "AdvancedPreviewScene.h"

#include "EditorStyleSet.h"
#include "Editor/EditorPerProjectUserSettings.h"
#include "AssetViewerSettings.h"


FMyViewportClient::FMyViewportClient(TWeakPtr<class FMyEditor> ParentIGCEditor, const TSharedRef<class FAdvancedPreviewScene>& AdvPreviewScene, const TSharedRef<class SMyViewport>& Viewport, UMyUI* ObjectToEdit)
	: FEditorViewportClient(nullptr, &AdvPreviewScene.Get(), StaticCastSharedRef<SEditorViewport>(Viewport))
	, MyEditorPtr(ParentIGCEditor)
	, MyEditorViewportPtr(Viewport)
	, MyObject(ObjectToEdit)
{
	SetViewMode(VMI_Lit);

	AdvancedPreviewScene = static_cast<FAdvancedPreviewScene*>(PreviewScene);

	SetViewLocation(FVector(0.0f, 3.0f, 2.0f));
	SetViewRotation(FRotator(-45.0f, -90.0f, 0.0f));
	SetViewLocationForOrbiting(FVector::ZeroVector, 500.0f);
}

FMyViewportClient::~FMyViewportClient()
{
}

void FMyViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);
}

void FMyViewportClient::Draw(const FSceneView * View, FPrimitiveDrawInterface * PDI)
{
	FEditorViewportClient::Draw(View, PDI);
}
