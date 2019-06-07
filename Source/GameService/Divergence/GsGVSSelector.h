// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class GsGVSPlatform;
struct FGsGVSInfo;
/**
 * 
 */
class  GsGVSSelector
{
public:
	GsGVSSelector();
	virtual ~GsGVSSelector();

	bool IsSelected()
	{
		return _do_select;
	}
	bool IsNeedClientUpdate()
	{
		return _need_clientupdate;
	}

	FGsGVSInfo* Selected()
	{
		return _selected;
	}
	void LoadClientVersion();

	void DoSelect(GsGVSPlatform* In_platform);
private:
	// gvs ���� ����
	bool _do_select = false;
	// Ŭ���̾�Ʈ ������Ʈ �ʿ伺(����)
	bool _need_clientupdate = false;
	// ���� Ŭ���̾�Ʈ ����
	FString _clientVersion;
	// ���õ� ����
	FGsGVSInfo*_selected;
};
