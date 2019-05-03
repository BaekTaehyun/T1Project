#pragma once


#include "../../Class/GSTState.h"
#include "../../Class/GSTStateMng.h"
#include "GSFStageMode.h"
#include "GSFStageGameBase.h"

//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 인스턴스 할당관리자(동일 인스턴스가 아닌 상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class GSFStageGameAllocator : GSTMapAllocator<GSFStageMode::Game, GSFStageGameBase>
{
public:
	GSFStageGameAllocator() {}
	virtual ~GSFStageGameAllocator() {}
	virtual GSFStageGameBase* Alloc(GSFStageMode::Game inMode) override;
};

//------------------------------------------------------------------------------
// 게임모드를 관리
//------------------------------------------------------------------------------
class GSStageManagerGame : public GSTStateMng<GSFStageMode::Game, GSFStageGameBase, GSFStageGameAllocator>
{
public:
	virtual ~GSStageManagerGame() {};
	virtual void RemoveAll() override;
	virtual void InitState() override;
};
