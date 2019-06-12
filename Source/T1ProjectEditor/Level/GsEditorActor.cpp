// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorActor.h"
#include "Editor/UnrealEd/Public/LevelEditorViewport.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "T1ProjectEditor/Widget/GsEditorBaseWidget.h"

// Sets default values
AGsEditorActor::AGsEditorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));

	SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void AGsEditorActor::BeginPlay()
{
	Super::BeginPlay();

	if (_IsDestoryOnPlay)
	{
		Destroy();
	}	
}

#if WITH_EDITOR
bool AGsEditorActor::ShouldTickIfViewportsOnly() const
{
	return true;
}
#endif

void AGsEditorActor::Tick(float in_delta)
{
	Super::Tick(in_delta);

	if (_Widget)
	{
		if (_Widget->bVisible)
		{
			FLevelEditorViewportClient* Client = (FLevelEditorViewportClient*)(GEditor->GetActiveViewport()->GetClient());

			if (Client)
			{
				FRotator rot = UKismetMathLibrary::FindLookAtRotation(_Widget->GetComponentLocation(), Client->GetViewLocation());

				_Widget->SetWorldRotation(rot);
			}
		}
	}
}