// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class UFileDownloadManager;
class GsGVSPlatform;
class GsGVSSelector;

class GsDivergenceVersion
{
public:
	GsDivergenceVersion();
	~GsDivergenceVersion();

	// gvs 로드
	void LoadGVS();
	// gvs 파싱
	void ParseGVS(const FString& In_filePath);

	bool IsInit() { return _bInit; }
	
private:
	// 파일 다운 매니저	
	UFileDownloadManager* _fileDownloadManager;
	
	// 플랫폼에 따른 gvs 정보
	TSharedPtr<GsGVSPlatform> _platform;

	// 현재 내상황에 맞는 정보 
	TSharedPtr<GsGVSSelector> _selector;

	bool _bInit = false;
};

DECLARE_LOG_CATEGORY_EXTERN(LogDivergenceVersion, Log, All);