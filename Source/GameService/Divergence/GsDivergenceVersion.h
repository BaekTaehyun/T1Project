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

	// gvs �ε�
	void LoadGVS();
	// gvs �Ľ�
	void ParseGVS(const FString& In_filePath);

	bool IsInit() { return _bInit; }
	
private:
	// ���� �ٿ� �Ŵ���	
	UFileDownloadManager* _fileDownloadManager;
	
	// �÷����� ���� gvs ����
	TSharedPtr<GsGVSPlatform> _platform;

	// ���� ����Ȳ�� �´� ���� 
	TSharedPtr<GsGVSSelector> _selector;

	bool _bInit = false;
};

DECLARE_LOG_CATEGORY_EXTERN(LogDivergenceVersion, Log, All);