// Fill out your copyright notice in the Description page of Project Settings.

#include "GsPoolingWidgetComponent.h"
#include "GSGameInstance.h"
#include "UI/GsUIManager.h"
#include "UI/GsWidgetPoolManager.h"
#include "Blueprint/UserWidget.h"
#include "Framework/Application/SlateApplication.h"


void UGsPoolingWidgetComponent::InitWidget()
{
	// �Լ� ���� ������. Ǯ �Ŵ����� ����ϵ��� ��

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
	// �Լ� ������. ���� Ǯ�� �ݳ��ϴ� �ڵ�. ���� Ŭ�������� RemoveFromParent ����
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

