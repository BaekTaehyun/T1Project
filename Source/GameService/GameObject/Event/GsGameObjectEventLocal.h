// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameObjectEventBase.h"

/**
 * 
 */
class GAMESERVICE_API FGsGameObjectEventLocal : public FGsGameObjectEventBase
{
	typedef FGsGameObjectEventBase Super;

public:
	FGsGameObjectEventLocal(UGsGameObjectBase* owner);
	virtual ~FGsGameObjectEventLocal();

	virtual void RegistEvent() override;

protected:
	virtual GsGameObjectEventParamBase* CreateParam(MessageGameObject::Action Type) override;

	//행동 처리 메서드 모읍
protected:
	void OnVehicleRide(const class GsGameObjectEventParamBase& Param);
};
