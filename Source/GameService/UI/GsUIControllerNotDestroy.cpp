// Fill out your copyright notice in the Description page of Project Settings.


#include "GsUIControllerNotDestroy.h"
//#include "GsUINonDestructiveWidget.h"


UGsUIWidgetBase* UGsUIControllerNotDestroy::CreateOrFind(UGameInstance* InOwner, TSubclassOf<UGsUIWidgetBase> InClass)
{
	//Super::CreateOrFind(); // 재정의한 내용이므로 부르지 않는다

	//FName Key = FName(*InClass.Get()->GetPathName());
	FName Key = FName(*InClass.Get()->GetName());

	UGsUIWidgetBase* outWidget = nullptr;
	if (CachedWidgetMap.Contains(Key))
	{
		outWidget = *CachedWidgetMap.Find(Key);

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
			CachedWidgetMap.Add(Key, outWidget);
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
	}
	else
	{
		InWidget->AddToViewport(InWidget->GetManagedZOrder());
	}
}

void UGsUIControllerNotDestroy::RemoveUsingWidget(UGsUIWidgetBase* InWidget)
{
	// 파괴되지 않을 항목인데 인스턴싱이 되게 했을경우, 계속 삭제가 안될 것이므로 위험하다.
	// 따라서 맵에 캐싱해둔 오브젝트를 제외하고는 파괴시킨다.

	if (InWidget->IsCachedWidget())
	{
		// FIX: 캐시된 항목은 지우지 않는다. RemoveFromParent 불러도 소용 없으므로 가려주기만 함
		InWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		// 캐시 이외에 인스턴싱 된 항목은 파괴 시킨다
		InWidget->SetEnableAutoDestroy(true);

		if (InWidget->IsInViewport())
		{
			InWidget->RemoveFromParent();
		}
	}

	UsingWidgetArray.Remove(InWidget);
}