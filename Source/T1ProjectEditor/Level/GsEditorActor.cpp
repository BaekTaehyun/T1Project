// Fill out your copyright notice in the Description page of Project Settings.


#include "GsEditorActor.h"
#include "Editor/UnrealEd/Public/LevelEditorViewport.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "Runtime/UMG/Public/Components/TextBlock.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "T1ProjectEditor/Widget/GsEditorWidget.h"

// Sets default values
AGsEditorActor::AGsEditorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	if (_Root)
	{
		RootComponent = _Root;
		_Root->SetRelativeLocation(FVector::ZeroVector);
	}

	_Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));

	if (_Widget)
	{
		_Widget->SetRelativeLocation(FVector(0, 0, _WidgetHeight));
	}
		
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

	
}

void AGsEditorActor::TickActor(float in_delta, ELevelTick in_type, FActorTickFunction& in_tickFuc)
{
	Super::TickActor(in_delta, in_type, in_tickFuc);

	if (in_type == ELevelTick::LEVELTICK_ViewportsOnly)
	{
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
	else if(in_type == ELevelTick::LEVELTICK_All)
	{
		if (_Widget)
		{
			if (_Widget->bVisible)
			{
				UWorld* world = GetWorld();

				if (world)
				{
					APlayerController* player = world->GetFirstPlayerController();

					if (player)
					{
						APlayerCameraManager* camera = player->PlayerCameraManager;

						if (camera)
						{
							FRotator rot = UKismetMathLibrary::FindLookAtRotation(_Widget->GetComponentLocation(), camera->GetCameraLocation());

							_Widget->SetWorldRotation(rot);
						}				
					}
				}
			}
		}
	}
}

void AGsEditorActor::InvalideWidgetPosition()
{
	if (_Widget)
	{
		_Widget->SetWorldLocation(GetActorLocation() + FVector::FVector(0, 0, _WidgetHeight));
	}	
}

void AGsEditorActor::InvalideWidgetText()
{
	if (_Widget)
	{
		UUserWidget* widgetObj = _Widget->GetUserWidgetObject();

		if (widgetObj)
		{
			UGsEditorWidget* widget = Cast<UGsEditorWidget>(widgetObj);

			if (widget)
			{
				if (widget->_DescriptionText)
				{
					widget->_DescriptionText->SetText(FText::FromString(_Description));
				}
			}
		}
	}	
}