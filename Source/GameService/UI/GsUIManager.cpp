// Fill out your copyright notice in the Description page of Project Settings.
#include "GsUIManager.h"
#include "GameService.h"
#include "GsUIWidgetBase.h"
#include "GsUIParameter.h"
#include "GsUIEventInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "GsUIPathTable.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"


AGsUIManager::AGsUIManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> UIPathTable(TEXT("/Game/UI/UIPathTable.UIPathTable"));
	WidgetClassTable = UIPathTable.Object;
}

void AGsUIManager::BeginDestroy()
{
	RemoveAll();

	Super::BeginDestroy();
}

void AGsUIManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

TWeakObjectPtr<UGsUIWidgetBase> AGsUIManager::PushByKeyName(FName InKey, class UGsUIParameter* InParam)
{
	auto WidgetClass = GetWidgetClass(InKey);

	return PushInter(WidgetClass, InParam);
}

void AGsUIManager::Push(TSubclassOf<UGsUIWidgetBase> InClass, UGsUIParameter* InParam)
{
	// �������Ʈ�� ���� ������ �������� �ʱ� ���� �Լ� �и�
	PushInter(InClass, InParam);
}

UGsUIWidgetBase* AGsUIManager::PushInter(TSubclassOf<UGsUIWidgetBase> InClass, class UGsUIParameter* InParam)
{
	if (nullptr == InClass)
	{
		GSLOG(Error, TEXT("UIWidget class is null"));
		return nullptr;
	}

	UGsUIWidgetBase* Widget = CreateWidgetInter(InClass);
	if (nullptr == Widget)
	{
		return nullptr;
	}

	if (Widget->IsStackUI())
	{
		PushStack(Widget, InParam);
	}
	else
	{
		PushNoStack(Widget, InParam);
	}

	return Widget;
}

void AGsUIManager::PushStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters)
{
	// â�� ���� ��ü�̸� ���� â���� Hide
	if (InWidget->IsWindow())
	{
		for (int32 i = Stack.Num() - 1; i >= 0; --i)
		{
			TWeakObjectPtr<UGsUIWidgetBase> Widget = Stack[i];
			if (Widget.IsValid() &&
				Widget.Get()->IsInViewport())
			{
				Widget->RemoveFromParent();
			}
		}
	}

	Stack.Add(InWidget);
	InWidget->OnPush(InParameters); // ������ �̺�Ʈ �߻���Ŵ

	AddToViewport(InWidget);
}

void AGsUIManager::PushNoStack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters)
{
	AddToViewport(InWidget);

	NoStack.Add(InWidget);
}

void AGsUIManager::Pop(UGsUIWidgetBase* InWidget)
{
	if (nullptr == InWidget)
	{
		return;
	}

	if (InWidget->IsStackUI())
	{
		PopStack(InWidget);
	}
	else
	{
		PopNoStack(InWidget);
	}
}

void AGsUIManager::PopStack(UGsUIWidgetBase* InWidget)
{
	// ���� ��ü�� ���ÿ� ������ ��쿣 ���� ���� ���� �����
	for (int32 i = Stack.Num() - 1; i >= 0; --i)
	{
		TWeakObjectPtr<UGsUIWidgetBase> Widget = Stack[i];
		if (Widget.Get() == InWidget)
		{
			Stack.RemoveAt(i);
			break;
		}
	}

	InWidget->RemoveFromParent();

	UsingWidgets.Remove(InWidget);

	// �������� �������̸� ���� â���� �����ִ� ó���� �Ѵ�
	if (InWidget->IsWindow())
	{
		TArray<TWeakObjectPtr<UGsUIWidgetBase>> TopWidgets;

		for (int32 i = Stack.Num() - 1; i >= 0; --i)
		{
			TWeakObjectPtr<UGsUIWidgetBase> Widget = Stack[i];

			TopWidgets.Add(Widget);

			if (Widget.Get()->IsWindow())
			{
				break;
			}
		}

		// ������� �ҷ��� Depth ������ �����Ƿ� �������� �ٽ� �ҷ��ش�
		for (int32 i = TopWidgets.Num() - 1; i >= 0; --i)
		{
			TWeakObjectPtr<UGsUIWidgetBase> Widget = TopWidgets[i];

			if (false == Widget.Get()->IsInViewport())
			{
				AddToViewport(Widget.Get());
			}
		}
	}
	else
	{
		// �ֻ�� ��ü�� �����ִٸ� ���ش�. (���� ��ü�� ���ÿ� ���� ��� �������� �� ����)
		TWeakObjectPtr<UGsUIWidgetBase> Widget = Stack.Last();
		if (Widget.IsValid() &&
			false == Widget.Get()->IsInViewport())
		{
			AddToViewport(Widget.Get());
		}
	}
}

