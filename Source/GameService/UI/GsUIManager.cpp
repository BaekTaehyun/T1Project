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

void AGsUIManager::PushByKeyName(FName InKey, class UGsUIParameter* InParam)
{
	auto WidgetClass = GetWidgetClass(InKey);
	
	Push(WidgetClass, InParam);
}

void AGsUIManager::Push(TSubclassOf<UGsUIWidgetBase> InClass, UGsUIParameter* InParam)
{
	if (nullptr == InClass)
	{
		GSLOG(Error, TEXT("UIWidget class is null"));
		return;
	}

	FName Key = FName(*InClass.Get()->GetPathName());

	// 개선: 널리턴하면 무조건 에러 뱉어서 검사를 넣었음. 다른 함수 써야하는지 확인필요 
	UGsUIWidgetBase* Widget = nullptr;
	if (CachedWidgets.Contains(Key))
	{
		Widget = *CachedWidgets.Find(Key);
	}

	if (nullptr == Widget)
	{
		// 개선: PC 얻어오는 과정
		APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (nullptr != PC)
		{
			Widget = CreateWidget<UGsUIWidgetBase>(PC, InClass);
			if (nullptr == Widget)
			{
				return;
			}

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
	StackedWidgets.Add(InWidget);
	InWidget->OnPush(InParameters); // 위젯에 이벤트 발생시킴

	// 창을 덮는 객체이면 이전 창들을 Hide
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

	AddToViewport(InWidget);
}

void AGsUIManager::PushUnstack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters)
{
	AddToViewport(InWidget);

	UnstackedWidgets.Add(InWidget);
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
		PopUnstack(InWidget);
	}
}

void AGsUIManager::PopStack(UGsUIWidgetBase* InWidget)
{
	InWidget->RemoveFromParent();

	// 같은 객체가 스택에 존재할 경우엔 가장 위의 것을 지운다
	for (int32 i = StackedWidgets.Num() - 1; i >= 0; --i)
	{
		UGsUIWidgetBase* Widget = StackedWidgets[i];

		if (Widget == InWidget)
		{
			StackedWidgets.RemoveAt(i);
			break;
		}
	}

	// 지워진게 윈도우이면 이전 창들을 열어주는 처리를 한다
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

		// 순서대로 불러야 Depth 문제가 없으므로 역순으로 다시 불러준다
		for (int32 i = TopWidgets.Num() - 1; i >= 0; --i)
		{
			UGsUIWidgetBase* Widget = TopWidgets[i];

			if (false == Widget->IsInViewport())
			{
				AddToViewport(Widget);
			}
		}
	}
	else
	{
		// 최상단 객체가 닫혀있다면 켜준다. (같은 객체가 스택에 있을 경우 꺼져있을 수 있음)
		UGsUIWidgetBase* Widget = StackedWidgets.Last();
		if (nullptr != Widget &&
			false == Widget->IsInViewport())
		{
			AddToViewport(Widget);
		}
	}
}

void AGsUIManager::PopUnstack(UGsUIWidgetBase* InWidget)
{
	InWidget->RemoveFromParent();

	// 같은 객체가 들어올 경우, 먼저 들어온 것부터 지운다
	for (int32 i = 0, maxCount = UnstackedWidgets.Num(); i < maxCount; ++i)
	{
		UGsUIWidgetBase* Widget = UnstackedWidgets[i];

		if (Widget == InWidget)
		{
			Widget = nullptr;
			UnstackedWidgets.RemoveAt(i);
			break;
		}
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
		const FSoftObjectPath& AssetRef = TableRow->WidgetClass.ToStringReference();

		TableRow->WidgetClass =	AssetMgr.SynchronousLoad(AssetRef);
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

//void AGsUIManager::TestForceGC()
//{
//	GetWorld()->ForceGarbageCollection(true);
//}
