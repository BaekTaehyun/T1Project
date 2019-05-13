#include "GsMessageManager.h"

FGsMessageManager* GSFMessageSingle::Instance = NULL;

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