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
	// gvs 선택 여부
	bool _do_select = false;
	// 클라이언트 업데이트 필요성(마켓)
	bool _need_clientupdate = false;
	// 현재 클라이언트 버전
	FString _clientVersion;
	// 선택된 인포
	FGsGVSInfo*_selected;
};
