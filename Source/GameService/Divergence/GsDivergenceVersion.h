// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
/**
 * TO DO: GsGVSSelector���� ���� ���� ���� �� ��� �������� ���ؾ���
 */
class UFileDownloadManager;
class GsGVSPlatform;
class GsGVSSelector;

class GsDivergenceVersion
{
public:
	GsDivergenceVersion();
	virtual ~GsDivergenceVersion();

	// gvs �ε�
	void LoadGVS();
	// gvs �Ľ�
	void ParseGVS(const FString& In_filePath);

	bool IsInit() { return _bInit; }
	
private:
	// ���� �ٿ� �Ŵ���	
	UFileDownloadManager* _fileDownloadManager;
	
	// �÷����� ���� gvs ����
	GsGVSPlatform* _platform;

	// ���� ����Ȳ�� �´� ���� 
	GsGVSSelector* _selector;

	bool _bInit = false;
};

DECLARE_LOG_CATEGORY_EXTERN(LogDivergenceVersion, Log, All);