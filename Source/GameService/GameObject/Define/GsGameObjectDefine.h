// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsGameObjectDefine.generated.h"

/**
 * 오브젝트 클래스 관련 EunmType/Define 모음
 */


/**
* Bit Mask Define
*/
#define CHECK_FLAG_TYPE(Type, Flag)     (((Type) & static_cast<uint8>(Flag)) > 0)
#define SET_FLAG_TYPE(Type, Flag)       (Type |= static_cast<uint8>(Flag))
#define REMOVE_FLAG_TYPE(Type, Flag)    (Type &= ~static_cast<uint8>(Flag))
#define CLEAR_FLAG_TYPE(Type)           (Type = 0x00)


//UENUM이 uint8밖에 지원하지 않는다 Base 타입은 None으로 두어 일단 하나의 여분을 챙겨둔다.
//확장성에 제한이 많이 생길것 같아서, 추후 확장이 필요할 경우 bitMask를 포기..
//포기하였을 경우 동급 레이어에 해당 하는 타입에 대한 고민 필요(ex Static, Projectile, Dynamic, Vehicle)
//즉 Base타입은 All과 같음
UENUM(BlueprintType, meta = (Bitflags))
enum class EGsGameObjectType : uint8
{
	Base		= 0x00,		// All  hierarchy : 0
	Static		= 0x01,		//hierarchy : 1
	Projectile	= 0x02,		//hierarchy : 1
	Vehicle		= 0x04,		//hierarchy : 1
	Dynamic		= 0x08,		//hierarchy : 1
	Player		= 0x10,		//hierarchy : 2
	NonPlayer	= 0x20,		//hierarchy : 2
	OtherPlayer = 0x40,		//hierarchy : 3
	LocalPlayer = 0x80,		//hierarchy : 3
};

ENUM_CLASS_FLAGS(EGsGameObjectType);

//[Todo] Enum Loop에 대한 더 좋은 방법이 있다면 수정한다.
static const EGsGameObjectType EGsGameObjectTypeALL[] = 
{ 
	EGsGameObjectType::Base, EGsGameObjectType::Static, EGsGameObjectType::Projectile, EGsGameObjectType::Vehicle,  
	EGsGameObjectType::Dynamic, EGsGameObjectType::Player, EGsGameObjectType::NonPlayer, EGsGameObjectType::OtherPlayer,
	EGsGameObjectType::LocalPlayer
};
static const int EGsGameObjectTypeALLCount = int(sizeof(EGsGameObjectTypeALL) / sizeof(*EGsGameObjectTypeALL));

/**
* Lowwer State Define
*/
UENUM()
enum class EGsStateBase : uint8
{
	None = 0,
	Spawn,
	Idle,
	ForwardWalk,
	BackwardWalk,
	SideWalk,
	Run,

	Ride,

	Beaten,
	Die,

	BaseMax,
};


/**
* Upper State Define
*/
UENUM()
enum class  EGsStateUpperBase : uint8
{
	None = 100,
	Idle,
	Attack,

	UpperBaseMax,
};


/**
* 이동 관련 타입 설정
* None : 정지를 의미한다.
*/
UENUM(BlueprintType, meta = (Bitflags))
enum class EGsGameObjectMoveType : uint8
{
    None            = 0x000,
    Walk            = 0x001,
    Run             = 0x002,
    Interpolation   = 0x004,      //추후 이동 동기화 관련 필요성이 있을지도...
    Jump            = 0x008,      //미사용 가능성이 있음
};

/**
* 이동 스타일 타입
*/
UENUM(BlueprintType)
enum class EGsGameObjectMoveDirType : uint8
{
    Forward,
    SideStep,
    Backward,
};

/**
* 캐릭터 파츠 타입
*/
UENUM()
enum class EGsPartsType : uint8
{
	HAIR,
	FACE,
	HEAD,
	BODY,
	SHOULDER,
	GLOVE,
	LEG,
	FOOT,
};

/**
* 스킬 액션 타입
*/
UENUM()
enum class SkillActionType : uint8
{
	CreateProjectile = 1,
};



