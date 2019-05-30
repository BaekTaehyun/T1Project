// Fill out your copyright notice in the Description page of Project Settings.


#include "GsTestGameMode.h"
#include "../Divergence/GsDivergenceVersion.h"
void AGsTestGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogDownloadMode, Warning,
		TEXT("DownloadMode begin play"));

	_divergenceVersion = MakeShareable(new GsDivergenceVersion());

	// �̻��� ������
	// �ʱ�ȭ �ȵǾ�����
	if (_divergenceVersion.IsValid() == true &&
		_divergenceVersion->IsInit() == false)
	{
		// gvs �ε�
		_divergenceVersion->LoadGVS();
	}
}

DEFINE_LOG_CATEGORY(LogDownloadMode);