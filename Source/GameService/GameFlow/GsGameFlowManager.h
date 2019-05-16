#pragma once

#include "../Class/GsState.h"
#include "../Class/GsStateMng.h"
#include "../Class/GsSingleton.h"
#include "../Class/GsManager.h"
#include "GsGameFlow.h"
#include "GsGameFlowBase.h"

class FGsMessageNone;
//------------------------------------------------------------------------------
// ���Ӹ�带 �����Ҷ� �ʿ��� �ν��Ͻ� �Ҵ������(���� �ν��Ͻ��� �ƴ� ��ӱ����� �����ϱ� ����)
//------------------------------------------------------------------------------
class FGsGameFlowAllocator : TGsMapAllocator<FGsGameFlow::Mode, FGsGameFlowBase>
{
public:
	FGsGameFlowAllocator() {}
	virtual ~FGsGameFlowAllocator() {}
	virtual FGsGameFlowBase* Alloc(FGsGameFlow::Mode inMode) override;
};

//------------------------------------------------------------------------------
// ���Ӹ�带 ����
//------------------------------------------------------------------------------
class FGsGameFlowManager : 
	public TGsStateMng<FGsGameFlow::Mode, FGsGameFlowBase, FGsGameFlowAllocator>,
	public TGsSingleton<FGsGameFlowManager>,
	public IGsManager
{
	typedef TGsStateMng<FGsGameFlow::Mode, FGsGameFlowBase, FGsGameFlowAllocator> Super;
public:
	virtual ~FGsGameFlowManager();

	//IGsManager
	virtual void Initialize() override;
	virtual void Finalize() override;

	void OnReconnectionStart();
	void OnReconnectionEnd();
};

typedef TGsSingleton<FGsGameFlowManager> FGsGameFlowSingle;
#define GMode() FGsGameFlowSingle::Instance