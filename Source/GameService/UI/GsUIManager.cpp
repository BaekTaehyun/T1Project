// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIManager.h"
#include "GameService.h"
#include "GsUIWidgetBase.h"
#include "GsUIParameter.h"
#include "GsUIEventInterface.h"


AGsUIManager::AGsUIManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 
}

void AGsUIManager::Push(TSubclassOf<UGsUIWidgetBase> InClass, UGsUIParameter* InParam)
{
	if (nullptr == InClass)
	{
		GSLOG(Error, TEXT("UIWidget class is null"));
		return;
	}
	
	FName Key = FName(*InClass.Get()->GetPathName());

	// �θ����ϸ� ������ ���� ��µ�.
	UGsUIWidgetBase* Widget = nullptr;
	if (CachedWidgets.Contains(Key))
	{
		Widget = *CachedWidgets.Find(Key);
	}

	if (nullptr == Widget)
	{
		// FIX: PC ������ ����
		APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (nullptr != PC)
		{
			Widget = CreateWidget<UGsUIWidgetBase>(PC, InClass);
			if (nullptr == Widget)
			{
				//GSLOG(Error, TEXT("Fail to create UIWidget. PathName: %s"), Key.ToString());
				return;
			}

			// ĳ��
			CachedWidgets.Add(Key, Widget);
		}
	}

	if (Widget->IsStackUI())
	{
		PushStack(Widget, InParam);
	}
	else
	{
		PushUnstack(Widget, InParam);
	}
}

void AGsUIManager::PushStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters)
{
	// ���� ��ü�� ���
	UGsUIWidgetBase* TopWidget = StackPeek();
	if (TopWidget == InWidget)
	{
		// ���������� ����
		if (false == InWidget->IsInViewport())
		{
			InWidget->AddToViewport();
		}

		return;
	}

	StackedWidgets.Add(InWidget);
	InWidget->OnPush(InParameters); // ������ �̺�Ʈ �߻���Ŵ

	// â�� ���� ��ü�̸� ���� â���� Hide
	if (InWidget->IsWindow())
	{
		for (int32 i = StackedWidgets.Num() - 1; i >= 0; --i)
		{
			UGsUIWidgetBase* Widget = StackedWidgets[i];
			if (nullptr == Widget)
			{
				continue;
			}

			if (Widget->IsInViewport())
			{
				Widget->RemoveFromParent();
			}
		}
	}	

	InWidget->AddToViewport();


	// ����â�̸� ���� ����

	// �˾��̸� �׳� Addtoviewport

	// ������� ����â ���� RemoveParent �� �ְ�

	/*
	UGsUIWidgetBase* TopWidget = StackPeek();
	if (TopWidget != InWidget)
	{
		StackedWidgets.Add(InWidget);

		// UI�̺�Ʈ ��
		InWidget->OnPush(InParameters);
	}

	InWidget->AddToViewport();

	// ���� â, �˾����� Hide ��Ű�� ���� 
	if (InWidget->IsWindow())
	{
		UpdateStack();
	}
	*/
}

void AGsUIManager::PushUnstack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters)
{
	InWidget->AddToViewport();

	UnstackedWidgets.Add(InWidget);
}

void AGsUIManager::Pop(UGsUIWidgetBase* InWidget)
{
	if (nullptr == InWidget)
	{
		return;
	}

	//InWidget->RemoveFromParent();

	if (InWidget->IsStackUI())
	{
		PopStack(InWidget);
	}
	else
	{
		PopUnstack(InWidget);
	}
}

