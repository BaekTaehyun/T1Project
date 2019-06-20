// Fill out your copyright notice in the Description page of Project Settings.
#include "GsUIController.h"
#include "GsUIWidgetBase.h"
#include "GameService.h"

void UGsUIController::BeginDestroy()
{
	RemoveAll();

	Super::BeginDestroy();
}

UGsUIWidgetBase* UGsUIController::CreateOrFind(UGameInstance* InOwner, TSubclassOf<UGsUIWidgetBase> InClass, const FName& InKey)
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
			else
			{
				// 뷰포트에 다시 넣어주기 위해 지운다
				RemoveFromParent(outWidget);
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
	// 중복생성 안되는 위젯의 경우, 스택 맨 위의 오브젝트와 같으면 더 쌓지 않음
	if (false == InWidget->CanMultipleInstance())
	{
		if (IsTopInStack(InWidget))
		{
			AddToViewport(InWidget);
			return;
		}
	}

	// 화면을 덮는 Window타입 객체면 이전 창들을 모두 Hide
	if (InWidget->IsWindow())
	{
		for (int32 i = StackableArray.Num() - 1; i >= 0; --i)
		{
			UGsUIWidgetBase* widget = StackableArray[i];
			RemoveFromParent(widget);
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
	// 기본값: Window: 10, Popup: 100, Tray: 500, HUD: 1
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

	// 상단에 그려져야 하는 항목(window가 나올때까지)을 모은다
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
		UGsUIWidgetBase* widget = topWidgets[i];

		// 같은 객체가 한 틱에서 RemoveFromParent 후 AddToViewport 시 IsInViewport 가 true일 때가 있다.
		// 발생될 상황은 많지 않을 것이고, 이러한 객체는 중복생성 가능한 타입으로 만들어 해결하자.
		// 발생빈도가 많으면 RND하여 수정할 것.
		AddToViewport(widget);
	}
}

void UGsUIController::RemoveFromParent(UGsUIWidgetBase* InWidget)
{
	if (nullptr != InWidget && InWidget->IsInViewport())
	{
		InWidget->RemoveFromParent();
	}
}

void UGsUIController::RemoveUsingWidget(UGsUIWidgetBase* InWidget)
{
	RemoveFromParent(InWidget);
	UsingWidgetArray.Remove(InWidget);
}

void UGsUIController::ClearStack()
{
	for (auto* widget : StackableArray)
	{
		RemoveUsingWidget(widget);
	}

	StackableArray.Empty();
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

bool UGsUIController::Back()
{
	UGsUIWidgetBase* topWidget = StackPeek();
	if (nullptr != topWidget)
	{
		RemoveWidget(topWidget);
		return true;
	}

	return false;
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

bool UGsUIController::IsTopInStack(UGsUIWidgetBase* InWidget)
{
	UGsUIWidgetBase* topWidget = StackPeek();
	if (InWidget == topWidget)
	{
		return true;
	}

	return false;
}
