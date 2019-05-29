// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Message/GsMessageGameObject.h"
#include "GameObject/ObjectClass/GsGameObjectBase.h"

/**
 * 임시 메세지 기반 파라미터 구조체 인터페이스 클래스
 * 일단 MessageGameObject::Action 타입에 1:1 대응되는 형태로 구현
 * 생성자에서 Type값을 설정
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

//탈것 탑승
class GAMESERVICE_API GsGameObjectEventParamVehicleRide : public GsGameObjectEventParamBase
{
	typedef GsGameObjectEventParamBase Super;

public:
	GsGameObjectEventParamVehicleRide();

	virtual void Clear() override;

public:
	//탑승자
	UGsGameObjectBase* Passenger;
};
