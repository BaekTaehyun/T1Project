#pragma once


#include "../../../Class/GsState.h"
#include "../../../Class/GsStateMng.h"
#include "../GsStageMode.h"
#include "GsStageGameBase.h"

//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �ν��Ͻ� �Ҵ������(���� �ν��Ͻ��� �ƴ� ��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsStageGameAllocator : TGsMapAllocator<FGsStageMode::Game, FGsStageGameBase>
{
public:
	FGsStageGameAllocator() {}
	virtual ~FGsStageGameAllocator() {}
	virtual FGsStageGameBase* Alloc(FGsStageMode::Game inMode) override;
};

//------------------------------------------------------------------------------
// �ΰ��� ������������ �����Ѵ�.
//------------------------------------------------------------------------------
class FGsStageManagerGame : public TGsStateMng<FGsStageMode::Game, FGsStageGameBase, FGsStageGameAllocator>
{
	typedef TGsStateMng<FGsStageMode::Game, FGsStageGameBase, FGsStageGameAllocator> Super;
public:
	virtual ~FGsStageManagerGame() {};
	virtual void RemoveAll() override;
	virtual void InitState() override;
};
