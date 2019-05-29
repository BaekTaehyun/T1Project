// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Message/GsMessageGameObject.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"

/**
 * �ӽ� �޼��� ��� �Ķ���� ����ü �������̽� Ŭ����
 * �ϴ� MessageGameObject::Action Ÿ�Կ� 1:1 �����Ǵ� ���·� ����
 * �����ڿ��� Type���� ����
 */
class GAMESERVICE_API GsGameObjectEventParamBase
{
public:
	GsGameObjectEventParamBase();
	virtual ~GsGameObjectEventParamBase();

	virtual void Clear();

	template<class tCast>
	static tCast* ConstCast(const GsGameObjectEventParamBase& c) { return static_cast<tCast*>(const_cast<GsGameObjectEventParamBase*>(&c)); }

public:
	MessageGameObject::Action ActionType;
	UGsGameObjectBase* Target;
};

//Ż�� ž��
class GAMESERVICE_API GsGameObjectEventParamVehicleRide : public GsGameObjectEventParamBase
{
	typedef GsGameObjectEventParamBase Super;

public:
	GsGameObjectEventParamVehicleRide();

	virtual void Clear() override;

public:
	//ž����
	UGsGameObjectBase* Passenger;
};
