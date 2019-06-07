// Fill out your copyright notice in the Description page of Project Settings.


#include "GsTestGameMode.h"
#include "../Divergence/GsDivergenceVersion.h"
void AGsTestGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogDownloadMode, Warning,
		TEXT("DownloadMode begin play"));

	_divergenceVersion = MakeShareable(new GsDivergenceVersion());

	// 이상이 없으면
	// 초기화 안되었으면
	if (_divergenceVersion.IsValid() == true &&
		_divergenceVersion->IsInit() == false)
	{
		// gvs 로드
		_divergenceVersion->LoadGVS();
	}
}

DEFINE_LOG_CATEGORY(LogDownloadMode);