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

	// 널리턴하면 무조건 에러 뱉는듯.
	UGsUIWidgetBase* Widget = nullptr;
	if (CachedWidgets.Contains(Key))
	{
		Widget = *CachedWidgets.Find(Key);
	}

	if (nullptr == Widget)
	{
		// FIX: PC 얻어오는 과정
		APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (nullptr != PC)
		{
			Widget = CreateWidget<UGsUIWidgetBase>(PC, InClass);
			if (nullptr == Widget)
			{
				//GSLOG(Error, TEXT("Fail to create UIWidget. PathName: %s"), Key.ToString());
				return;
			}

			// 캐싱
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
	// 동일 객체일 경우
	UGsUIWidgetBase* TopWidget = StackPeek();
	if (TopWidget == InWidget)
	{
		// 꺼져있으면 켜줌
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

	InWidget->AddToViewport();


	// 동일창이면 동작 무시

	// 팝업이면 그냥 Addtoviewport

	// 윈도우면 이전창 전부 RemoveParent 후 넣고

	/*
	UGsUIWidgetBase* TopWidget = StackPeek();
	if (TopWidget != InWidget)
	{
		StackedWidgets.Add(InWidget);

		// UI이벤트 콜
		InWidget->OnPush(InParameters);
	}

	InWidget->AddToViewport();

	// 이전 창, 팝업들을 Hide 시키기 위함 
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

	// 팝업닫히면 여기여기여기

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

			// 방금 지운 Remove한 객체와 같으면 제대로 동작하지 않음. 
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

void AGsUIManager::UpdateStack()
{
	// 스택에 같은 객체가 반복될 경우 중복처리를 막기 위함
	for (auto* Iter : StackedWidgets)
	{
		Iter->bStackProcessed = false;
	}

	// 이전이전창까지 열기
	// 이전 창까지 가리기?
	//TArray<UGsUIWidgetBase*> 


	//GSLOG(Warning, TEXT("----------------UpdateStack()"));
	bool bUnderTopWindow = false;
	//for (int32 i = 0; i < StackedWidgets.Num(); ++i)
	for (int32 i = StackedWidgets.Num() -1; i >= 0; --i)
	{
		UGsUIWidgetBase* Widget = StackedWidgets[i];

		//GSLOG(Warning, TEXT("i = %d"), i);

		// TopWindow까지는 보여주며, 나머지는 감춤, 중첩된 경우는 
		if (bUnderTopWindow &&
			false == Widget->bStackProcessed)
		{
			Widget->RemoveFromParent();
		}
		else
		{
			// 꺼져있으면 켜줌
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
