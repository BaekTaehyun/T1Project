#include "GsMessageManager.h"

template<>
FGsMessageManager* FGsMessageSingle::_instance = nullptr;

FGsMessageManager::~FGsMessageManager()
{
	int an = 0;
}

void FGsMessageManager::Initialize()
{
	TGsSingleton::InitInstance(this);
}

void FGsMessageManager::Finalize()
{
	TGsSingleton::RemoveInstance();
}

void FGsMessageManager::Update()
{
#pragma todo("bak1210 : Message Update Sequence Set Need!")
	_system.Update();
	_net.Update();
	_hive.Update();
	_stage.Update();
}