void AGsUIManager::PopStack(UGsUIWidgetBase* InWidget)
{
	InWidget->RemoveFromParent();

	// ���ÿ� ���� ��ü�� �ݺ��� ��� ��� �������� ���� ���� ���� ���� ���� �͸� ���쵵�� ó��
	for (int32 i = StackedWidgets.Num() - 1; i >= 0; --i)
	{
		UGsUIWidgetBase* Widget = StackedWidgets[i];

		if (Widget == InWidget)
		{
			StackedWidgets.RemoveAt(i);
			break;
		}
	}

	// �˾������� ���⿩�⿩��

	// �������� �������̸� ���� â���� �����ִ� ó���� �Ѵ�
	if (InWidget->IsWindow())
	{
		TArray<UGsUIWidgetBase*> TopWidgets;

		for (int32 i = StackedWidgets.Num() - 1; i >= 0; --i)
		{
			UGsUIWidgetBase* Widget = StackedWidgets[i];

			TopWidgets.Add(Widget);

			if (Widget->IsWindow())
			{
				break;
			}
		}

		// ������� �ҷ��� Depth ������ �����Ƿ� �������� �ٽ� �ҷ��ش�

		for (int32 i = TopWidgets.Num() - 1; i >= 0; --i)
		{
			UGsUIWidgetBase* Widget = TopWidgets[i];

			// ��� ���� Remove�� ��ü�� ������ ����� �������� ����. 
			if (false == Widget->IsInViewport())
			{
				Widget->AddToViewport();
			}
		}
	}
	

	//UpdateStack();
}

void AGsUIManager::PopUnstack(UGsUIWidgetBase* InWidget)
{
	InWidget->RemoveFromParent();

	// ���� ��ü�� �ݺ��� ��� ��� �������� ���� ���� ���� ���� ���� �͸� ���쵵�� ó��
	for (int32 i = UnstackedWidgets.Num() - 1; i >= 0; --i)
	{
		UGsUIWidgetBase* Widget = UnstackedWidgets[i];

		if (Widget == InWidget)
		{
			UnstackedWidgets.RemoveAt(i);
			break;
		}
	}
}

void AGsUIManager::UpdateStack()
{
	// ���ÿ� ���� ��ü�� �ݺ��� ��� �ߺ�ó���� ���� ����
	for (auto* Iter : StackedWidgets)
	{
		Iter->bStackProcessed = false;
	}

	// ��������â���� ����
	// ���� â���� ������?
	//TArray<UGsUIWidgetBase*> 


	//GSLOG(Warning, TEXT("----------------UpdateStack()"));
	bool bUnderTopWindow = false;
	//for (int32 i = 0; i < StackedWidgets.Num(); ++i)
	for (int32 i = StackedWidgets.Num() -1; i >= 0; --i)
	{
		UGsUIWidgetBase* Widget = StackedWidgets[i];

		//GSLOG(Warning, TEXT("i = %d"), i);

		// TopWindow������ �����ָ�, �������� ����, ��ø�� ���� 
		if (bUnderTopWindow &&
			false == Widget->bStackProcessed)
		{
			Widget->RemoveFromParent();
		}
		else
		{
			// ���������� ����
			if (false == Widget->IsInViewport())
			{	
				Widget->AddToViewport();
			}

			if (Widget->IsWindow())
			{
				bUnderTopWindow = true;
			}
		}

		Widget->bStackProcessed = true;
	}
}

UGsUIWidgetBase* AGsUIManager::StackPeek()
{
	if (0 == StackedWidgets.Num())
	{
		return nullptr;
	}

	return StackedWidgets.Last();
}

UGsUIWidgetBase* AGsUIManager::GetCachedWidget(FString InPathName)
{
	return *CachedWidgets.Find(FName(*InPathName));
}

void AGsUIManager::RemoveAllStack()
{
	for (int32 i = StackedWidgets.Num() - 1; i >= 0; --i)
	{
		UGsUIWidgetBase* Widget = StackedWidgets[i];
		if (nullptr == Widget)
		{
			continue;
		}

		if (Widget->IsInViewport())
		{
			Widget->RemoveFromParent();
		}
	}

	StackedWidgets.Empty();
}

void AGsUIManager::RemoveAllTray()
{
	for (auto* Widget : UnstackedWidgets)
	{
		if (nullptr == Widget)
		{
			continue;
		}

		if (Widget->IsInViewport())
		{
			Widget->RemoveFromParent();
		}
	}

	UnstackedWidgets.Empty();
}

void AGsUIManager::RemoveAll()
{
	RemoveAllStack();
	RemoveAllTray();

	CachedWidgets.Empty();
}

AGsUIManager* AGsUIManager::GetUIManager(class APlayerController* InOwner)
{
	if (nullptr != InOwner)
	{
		return Cast<AGsUIManager>(InOwner->GetHUD());
	}

	return nullptr;
}
