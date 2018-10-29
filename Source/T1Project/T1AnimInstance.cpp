// Fill out your copyright notice in the Description page of Project Settings.

#include "T1AnimInstance.h"

UT1AnimInstance::UT1AnimInstance()
{
	CurrentPawnSpeed = 0.0f;
}

void UT1AnimInstance::NativeUpdateAnimation(float deltaSecond)
{
	Super::NativeUpdateAnimation(deltaSecond);

	auto Pawn = TryGetPawnOwner();
	if (::IsValid(Pawn))
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
	}
}