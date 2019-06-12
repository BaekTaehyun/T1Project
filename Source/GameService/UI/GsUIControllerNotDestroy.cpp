// Fill out your copyright notice in the Description page of Project Settings.
#include "GsUIControllerNotDestroy.h"


UGsUIWidgetBase* UGsUIControllerNotDestroy::CreateOrFind(UGameInstance* InOwner, TSubclassOf<UGsUIWidgetBase> InClass, const FName& InKey)
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

void UGsUIControllerNotDestroy::RemoveAll()
{
	//Super::RemoveAll(); // 재정의한 내용이므로 부르지 않는다

	for (auto* widget : UsingWidgetArray)
	{
		if (nullptr != widget)
		{
			// 파괴시킬 수 있게 전환
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
	// 지우지 않는 대신에 보여준다
	if (InWidget->IsInViewport())
	{		
		InWidget->SetVisibility(ESlateVisibility::Visible);
		//InWidget->SetIsEnabled(true);
	}
	else
	{
		InWidget->AddToViewport(InWidget->GetManagedZOrder());
	}
}

void UGsUIControllerNotDestroy::RemoveUsingWidget(UGsUIWidgetBase* InWidget)
{
	// CachedWidgetMap에 캐싱해둔 위젯을 제외하고는 파괴시킨다
	if (InWidget->IsCachedWidget())
	{
		// 캐시된 항목은 지우지 않는다. 
		// RemoveFromParent가 호출 안되게 처리되었으므로 Hide 만 진행한다
		InWidget->SetVisibility(ESlateVisibility::Hidden);
		//InWidget->SetIsEnabled(false);
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