#include "GsMessageManager.h"

FGsMessageManager* FGsMessageSingle::Instance = NULL;

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
	//bak1210 ������ �������� �Ѵ�.
	_system.Update();
	_net.Update();
	_hive.Update();
	_stage.Update();
}