// Fill out your copyright notice in the Description page of Project Settings.
#include "GsUIControllerNotDestroy.h"


void UGsUIControllerNotDestroy::RemoveAll()
{
	for (auto* widget : UsingWidgetArray)
	{
		if (nullptr != widget)
		{
			// �ı���ų �� �ְ� ��ȯ
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
	// ������ �ʴ� ��ſ� �����ش�
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
	// CachedWidgetMap�� ĳ���ص� ������ �����ϰ�� �ı���Ų��
	if (InWidget->IsCachedWidget())
	{
		// bEnableAutoDestroy �� false�� ���, RemoveFromParent�� �Ͼ�� �ʰ� ó�������Ƿ� Hide�� ����
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