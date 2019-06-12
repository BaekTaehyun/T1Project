// Fill out your copyright notice in the Description page of Project Settings.
#include "GsUIController.h"
#include "GsUIWidgetBase.h"
#include "GameService.h"

void UGsUIController::BeginDestroy()
{
	RemoveAll();

	Super::BeginDestroy();
}

UGsUIWidgetBase* UGsUIController::CreateOrFind(UWorld* InOwner, TSubclassOf<UGsUIWidgetBase> InClass, const FName& InKey)
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

UGsUIWidgetBase* UGsUIController::CreateOrFind(UGameInstance* InOwner, TSubclassOf<UGsUIWidgetBase> InClass, const FName& InKey)
{
	// ��� Ŭ����(UGsUIControllerNotDestroy)���� ����
	return nullptr;
}


void UGsUIController::AddWidget(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters)
{	
	if (InWidget->IsStackUI())
	{
		PushStack(InWidget, InParameters);
	}
	else
	{
		AddToViewport(InWidget);
	}
}

void UGsUIController::PushStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters)
{
	// ȭ���� ���� WindowŸ�� ��ü�� ���� â���� ��� Hide
	if (InWidget->IsWindow())
	{
		for (int32 i = StackableArray.Num() - 1; i >= 0; --i)
		{
			UGsUIWidgetBase* widget = StackableArray[i];
			if (nullptr != widget && 
				widget->IsInViewport())
			{
				widget->RemoveFromParent();
			}
		}
	}

	StackableArray.Add(InWidget);

	// ������ �̺�Ʈ �߻���Ŵ
	InWidget->OnPush(InParameters);

	AddToViewport(InWidget);
}

void UGsUIController::AddToViewport(UGsUIWidgetBase* InWidget)
{
	// �Ϲ� AddToViewport �� ZOrder + 10�� ���� ����. UUserWidget::AddToScreen ����.
	// ���� ZOrder�̸� �̹� �����ϴ� �� ������ Insert �ȴ�. SOverlay::AddSlot ����.
	// Window < Popup < Tray ���� ������ ���� GetManagedZOrder() �� ���� Ÿ�Ժ� �⺻���� �޸��Ѵ�.
	// �⺻��: Window: 10, Popup: 100, Tray: 500

	if (false == InWidget->IsInViewport())
	{
		InWidget->AddToViewport(InWidget->GetManagedZOrder());
	}
}

void UGsUIController::RemoveWidget(UGsUIWidgetBase* InWidget)
{
	if (InWidget->IsStackUI())
	{
		PopStack(InWidget);
	}
	else
	{
		RemoveUsingWidget(InWidget);
	}
}

void UGsUIController::RemoveWidget(const FName& InKey)
{
	UGsUIWidgetBase** widget = CachedWidgetMap.Find(InKey);
	if (nullptr != widget)
	{
		RemoveWidget(*widget);
	}
}

void UGsUIController::PopStack(UGsUIWidgetBase* InWidget)
{
	// ���� ��ü�� ���ÿ� ������ ��쿣 ���� ���� ���� �����
	for (int32 i = StackableArray.Num() - 1; i >= 0; --i)
	{
		UGsUIWidgetBase* widget = StackableArray[i];
		if (widget == InWidget)
		{
			StackableArray.RemoveAt(i);
			break;
		}
	}

	RemoveUsingWidget(InWidget);

	// �������� �������̸� ���� â���� �����ִ� ó���� �Ѵ�
	if (InWidget->IsWindow())
	{
		TArray<UGsUIWidgetBase*> topWidgets;

		for (int32 i = StackableArray.Num() - 1; i >= 0; --i)
		{
			UGsUIWidgetBase* widget = StackableArray[i];

			topWidgets.Add(widget);

			if (widget->IsWindow())
			{
				break;
			}
		}

		// ������� �ҷ��� Depth ������ �����Ƿ� �������� �ٽ� �ҷ��ش�
		for (int32 i = topWidgets.Num() - 1; i >= 0; --i)
		{
			TWeakObjectPtr<UGsUIWidgetBase> widget = topWidgets[i];

			if (false == widget.Get()->IsInViewport())
			{
				AddToViewport(widget.Get());
			}
		}
	}
	else
	{
		// �ֻ�� ��ü�� �����ִٸ� ���ش�. (���� ��ü�� ���ÿ� ���� ��� �������� �� ����)
		UGsUIWidgetBase* widget = StackPeek();
		if (nullptr != widget &&
			false == widget->IsInViewport())
		{
			AddToViewport(widget);
		}
	}
}

void UGsUIController::RemoveUsingWidget(UGsUIWidgetBase* InWidget)
{
	InWidget->RemoveFromParent();

	UsingWidgetArray.Remove(InWidget);
}

void UGsUIController::RemoveAll()
{
	for (auto* widget : UsingWidgetArray)
	{
		if (nullptr != widget)
		{
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

void UGsUIController::Back()
{
	UGsUIWidgetBase* topWidget = StackPeek();
	if (nullptr != topWidget)
	{
		RemoveWidget(topWidget);
	}
}

UGsUIWidgetBase* UGsUIController::StackPeek()
{
	if (0 == StackableArray.Num())
	{
		return nullptr;
	}

	return StackableArray.Last();
}

UGsUIWidgetBase* UGsUIController::GetCachedWidgetByName(FName InKey, bool InActiveCheck)
{
	UGsUIWidgetBase** widget = CachedWidgetMap.Find(InKey);
	if (nullptr == widget)
	{
		return nullptr;
	}

	if (InActiveCheck)
	{		
		if (false == (*widget)->GetIsVisible())
		{
			return false;
		}
	}

	return *widget;
}