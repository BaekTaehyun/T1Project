#pragma once
#include "../Class/GSTState.h"
#include "../Class/GSTStateMng.h"
#include "../Class/GSTSingleton.h"
#include "../Class/GSIManager.h"
#include "GSFNet.h"
#include "GSFNetBase.h"


class GSFNetAllocator : GSTMapAllocator<GSFNet::Mode, GSFNetBase>
{
public:
	GSFNetAllocator() {}
	virtual ~GSFNetAllocator() {}
	virtual GSFNetBase* Alloc(GSFNet::Mode inMode) override;
};


class GSFNetManager :
	public GSTStateMng<GSFNet::Mode, GSFNetBase, GSFNetAllocator>,
	public GSTSingleton<GSFNetManager>,
	public GSIManager
{
	typedef GSTStateMng<GSFNet::Mode, GSFNetBase, GSFNetAllocator> Super;
public:
	virtual ~GSFNetManager();

	//GSIManager
	virtual void Initialize() override;
	virtual void Finalize() override;
};