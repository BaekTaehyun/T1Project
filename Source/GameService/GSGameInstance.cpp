// Fill out your copyright notice in the Description page of Project Settings.

#include "GSGameInstance.h"
#include "GameService.h"

//-------------------------------------------------------------------------------
// �����ʱ�ȭ ����
//-------------------------------------------------------------------------------
// 1. ���� ���� �ʱ�ȭ(PlayMode)	:	UGameInstance::Init
// 2. ������ ���� ������ �ʱ�ȭ		:	AActor::PostInitailizeComponents
// 3. �÷��̾��� �α���			:	AGameMode::PostLogin
// 4. ������ ����					:	AGameMode::StartPlay -> AActor::BeginPlay
//-------------------------------------------------------------------------------

// �÷����� �ʱ�ȭ, �÷��̸�� ����� ȣ��� ------- �ߺ�ó�� ����
UGSGameInstance::UGSGameInstance()
{
	GSLOG_S(Warning);
}

// �÷��̸��(�� ��������)������ ȣ��
void UGSGameInstance::Init()
{
	GSLOG_S(Warning);
	Super::Init();
}



