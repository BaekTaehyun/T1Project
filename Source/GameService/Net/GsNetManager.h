#pragma once
#include "../Class/GsState.h"
#include "../Class/GsStateMng.h"
#include "../Class/GsSingleton.h"
#include "../Class/GsManager.h"
#include "GsNet.h"
#include "GsNetBase.h"


class FGsNetAllocator : TGsMapAllocator<FGsNet::Mode, FGsNetBase>
{
public:
	FGsNetAllocator() {}
	virtual ~FGsNetAllocator() {}
	virtual FGsNetBase* Alloc(FGsNet::Mode inMode) override;
};


class FGsNetManager :
	public TGsStateMng<FGsNet::Mode, FGsNetBase, FGsNetAllocator>,
	public TGsSingleton<FGsNetManager>,
	public IGsManager
{
	typedef TGsStateMng<FGsNet::Mode, FGsNetBase, FGsNetAllocator> Super;
public:
	virtual ~FGsNetManager();

	//IGsManager
	virtual void Initialize() override;
	virtual void Finalize() override;
};

typedef TGsSingleton<FGsNetManager>	GSFNetSingle;
#define GNet()	GSFNetSingle::Instance()