#pragma once

#include "../Class/GSTState.h"
#include "../Class/GSTStateMng.h"
#include "../Class/GSTMessageHandler.h"
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
class GSFGameModeManager : public GSTStateMng<GSFGameMode::Mode, GSFGameModeBase, GSFGameModeAllocator>
{
public:
	virtual ~GSFGameModeManager() {};
	virtual void RemoveAll() override;
	virtual void InitState() override;
};