void AGsUIManager::PopNoStack(UGsUIWidgetBase* InWidget)
{
	InWidget->RemoveFromParent();

	// ���� ��ü�� ���� ���, ���� ���� �ͺ��� �����
	for (int32 i = 0, maxCount = NoStack.Num(); i < maxCount; ++i)
	{
		TWeakObjectPtr<UGsUIWidgetBase> Widget = NoStack[i];
		if (Widget.Get() == InWidget)
		{
			NoStack.RemoveAt(i);
			break;
		}
	}
}

TWeakObjectPtr<UGsUIWidgetBase> AGsUIManager::StackPeek()
{
	if (0 == Stack.Num())
	{
		return nullptr;
	}

	return Stack.Last();
}

void AGsUIManager::RemoveAllStack()
{
	for (int32 i = Stack.Num() - 1; i >= 0; --i)
	{
		TWeakObjectPtr<UGsUIWidgetBase> Widget = Stack[i];
		if (false == Widget.IsValid())
		{
			continue;
		}

		if (Widget->IsInViewport())
		{
			Widget->RemoveFromParent();
		}

		UsingWidgets.Remove(Widget.Get());
	}	

	Stack.Empty();
}

void AGsUIManager::RemoveAllNoStack()
{
	for (auto Widget : NoStack)
	{
		if (Widget.IsValid())
		{
			if (Widget.Get()->IsInViewport())
			{
				Widget.Get()->RemoveFromParent();
			}

			UsingWidgets.Remove(Widget.Get());
		}
	}

	NoStack.Empty();
}

void AGsUIManager::RemoveAll()
{
	for (auto* Widget : UsingWidgets)
	{
		if (nullptr != Widget)
		{
			if (Widget->IsInViewport())
			{
				Widget->RemoveFromParent();
			}

			Widget = nullptr;
		}
	}

	Stack.Empty();
	NoStack.Empty();
	UsingWidgets.Empty();
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

TSubclassOf<UGsUIWidgetBase> AGsUIManager::GetWidgetClass(FName InKey)
{	
	FGsTableUIPath* TableRow = GetTableRow(InKey);
	if (nullptr == TableRow)
	{
		GSLOG(Error, TEXT("Fail to find row in UIPathTable. Key: %s"), *InKey.ToString());
		return nullptr;
	}

	if (TableRow->WidgetClass.IsNull())
	{
		GSLOG(Error, TEXT("WidgetClass is null. Key: %s"), *InKey.ToString());
		return nullptr;
	}

	if (TableRow->WidgetClass.IsPending())
	{
		FStreamableManager& AssetMgr = UAssetManager::GetStreamableManager();
		TableRow->WidgetClass = AssetMgr.LoadSynchronous(TableRow->WidgetClass, true);
	}

	return TableRow->WidgetClass.Get();
}

FGsTableUIPath* AGsUIManager::GetTableRow(FName InKey)
{
	if (nullptr != WidgetClassTable)
	{
		return WidgetClassTable->FindRow<FGsTableUIPath>(InKey, TEXT(""));
	}

	return nullptr;
}

void AGsUIManager::AddToViewport(UGsUIWidgetBase* InWidget)
{
	// �Ϲ� AddToViewport �� ZOrder + 10�� ���� ����. UUserWidget::AddToScreen ����.
	// ���� ZOrder�̸� �̹� �����ϴ� �� ������ Insert �ȴ�. SOverlay::AddSlot ����.
	// Window < Popup < Tray ���� ������ ���� GetManagedZOrder() �� ���� Ÿ�Ժ� �⺻���� �޸��Ѵ�.
	// �⺻��: Window: 10, Popup: 100, Tray: 500
	InWidget->AddToViewport(InWidget->GetManagedZOrder());
}

UGsUIWidgetBase* AGsUIManager::CreateWidgetInter(TSubclassOf<UGsUIWidgetBase> InClass)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (nullptr == PC)
	{
		return nullptr;
	}

	FName Key = FName(*InClass.Get()->GetPathName());

	// ĳ�� �ص� ���� �˻�
	UGsUIWidgetBase* Widget = nullptr;
	if (CachedWidgets.Contains(Key))
	{
		Widget = *CachedWidgets.Find(Key);

		// ���� ������ ���λ���
		if (UsingWidgets.Contains(Widget))
		{
			Widget = CreateWidget<UGsUIWidgetBase>(PC, InClass);
			if (nullptr == Widget)
			{
				return nullptr;
			}
		}
	}
	else
	{
		Widget = CreateWidget<UGsUIWidgetBase>(PC, InClass);
		if (nullptr == Widget)
		{
			return nullptr;
		}

		CachedWidgets.Add(Key, Widget);
	}

	UsingWidgets.Add(Widget);

	return Widget;
}

void AGsUIManager::Back()
{
	TWeakObjectPtr<UGsUIWidgetBase> TargetWidget = StackPeek();
	if (TargetWidget.IsValid())
	{
		Pop(TargetWidget.Get());
	}
}
