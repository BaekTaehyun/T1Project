// Fill out your copyright notice in the Description page of Project Settings.
#include "GsUIManager.h"
#include "GameService.h"
#include "GsUIWidgetBase.h"
#include "GsUIParameter.h"
#include "GsUIEventInterface.h"
//#include "UI/GsUIPopup.h"
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
	// FIX: 로직 개선 필요
	// 동일 객체가 최상단일 경우, 다시 추가하지 않는다.
	UGsUIWidgetBase* TopWidget = StackPeek();
	if (TopWidget == InWidget)
	{
		if (false == InWidget->IsInViewport())
		{
			InWidget->AddToViewport();
		}

		return;
	}

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
	
	/*
	// TEST: ZOrder문제 수정 위한 테스트
	if (InWidget->IsA<UGsUIPopup>())
	{
		InWidget->AddToViewport(DefaultPopupZOrder);
	}
	else
	{
		InWidget->AddToViewport();
	}
	*/
	InWidget->AddToViewport();
}

void AGsUIManager::PushUnstack(UGsUIWidgetBase* InWidget, UGsUIParameter* InParameters)
{
	//InWidget->AddToViewport(DefaultTrayZOrder);
	InWidget->AddToViewport();

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

	// 스택에 같은 객체가 반복될 경우 모두 지워지는 것을 막기 위해 가장 위의 것만 지우도록 처리
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

			// FIX: 방금 지운 Remove한 객체와 같으면 제대로 동작하지 않음. 수정필요
			if (false == Widget->IsInViewport())
			{
				Widget->AddToViewport();
			}
		}
	}
}

void AGsUIManager::PopUnstack(UGsUIWidgetBase* InWidget)
{
	InWidget->RemoveFromParent();

	// 같은 객체가 반복될 경우 모두 지워지는 것을 막기 위해 가장 위의 것만 지우도록 처리
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
