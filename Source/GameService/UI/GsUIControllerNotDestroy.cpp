// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIControllerNotDestroy.h"


UGsUIWidgetBase* UGsUIControllerNotDestroy::CreateOrFind(UGameInstance* InOwner, TSubclassOf<UGsUIWidgetBase> InClass)
{
	FName Key = FName(*InClass.Get()->GetName());

	UGsUIWidgetBase* outWidget = nullptr;
	if (CachedWidgetMap.Contains(Key))
	{
		outWidget = *CachedWidgetMap.Find(Key);

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
			CachedWidgetMap.Add(Key, outWidget);
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
	}
	else
	{
		InWidget->AddToViewport(InWidget->GetManagedZOrder());
	}
}

void UGsUIControllerNotDestroy::RemoveUsingWidget(UGsUIWidgetBase* InWidget)
{
	// CachedWidgetMap�� ĳ���ص� ������ �����ϰ��� �ı���Ų��
	if (InWidget->IsCachedWidget())
	{
		// ĳ�õ� �׸��� ������ �ʴ´�. 
		// RemoveFromParent�� ȣ�� �ȵǰ� ó���Ǿ����Ƿ� Hide �� �����Ѵ�
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

	UsingWidgetArray.Remove(InWidget);
}