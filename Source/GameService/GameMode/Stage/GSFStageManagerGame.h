#pragma once


#include "../../Class/GSTState.h"
#include "../../Class/GSTStateMng.h"
#include "GSFStageMode.h"
#include "GSFStageGameBase.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �ν��Ͻ� �Ҵ������(���� �ν��Ͻ��� �ƴ� ��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class GSFStageGameAllocator : GSTMapAllocator<GSFStageMode::Game, GSFStageGameBase>
{
public:
	GSFStageGameAllocator() {}
	virtual ~GSFStageGameAllocator() {}
	virtual GSFStageGameBase* Alloc(GSFStageMode::Game inMode) override;
};

//------------------------------------------------------------------------------
// ���Ӹ�带 ����
//------------------------------------------------------------------------------
class GSStageManagerGame : public GSTStateMng<GSFStageMode::Game, GSFStageGameBase, GSFStageGameAllocator>
{
public:
	virtual ~GSStageManagerGame() {};
	virtual void RemoveAll() override;
	virtual void InitState() override;
};
