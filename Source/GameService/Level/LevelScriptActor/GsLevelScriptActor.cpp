// Fill out your copyright notice in the Description page of Project Settings.


#include "GsLevelScriptActor.h"

void AGsLevelScriptActor::AddPlayerSpawnPoint(FString in_name, APlayerSpawnPoint * in_point)
{
	if (false == _PlayerSpawnMap.Contains(in_name))
	{
		_PlayerSpawnMap.Add(in_name, in_point);
	}
}

void AGsLevelScriptActor::RemovePlayerSpawnPoint(FString in_name)
{
	_PlayerSpawnMap.Remove(in_name);
}

bool AGsLevelScriptActor::TryGetPlayerSpawnPoint(FString in_tag, APlayerSpawnPoint*& out_point)
{
	APlayerSpawnPoint** find = _PlayerSpawnMap.Find(in_tag);

	if (find)
	{
		out_point = (*find);

		return true;
	}

	return false;
}