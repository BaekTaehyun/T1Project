// Fill out your copyright notice in the Description page of Project Settings.

#include "GsInputBindingBase.h"

UGsInputBindingBase::UGsInputBindingBase()
{
}

UGsInputBindingBase::~UGsInputBindingBase()
{
}

void UGsInputBindingBase::Initialize()
{
}

void UGsInputBindingBase::Finalize()
{
}

void UGsInputBindingBase::SetBinding(UInputComponent* input)
{
	InputComponent = input;
}