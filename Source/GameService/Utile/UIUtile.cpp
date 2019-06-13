// Fill out your copyright notice in the Description page of Project Settings.


#include "UIUtile.h"
#include "Runtime/Engine/Classes/Camera/PlayerCameraManager.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

bool UUIUtile::TryGetRotationForFacePlayerCamera(UWorld* in_world, FVector in_location, FRotator& out_rot)
{
	APlayerController* player = in_world->GetFirstPlayerController();

	if (player)
	{
		APlayerCameraManager* camera = player->PlayerCameraManager;

		if (camera)
		{
			out_rot = UKismetMathLibrary::FindLookAtRotation(in_location, camera->GetCameraLocation());

			return true;
		}
	}

	return false;
}