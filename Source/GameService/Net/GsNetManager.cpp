
#include "GsNetManager.h"
#include "GsNetLobby.h"
#include "GsNetGame.h"

//------------------------------------------------------------------

//------------------------------------------------------------------
FGsNetBase* FGsNetAllocator::Alloc(FGsNet::Mode inMode)
{
	if (FGsNet::Mode::LOBBY == inMode)
	{
		return new FGsNetLobby();
	}
	else if (FGsNet::Mode::GAME == inMode)
	{
		return new FGsNetGame();
	}
	else if (FGsNet::Mode::RECONNECT == inMode)
	{
		return NULL;
	}
	return NULL;
}
//------------------------------------------------------------------

//------------------------------------------------------------------
template<>
FGsNetManager* GSFNetSingle::_instance = nullptr;

FGsNetManager::~FGsNetManager()
{
	int a = 0;
}

void FGsNetManager::Initialize()
{
	TGsSingleton::InitInstance(this);

	const FGsNet::Mode allMode [] = { FGsNet::Mode::LOBBY, FGsNet::Mode::GAME };
	for (auto& e : allMode)
	{
		MakeInstance(e);
	}

	ChangeState(FGsNet::Mode::LOBBY);

	Super::InitState();
}

void FGsNetManager::Finalize()
{
	Super::RemoveAll();
	TGsSingleton::RemoveInstance();
}