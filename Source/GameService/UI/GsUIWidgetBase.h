// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GsUIParameter.h"
#include "GsUIEventInterface.h"
#include "GsUIWidgetBase.generated.h"


/**
 * GsUIManager에서 관리받는 위젯의 기본클래스. GsUIWindow, GsUIPopup, GsUITray 를 사용할 것.
 * 개선: abstract 클래스로 만들면 좋겠다
 */
UCLASS()
class GAMESERVICE_API UGsUIWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UGsUIWidgetBase(const FObjectInitializer& ObjectInitializer);

	/** 스택시키는 UI인가 */
	virtual bool IsStackUI() const { return false; }
	
	/** 윈도우타입(화면을 다 가리는)인가 */
	virtual bool IsWindow() const { return false; }

	UFUNCTION(BlueprintCallable, Category = "GsManaged")
	virtual void Close();

	UFUNCTION(BlueprintCallable, Category = "GsManaged")
	class AGsUIManager* GetUIManager();

private:
	/** 스택 중복 처리를 막는다 */
	bool bStackProcessed;
	
protected:
	/** 스택에 푸쉬할 때 부르며, 초기화 데이터를 넘길 수 있음. 위의 창이 닫혀서 다시 위에 올라올 때는 불리지 않음 
	 * 주의: OnInitialized 뒤에 불리지만 Construct 보다 전에 불리므로 Slate 세팅과 관련된 처리를 여기서 하면 안됨.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "GsManaged", meta = (DisplayName = "OnPush"))
	void OnPush(UGsUIParameter* InParam = nullptr);
	virtual void OnPush_Implementation(UGsUIParameter* InParam = nullptr);

	// 방법 1. 이 메시지를 보낸다. 간단할 경우에 사용.
	UFUNCTION(BlueprintNativeEvent, Category = "GsManaged", meta = (DisplayName = "OnMessage"))
	void OnMessage(FName InKey, UGsUIParameter* InParam = nullptr);
	virtual void OnMessage_Implementation(FName InKey, UGsUIParameter* InParam = nullptr);
	// 방법 2. UMG에서 IGsUIEventInterface를 추가 후 해당 메시지를 받는다. 간단할 경우에 사용.
	// 방법 3. UGsUIWindow, Popup, Tray 중 하나를 상속받아 별도 구현하여 사용. 복잡한 경우에 사용.


	/*
	// 이하는 UserWidget에서 제공되는 함수로, 용도에 맞게 상속받아 사용할 것.

	// CreateWidget 시 한 번 불린다. 한 번만 수행해야 할 항목 처리 시 사용. (BP이벤트: OnInitialized)
	virtual void NativeOnInitialized() override;
	
	// AddToViewport 직후에 불림. (BP이벤트: PreConstruct)
	virtual void NativePreConstruct() override;
	
	// Slate 세팅이 완료되면 불린다. 일반적인 초기세팅에 사용. (BP이벤트: Construct) 
	// 주의: 유니티의 OnEnable 같지만 SetVisible에 영향받지 않음에 유의
	virtual void NativeConstruct() override;

	// RemoveFromParent된 후 불린다. (BP이벤트: Destruct) 
	// 주의: 유니티의 OnDisable 같지만 SetVisible에 영향받지 않음에 유의
	virtual void NativeDestruct() override;

	// RefCount가 0이되어 실제 파괴될 때 불린다. 별도 BP이벤트는 제공하지 않는다.
	virtual void BeginDestroy() override;
	*/

	
	

	friend class AGsUIManager;
};
