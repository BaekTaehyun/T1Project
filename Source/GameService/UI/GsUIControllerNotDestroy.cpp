// Fill out your copyright notice in the Description page of Project Settings.
#include "GsUIControllerNotDestroy.h"


void UGsUIControllerNotDestroy::RemoveAll()
{
	for (auto* widget : UsingWidgetArray)
	{
		if (nullptr != widget)
		{
			// 파괴시킬 수 있게 전환
			widget->SetEnableAutoDestroy(true);

			if (widget->IsInViewport())
			{
				widget->RemoveFromParent();
			}

			widget = nullptr;
		}
	}

	StackableArray.Empty();
	UsingWidgetArray.Empty();
	CachedWidgetMap.Empty();
}

void UGsUIControllerNotDestroy::AddToViewport(UGsUIWidgetBase* InWidget)
{
	// 지우지 않는 대신에 보여준다
	if (false == InWidget->IsInViewport())
	{
		InWidget->AddToViewport(InWidget->GetManagedZOrder());
	}
	else
	{
		InWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UGsUIControllerNotDestroy::RemoveFromParent(UGsUIWidgetBase* InWidget)
{
	// CachedWidgetMap에 캐싱해둔 위젯을 제외하고는 파괴시킨다
	if (InWidget->IsCachedWidget())
	{
		// bEnableAutoDestroy 가 false인 경우, RemoveFromParent가 일어나지 않게 처리했으므로 Hide만 진행
		InWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		InWidget->SetEnableAutoDestroy(true);

		if (InWidget->IsInViewport())
		{
			InWidget->RemoveFromParent();
		}
	}
}