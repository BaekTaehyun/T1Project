// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGVSSelector.h"
#include "GsServiceType.h"
#include "GsGVSPlatform.h"

GsGVSSelector::GsGVSSelector()
{
}

GsGVSSelector::~GsGVSSelector()
{
}
void GsGVSSelector::LoadClientVersion()
{
	// 언리얼 에디터에서 버전가져오는거 필요...
	//UWorld* world = GEngine->;

}

void GsGVSSelector::DoSelect(GsGVSPlatform* In_platform)
{
	// 지금은 그냥 라이브꺼로 세팅하게 함...
	_selected = In_platform->GetTypeGVS(EGsServiceType::live);
	_do_select = true;
	
}