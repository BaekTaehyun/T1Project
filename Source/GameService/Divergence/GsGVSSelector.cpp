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
	// �𸮾� �����Ϳ��� �����������°� �ʿ�...
	//UWorld* world = GEngine->;

}

void GsGVSSelector::DoSelect(GsGVSPlatform* In_platform)
{
	// ������ �׳� ���̺겨�� �����ϰ� ��...
	_selected = In_platform->GetTypeGVS(EGsServiceType::live);
	_do_select = true;
	
}