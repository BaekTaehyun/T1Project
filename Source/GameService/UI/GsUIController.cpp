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

		// 사용 중이고
		if (UsingWidgetArray.Contains(outWidget))
		{
			// 중복 생성 가능 오브젝트이면
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
	// 상속 클래스(UGsUIControllerNotDestroy)에서 구현
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
	// 화면을 덮는 Window타입 객체면 이전 창들을 모두 Hide
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

	// 위젯에 이벤트 발생시킴
	InWidget->OnPush(InParameters);

	AddToViewport(InWidget);
}

void UGsUIController::AddToViewport(UGsUIWidgetBase* InWidget)
{
	// 일반 AddToViewport 시 ZOrder + 10된 값이 들어간다. UUserWidget::AddToScreen 참고.
	// 같은 ZOrder이면 이미 존재하는 것 다음에 Insert 된다. SOverlay::AddSlot 참고.
	// Window < Popup < Tray 뎁스 보장을 위해 GetManagedZOrder() 를 통해 타입별 기본값을 달리한다.
	// 기본값: Window: 10, Popup: 100, Tray: 500

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
	// 같은 객체가 스택에 존재할 경우엔 가장 위의 것을 지운다
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

	// 지워진게 윈도우이면 이전 창들을 열어주는 처리를 한다
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

		// 순서대로 불러야 Depth 문제가 없으므로 역순으로 다시 불러준다
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
		// 최상단 객체가 닫혀있다면 켜준다. (같은 객체가 스택에 있을 경우 꺼져있을 수 있음)
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