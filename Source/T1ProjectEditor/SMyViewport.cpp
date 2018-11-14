
#include "SMyViewport.h"
#include "FMyViewportClient.h"
#include "AdvancedPreviewScene.h"

#define LOCTEXT_NAMESPACE "SMyViewport"

SMyViewport::SMyViewport()
	: PreviewScene(MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues())))
{

}

SMyViewport::~SMyViewport()
{
	if (MyViewportClient.IsValid())
	{
		MyViewportClient->Viewport = NULL;
	}
}

void SMyViewport::AddReferencedObjects(FReferenceCollector & Collector)
{
	Collector.AddReferencedObject(MyObject);
	Collector.AddReferencedObject(PreviewMeshComponent);
}

TSharedRef<class FAdvancedPreviewScene> SMyViewport::GetPreviewScene()
{
	return PreviewScene.ToSharedRef();
}

TSharedRef<FEditorViewportClient> SMyViewport::MakeEditorViewportClient()
{
	MyViewportClient = MakeShareable(new FMyViewportClient(MyEditorPtr, PreviewScene.ToSharedRef(), SharedThis(this), MyObject));

	return MyViewportClient.ToSharedRef();
}


void SMyViewport::Construct(const FArguments& InArgs)
{
	MyEditorPtr = InArgs._ParentMyEditor;
	MyObject = InArgs._ObjectToEdit;

	SEditorViewport::Construct(SEditorViewport::FArguments());

	ViewportOverlay->AddSlot()
		.VAlign(VAlign_Top)
		.HAlign(HAlign_Left)
		.Padding(FMargin(10.0f, 10.0f, 10.0f, 10.0f))
		[
			SAssignNew(OverlayTextVerticalBox, SVerticalBox)
		];

	OverlayTextVerticalBox->ClearChildren();
	OverlayTextVerticalBox->AddSlot()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("IGCWelcomeText", "Welcome To IGC 2018"))
		.TextStyle(FEditorStyle::Get(), TEXT("TextBlock.ShadowedText"))
		.ColorAndOpacity(FLinearColor::Red)
		];


	UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(NULL, TEXT("/Engine/EngineMeshes/SM_MatPreviewMesh_01.SM_MatPreviewMesh_01"), NULL, LOAD_None, NULL);
	UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(NULL, TEXT("/Engine/EditorMeshes/ColorCalibrator/M_ChromeBall.M_ChromeBall"), NULL, LOAD_None, NULL);

	PreviewMeshComponent = NewObject<UStaticMeshComponent>(GetTransientPackage(), NAME_None, RF_Transient);
	PreviewMeshComponent->SetStaticMesh(StaticMesh);
	PreviewMeshComponent->SetMaterial(0, BaseMaterial);

	FTransform Transform = FTransform::Identity;
	PreviewScene->AddComponent(PreviewMeshComponent, Transform);

	PreviewMeshComponent->SetSimulatePhysics(true);
}

#undef LOCTEXT_NAMESPACE