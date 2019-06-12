// Fill out your copyright notice in the Description page of Project Settings.
#include "GsUIControllerNotDestroy.h"


UGsUIWidgetBase* UGsUIControllerNotDestroy::CreateOrFind(UGameInstance* InOwner, TSubclassOf<UGsUIWidgetBase> InClass, const FName& InKey)
{
	UGsUIWidgetBase** widget = CachedWidgetMap.Find(InKey);
	UGsUIWidgetBase* outWidget = nullptr;
	if (nullptr != widget)
	{
		outWidget = *widget;

		// ��� ���̰�
		if (UsingWidgetArray.Contains(outWidget))
		{
			// �ߺ� ���� ���� ������Ʈ�̸�
			if (outWidget->CanMultipleInstance())
			{
				outWidget = CreateWidget<UGsUIWidgetBase>(InOwner, InClass);
				UsingWidgetArray.Add(outWidget);
			}
		}
		else
		{
			UsingWidgetArray.Add(outWidget);
		}
	}
	else
	{
		outWidget = CreateWidget<UGsUIWidgetBase>(InOwner, InClass);
		if (nullptr != outWidget)
		{
			outWidget->bIsCachedWidget = true;
			CachedWidgetMap.Add(InKey, outWidget);
			UsingWidgetArray.Add(outWidget);
		}
	}

	return outWidget;
}

void UGsUIControllerNotDestroy::RemoveAll()
{
	//Super::RemoveAll(); // �������� �����̹Ƿ� �θ��� �ʴ´�

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
	if (InWidget->IsInViewport())
	{		
		InWidget->SetVisibility(ESlateVisibility::Visible);
		//InWidget->SetIsEnabled(true);
	}
	else
	{
		InWidget->AddToViewport(InWidget->GetManagedZOrder());
	}
}

void UGsUIControllerNotDestroy::RemoveUsingWidget(UGsUIWidgetBase* InWidget)
{
	// CachedWidgetMap�� ĳ���ص� ������ �����ϰ�� �ı���Ų��
	if (InWidget->IsCachedWidget())
	{
		// ĳ�õ� �׸��� ������ �ʴ´�. 
		// RemoveFromParent�� ȣ�� �ȵǰ� ó���Ǿ����Ƿ� Hide �� �����Ѵ�
		InWidget->SetVisibility(ESlateVisibility::Hidden);
		//InWidget->SetIsEnabled(false);
	}
	else
	{
		InWidget->SetEnableAutoDestroy(true);

		if (InWidget->IsInViewport())
		{
			InWidget->RemoveFromParent();
		}
	}

	UsingWidgetArray.Remove(InWidget);
}