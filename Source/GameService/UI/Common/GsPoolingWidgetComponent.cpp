// Fill out your copyright notice in the Description page of Project Settings.

#include "GsPoolingWidgetComponent.h"
#include "GSGameInstance.h"
#include "UI/GsUIManager.h"
#include "UI/GsWidgetPoolManager.h"
#include "Blueprint/UserWidget.h"
#include "Framework/Application/SlateApplication.h"


void UGsPoolingWidgetComponent::InitWidget()
{
	// 함수 내용 재정의. 풀 매니저를 사용하도록 함

	// Don't do any work if Slate is not initialized
	if (FSlateApplication::IsInitialized())
	{
		if (WidgetClass && Widget == nullptr && GetWorld())
		{
			UGsGameInstance* gameInst = GetWorld()->GetGameInstance<UGsGameInstance>();
			if (nullptr != gameInst)
			{
				UGsWidgetPoolManager* poolManager = gameInst->GetUIManager()->GetWidgetPoolManager();
				if (nullptr != poolManager)
				{
					Widget = poolManager->CreateOrGetWidget(GetWidgetClass());
				}
			}
		}

#if WITH_EDITOR
		if (Widget && !GetWorld()->IsGameWorld() && !bEditTimeUsable)
		{
			if (!GEnableVREditorHacks)
			{
				// Prevent native ticking of editor component previews
				Widget->SetDesignerFlags(EWidgetDesignFlags::Designing);
			}
		}
#endif
	}
}

void UGsPoolingWidgetComponent::ReleaseResources()
{
	// 함수 재정의. 위젯 풀에 반납하는 코드. 상위 클래스에서 RemoveFromParent 수행
	UGsGameInstance* gameInst = GetWorld()->GetGameInstance<UGsGameInstance>();
	if (nullptr != gameInst)
	{
		UGsWidgetPoolManager* poolManager = gameInst->GetUIManager()->GetWidgetPoolManager();
		if (nullptr != poolManager)
		{
			poolManager->ReleaseWidget(GetWidgetClass(), Widget);
		}
	}

	Super::ReleaseResources();
}

