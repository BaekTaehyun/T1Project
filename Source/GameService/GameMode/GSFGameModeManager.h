#pragma once

#include "../Class/GSTState.h"
#include "../Class/GSTStateMng.h"
#include "../Class/GSTSingleton.h"
#include "../Class/GSIManager.h"
#include "GSFGameMode.h"
#include "GSFGameModeBase.h"
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

	void OnReconnectionStart(const GTSMessageNone& message);
	void OnReconnectionEnd(const GTSMessageNone& message);
};

typedef GSTSingleton<GSFGameModeManager> GSFGameModeSingle;