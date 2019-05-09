#include "GSFMessageManager.h"

GSFMessageManager* GSFMessageSingle::Instance = NULL;

GSFMessageManager::~GSFMessageManager()
{
	int an = 0;
}

void GSFMessageManager::Initialize()
{
	GSTSingleton::InitInstance(this);
}

void GSFMessageManager::Finalize()
{
	GSTSingleton::RemoveInstance();
}