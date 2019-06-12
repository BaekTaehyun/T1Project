// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorTerrainWidget.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Editor/UnrealEd/Public/LevelEditorViewport.h"
#include "Editor/UnrealEd/Classes/Editor/EditorEngine.h"
#include "Editor/UnrealEd/Public/Editor.h"

void UGsEditorTerrainWidget::NativeTick(const FGeometry& in_geometry, float in_delta)
{
	Super::NativeTick(in_geometry, in_delta);

//#if WITH_EDITOR
//	FLevelEditorViewportClient* Client = (FLevelEditorViewportClient*)(GEditor->GetActiveViewport()->GetClient());
//
//	if (Client)
//	{
//		FRotator rot = UKismetMathLibrary::FindLookAtRotation(worldlocatio(), Client->GetViewLocation());
//
//		SetWorldRotation(rot);
//	}
//#endif
}