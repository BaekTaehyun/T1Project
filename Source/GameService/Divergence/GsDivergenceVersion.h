// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
/**
 * TO DO: GsGVSSelector에서 현재 버전 정의 및 어디서 참조할지 정해야함
 */
class UFileDownloadManager;
class GsGVSPlatform;
class GsGVSSelector;

class GsDivergenceVersion
{
public:
	GsDivergenceVersion();
	virtual ~GsDivergenceVersion();

	// gvs 로드
	void LoadGVS();
	// gvs 파싱
	void ParseGVS(const FString& In_filePath);

	bool IsInit() { return _bInit; }
	
private:
	// 파일 다운 매니저	
	UFileDownloadManager* _fileDownloadManager;
	
	// 플랫폼에 따른 gvs 정보
	GsGVSPlatform* _platform;

	// 현재 내상황에 맞는 정보 
	GsGVSSelector* _selector;

	bool _bInit = false;
};

DECLARE_LOG_CATEGORY_EXTERN(LogDivergenceVersion, Log, All);