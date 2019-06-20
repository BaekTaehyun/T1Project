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
	
private:
	friend class UGsUIController;
	friend class UGsUIControllerNotDestroy;
	
	ESlateVisibility BackupVisibility;
	bool bIsCachedWidget = false;
	bool bEnableAutoDestroy = false;

protected:
	// Window, Popup, Tray �������� ������ ���� �ʰ� ������ �����ϰ� ���� ��� true�� ����
	UPROPERTY(EditDefaultsOnly, Category = "GsManaged")
	bool bNotUseManagedZOrder = false;

	// ����: ����ȭ�� ���� ������ ���� �ʵ��� �ϴ� ���� ������.
	// ���� ������ ���� ������ ��. 
	UPROPERTY(EditDefaultsOnly, Category = "GsManaged")
	int32 AddZOrder = 0;

	// ���� ��ȯ �� �ı��� ������Ʈ�ΰ�
	UPROPERTY(EditDefaultsOnly, Category = "GsManaged")
	bool bNotDestroy = false;

public:
	UGsUIWidgetBase(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void RemoveFromParent() override;

	UFUNCTION()
	virtual void ToggleHideUnhide(bool InHide);

protected:
	//���ÿ� Ǫ���� �� �θ���, �ʱ�ȭ �����͸� �ѱ� �� ����.
	//����: OnInitialized �ڿ� �Ҹ����� Construct ���� ���� �Ҹ��Ƿ� Slate ���ð� ���õ� ó���� ���⼭ �ϸ� �ȵ�.
	UFUNCTION(BlueprintNativeEvent, Category = "GsManaged", meta = (DisplayName = "OnPush"))
	void OnPush(UGsUIParameter* InParam = nullptr);
	virtual void OnPush_Implementation(UGsUIParameter* InParam = nullptr);

	// ����: �ʱ�ȭ, �ı��ÿ��� ����� ��. �ʱ�ȭ �� bNotDestroy �� ���� ����. ����ȯ �� �ı�ó������.
	void SetEnableAutoDestroy(bool bInEnableAutoDestroy);

public:
	UFUNCTION(BlueprintCallable, Category = "GsManaged")
	virtual void Close();

	// FName�� �����ڷ� �� �� �ִ� �޽����� ����
	UFUNCTION(BlueprintNativeEvent, Category = "GsManaged", meta = (DisplayName = "OnMessage"))
	void OnMessage(FName InKey, UGsUIParameter* InParam = nullptr);
	virtual void OnMessage_Implementation(FName InKey, UGsUIParameter* InParam = nullptr);

public:
	// ���� �����Ǵ� UI�ΰ�
	virtual bool IsStackUI() const { return false; }

	// ������ Ÿ��(ȭ���� �� ������ Ÿ��. GsUIWindow ��Ӱ�ü)�ΰ�
	virtual bool IsWindow() const { return false; }

	// ���� �ε� �� �ı����� �ʴ� UI�ΰ�
	virtual bool IsNondestructiveWidget() const { return bNotDestroy; }

	// �ߺ� ������ ������ ����(Popup�� Tray���� ����Ѵ�)
	virtual bool CanMultipleInstance() const { return false; }

	// UIController���� ĳ���ϰ� �ִ� ���� 
	bool IsCachedWidget() const { return bIsCachedWidget; }

	UFUNCTION(BlueprintCallable, Category = "GsManaged")
	class UGsUIManager* GetUIManager();

	// Window < Popup < Tray ���� ������ ���� ��. �ڼ� Ŭ�������� �� �ο�(Window: 10, Popup: 100, Tray: 500)
	virtual int32 GetManagedDefaultZOrder() const { return 0; }
	virtual int32 GetManagedZOrder() const;

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
};
