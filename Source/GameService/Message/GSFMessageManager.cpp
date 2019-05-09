#include "GSFMessageManager.h"

GSFMessageManager* GSFMessageSingle::Instance = NULL;

void GSFMessageManager::Init()
{
	GSTSingleton::InitInstance(this);
}

void GSFMessageManager::RemoveAll()
{
	GSTSingleton::RemoveInstance();
}