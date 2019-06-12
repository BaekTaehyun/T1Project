// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelUtile.h"

FName ULevelUtile::GetLevelNameFromActor(AActor* Actor)
{
	if (Actor)
	{
		ULevel* level = Actor->GetLevel();

		if (level)
		{
			UObject* outer = level->GetOuter();

			if (outer)
			{
				return outer->GetFName();
			}
		}
	}

	return NAME_None;
}
