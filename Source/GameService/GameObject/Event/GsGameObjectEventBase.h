// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SampleParam/GsGameObjectEventParamBase.h"

/**
 * 오브젝트 이벤트 처리 담당 클래스
 * 현재는 따로 구분 두지 않고 Local클래스에만 정의해둠..
 * 추후 관련 구조는 고민이 필요
 */
class GAMESERVICE_API FGsGameObjectEventBase
{
public:
	FGsGameObjectEventBase(UGsGameObjectBase* owner);
	virtual ~FGsGameObjectEventBase();
	
	//이벤트 유형별 행동 메서드 등록
	virtual void RegistEvent();

	//임시 전달 구조체 얻기
	GsGameObjectEventParamBase* GetParam(MessageGameObject::Action Type);
	template<typename tCast>
	tCast* GetCastParam(MessageGameObject::Action Type) { return static_cast<tCast*>(GetParam(Type)); }

	void ClearParam(MessageGameObject::Action Type);

protected:
	virtual GsGameObjectEventParamBase* CreateParam(MessageGameObject::Action Type);

protected:
	class UGsGameObjectBase* Owner;

	//EventParam데이터 캐싱
	TMap<MessageGameObject::Action, GsGameObjectEventParamBase*> EventParams;
};