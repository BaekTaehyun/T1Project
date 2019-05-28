// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SampleParam/GsGameObjectEventParamBase.h"

/**
 * ������Ʈ �̺�Ʈ ó�� ��� Ŭ����
 * ����� ���� ���� ���� �ʰ� LocalŬ�������� �����ص�..
 * ���� ���� ������ ����� �ʿ�
 */
class GAMESERVICE_API FGsGameObjectEventBase
{
public:
	FGsGameObjectEventBase(UGsGameObjectBase* owner);
	virtual ~FGsGameObjectEventBase();
	
	//�̺�Ʈ ������ �ൿ �޼��� ���
	virtual void RegistEvent();

	//�ӽ� ���� ����ü ���
	GsGameObjectEventParamBase* GetParam(MessageGameObject::Action Type);
	template<typename tCast>
	tCast* GetCastParam(MessageGameObject::Action Type) { return static_cast<tCast*>(GetParam(Type)); }

	void ClearParam(MessageGameObject::Action Type);

protected:
	virtual GsGameObjectEventParamBase* CreateParam(MessageGameObject::Action Type);

protected:
	class UGsGameObjectBase* Owner;

	//EventParam������ ĳ��
	TMap<MessageGameObject::Action, GsGameObjectEventParamBase*> EventParams;
};