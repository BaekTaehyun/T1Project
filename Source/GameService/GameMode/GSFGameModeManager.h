#pragma once

#include "../Class/GSTState.h"
#include "../Class/GSTStateMng.h"
#include "../Class/GSTSingleton.h"
#include "../Class/GSIManager.h"
#include "GSFGameMode.h"
#include "GSFGameModeBase.h"

class GSFMessageNone;
//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �ν��Ͻ� �Ҵ������(���� �ν��Ͻ��� �ƴ� ��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFGameModeAllocator : GSTMapAllocator<GSFGameMode::Mode, GSFGameModeBase>
{
public:
	GSFGameModeAllocator() {}
	virtual ~GSFGameModeAllocator() {}
	virtual GSFGameModeBase* Alloc(GSFGameMode::Mode inMode) override;	
};

//------------------------------------------------------------------------------
// ���Ӹ�带 ����
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