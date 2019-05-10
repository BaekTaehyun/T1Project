#pragma once

#include "../Class/GSTState.h"
#include "../Class/GSTStateMng.h"
#include "../Class/GSTSingleton.h"
#include "../Class/GSIManager.h"
#include "GSFGameMode.h"
#include "GSFGameModeBase.h"

class GSFMessageNone;
//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 인스턴스 할당관리자(동일 인스턴스가 아닌 상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class GSFGameModeAllocator : GSTMapAllocator<GSFGameMode::Mode, GSFGameModeBase>
{
public:
	GSFGameModeAllocator() {}
	virtual ~GSFGameModeAllocator() {}
	virtual GSFGameModeBase* Alloc(GSFGameMode::Mode inMode) override;	
};

//------------------------------------------------------------------------------
// 게임모드를 관리
//------------------------------------------------------------------------------
class GSFGameModeManager : 
	public GSTStateMng<GSFGameMode::Mode, GSFGameModeBase, GSFGameModeAllocator>,
	public GSTSingleton<GSFGameModeManager>,
	public GSIManager
{
	typedef GSTStateMng<GSFGameMode::Mode, GSFGameModeBase, GSFGameModeAllocator> Super;
public:
	virtual ~GSFGameModeManager();

	//GSIManager
	virtual void Initialize() override;
	virtual void Finalize() override;

	void OnReconnectionStart(const GSFMessageNone& message);
	void OnReconnectionEnd(const GSFMessageNone& message);
};

typedef GSTSingleton<GSFGameModeManager> GSFGameModeSingle;
#define GMode() GSFGameModeSingle::Instance