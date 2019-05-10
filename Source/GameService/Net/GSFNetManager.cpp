
#include "GSFNetManager.h"
#include "GSFNetLobby.h"
#include "GSFNetGame.h"

//------------------------------------------------------------------

//------------------------------------------------------------------
GSFNetBase* GSFNetAllocator::Alloc(GSFNet::Mode inMode)
{
	if (GSFNet::Mode::LOBBY == inMode)
	{
		return new GSFNetLobby();
	}
	else if (GSFNet::Mode::GAME == inMode)
	{
		return new GSFNetGame();
	}
	else if (GSFNet::Mode::RECONNECT == inMode)
	{
		return NULL;
	}
	return NULL;
}
//------------------------------------------------------------------

//------------------------------------------------------------------

GSFNetManager* GSFNetSingle::Instance = NULL;
GSFNetManager::~GSFNetManager()
{
	int a = 0;
}

void GSFNetManager::Initialize()
{
	GSTSingleton::InitInstance(this);

	constexpr std::initializer_list<GSFNet::Mode> allMode = { GSFNet::Mode::LOBBY, GSFNet::Mode::GAME };
	for (auto& e : allMode)
	{
		MakeInstance(e);
	}

	ChangeState(GSFNet::Mode::LOBBY);

	Super::InitState();
}

void GSFNetManager::Finalize()
{
	Super::RemoveAll();
	GSTSingleton::RemoveInstance();
}