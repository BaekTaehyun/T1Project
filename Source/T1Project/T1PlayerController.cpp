// Fill out your copyright notice in the Description page of Project Settings.
#include "T1PlayerController.h"
#include "T1Project.h"


void AT1PlayerController::PostInitializeComponents()
{
	T1LOG_S(Warning);
	Super::PostInitializeComponents();
}

void AT1PlayerController::Possess(APawn* pawn)
{
	T1LOG_S(Warning);
	Super::Possess(pawn);
}

void AT1PlayerController::BeginPlay()
{
	Super::BeginPlay();

	//�÷��� ���������� �𸮾� ����Ʈ�� ������ �ʾƵ� �ǵ��� ����
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}