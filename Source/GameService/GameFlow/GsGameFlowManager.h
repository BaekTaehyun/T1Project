#pragma once

#include "../Class/GsState.h"
#include "../Class/GsStateMng.h"
#include "../Class/GsSingleton.h"
#include "../Class/GsManager.h"
#include "GsGameFlow.h"
#include "GsGameFlowBase.h"

class FGsMessageNone;
//------------------------------------------------------------------------------
// 게임모드를 관리할때 필요한 인스턴스 할당관리자(동일 인스턴스가 아닌 상속구조도 지원하기 위함)
//------------------------------------------------------------------------------
class FGsGameFlowAllocator : TGsMapAllocator<FGsGameFlow::Mode, FGsGameFlowBase>
{
public:
	FGsGameFlowAllocator() {}
	virtual ~FGsGameFlowAllocator() {}
	virtual FGsGameFlowBase* Alloc(FGsGameFlow::Mode inMode) override;
};

//------------------------------------------------------------------------------
// 게임모드를 관리
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