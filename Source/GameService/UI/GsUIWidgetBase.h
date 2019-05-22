// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GsUIParameter.h"
#include "GsUIEventInterface.h"
#include "GsUIWidgetBase.generated.h"


/**
 * GsUIManager���� �����޴� ������ �⺻Ŭ����. GsUIWindow, GsUIPopup, GsUITray �� ����� ��.
 * ����: abstract Ŭ������ ����� ���ڴ�
 */
UCLASS()
class GAMESERVICE_API UGsUIWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UGsUIWidgetBase(const FObjectInitializer& ObjectInitializer);

	/** ���ý�Ű�� UI�ΰ� */
	virtual bool IsStackUI() const { return false; }
	
	/** ������Ÿ��(ȭ���� �� ������)�ΰ� */
	virtual bool IsWindow() const { return false; }

	UFUNCTION(BlueprintCallable, Category = "GsManaged")
	virtual void Close();

	UFUNCTION(BlueprintCallable, Category = "GsManaged")
	class AGsUIManager* GetUIManager();

private:
	/** ���� �ߺ� ó���� ���´� */
	bool bStackProcessed;
	
protected:
	/** ���ÿ� Ǫ���� �� �θ���, �ʱ�ȭ �����͸� �ѱ� �� ����. ���� â�� ������ �ٽ� ���� �ö�� ���� �Ҹ��� ���� 
	 * ����: OnInitialized �ڿ� �Ҹ����� Construct ���� ���� �Ҹ��Ƿ� Slate ���ð� ���õ� ó���� ���⼭ �ϸ� �ȵ�.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "GsManaged", meta = (DisplayName = "OnPush"))
	void OnPush(UGsUIParameter* InParam = nullptr);
	virtual void OnPush_Implementation(UGsUIParameter* InParam = nullptr);

	// ��� 1. �� �޽����� ������. ������ ��쿡 ���.
	UFUNCTION(BlueprintNativeEvent, Category = "GsManaged", meta = (DisplayName = "OnMessage"))
	void OnMessage(FName InKey, UGsUIParameter* InParam = nullptr);
	virtual void OnMessage_Implementation(FName InKey, UGsUIParameter* InParam = nullptr);
	// ��� 2. UMG���� IGsUIEventInterface�� �߰� �� �ش� �޽����� �޴´�. ������ ��쿡 ���.
	// ��� 3. UGsUIWindow, Popup, Tray �� �ϳ��� ��ӹ޾� ���� �����Ͽ� ���. ������ ��쿡 ���.


	/*
	// ���ϴ� UserWidget���� �����Ǵ� �Լ���, �뵵�� �°� ��ӹ޾� ����� ��.

	// CreateWidget �� �� �� �Ҹ���. �� ���� �����ؾ� �� �׸� ó�� �� ���. (BP�̺�Ʈ: OnInitialized)
	virtual void NativeOnInitialized() override;
	
	// AddToViewport ���Ŀ� �Ҹ�. (BP�̺�Ʈ: PreConstruct)
	virtual void NativePreConstruct() override;
	
	// Slate ������ �Ϸ�Ǹ� �Ҹ���. �Ϲ����� �ʱ⼼�ÿ� ���. (BP�̺�Ʈ: Construct) 
	// ����: ����Ƽ�� OnEnable ������ SetVisible�� ������� ������ ����
	virtual void NativeConstruct() override;

	// RemoveFromParent�� �� �Ҹ���. (BP�̺�Ʈ: Destruct) 
	// ����: ����Ƽ�� OnDisable ������ SetVisible�� ������� ������ ����
	virtual void NativeDestruct() override;

	// RefCount�� 0�̵Ǿ� ���� �ı��� �� �Ҹ���. ���� BP�̺�Ʈ�� �������� �ʴ´�.
	virtual void BeginDestroy() override;
	*/

	
	

	friend class AGsUIManager;
};
