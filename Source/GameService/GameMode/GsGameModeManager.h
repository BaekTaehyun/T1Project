#pragma once

#include "../Class/GsState.h"
#include "../Class/GsStateMng.h"
#include "../Class/GsSingleton.h"
#include "../Class/GsManager.h"
#include "GsGameMode.h"
#include "GsGameModeBase.h"

class FGsMessageNone;
//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 인스턴스 할당관리자(동일 인스턴스가 아닌 상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class FGsGameModeAllocator : TGsMapAllocator<FGsGameMode::Mode, FGsGameModeBase>
{
public:
	FGsGameModeAllocator() {}
	virtual ~FGsGameModeAllocator() {}
	virtual FGsGameModeBase* Alloc(FGsGameMode::Mode inMode) override;
};

//------------------------------------------------------------------------------
// 게임모드를 관리
//------------------------------------------------------------------------------
class FGsGameModeManager : 
	public TGsStateMng<FGsGameMode::Mode, FGsGameModeBase, FGsGameModeAllocator>,
	public TGsSingleton<FGsGameModeManager>,
	public IGsManager
{
	typedef TGsStateMng<FGsGameMode::Mode, FGsGameModeBase, FGsGameModeAllocator> Super;
public:
	virtual ~FGsGameModeManager();

	//IGsManager
	virtual void Initialize() override;
	virtual void Finalize() override;

	void OnReconnectionStart();
	void OnReconnectionEnd();
};

typedef TGsSingleton<FGsGameModeManager> FGsGameModeSingle;
#define GMode() FGsGameModeSingle::Instance