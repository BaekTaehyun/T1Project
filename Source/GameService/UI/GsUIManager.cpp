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
	// 블루프린트에 리턴 포인터 제공하지 않기 위해 함수 분리
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
	// 창을 덮는 객체이면 이전 창들을 Hide
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
	InWidget->OnPush(InParameters); // 위젯에 이벤트 발생시킴

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
	// 같은 객체가 스택에 존재할 경우엔 가장 위의 것을 지운다
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

	// 지워진게 윈도우이면 이전 창들을 열어주는 처리를 한다
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

		// 순서대로 불러야 Depth 문제가 없으므로 역순으로 다시 불러준다
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
		// 최상단 객체가 닫혀있다면 켜준다. (같은 객체가 스택에 있을 경우 꺼져있을 수 있음)
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

	// 같은 객체가 들어올 경우, 먼저 들어온 것부터 지운다
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
	// 일반 AddToViewport 시 ZOrder + 10된 값이 들어간다. UUserWidget::AddToScreen 참고.
	// 같은 ZOrder이면 이미 존재하는 것 다음에 Insert 된다. SOverlay::AddSlot 참고.
	// Window < Popup < Tray 뎁스 보장을 위해 GetManagedZOrder() 를 통해 타입별 기본값을 달리한다.
	// 기본값: Window: 10, Popup: 100, Tray: 500
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

	// 캐싱 해둔 위젯 검사
	UGsUIWidgetBase* Widget = nullptr;
	if (CachedWidgets.Contains(Key))
	{
		Widget = *CachedWidgets.Find(Key);

		// 쓰고 있으면 새로생성
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
