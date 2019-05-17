// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsServiceType.h"
#include "GsGVSPlatform.generated.h"
/**
 * 
 */

// gvs �⺻ ����
USTRUCT()
struct FGsGVSInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FString platform;

	UPROPERTY()
	FString service_type;

	UPROPERTY()
	TArray<FString> version;
	UPROPERTY()
	FString server_id;
	UPROPERTY()
	TArray<FString> auth_agent;
	UPROPERTY()
	FString auth_agent_port;
	UPROPERTY()
	FString cdn_suburl;

public:
	// �α� ���
	void PrintLog();

	// ���� �÷����� ������?
	bool IsCurrentPlatform();
	// ���� Ÿ���� ��������?
	bool IsSameServiceType(const FString& In_str);
	// ���� ���� ������ ������ �ִ��� �˻�
	bool IsIncludeVersion(const FString& In_versetText);
};



// �÷��� ����
class  GsGVSPlatform
{
public:
		
	GsGVSPlatform();
	~GsGVSPlatform();

	// gvs json ������ ����
	void SetInfo(TArray<FGsGVSInfo>& In_arrInfo);
	// ������ �Ǵ� ���̺� gvs ������ �����ϴ��� Ȯ�� 
	bool VerifyliveGvs();
	// ������
	FGsGVSInfo* GetTypeGVS(EGsServiceType In_type);
private:

	// Ÿ�Ժ� ��Ʈ��
	TMap<EGsServiceType, FString> _mapTypeStr;
	// Ÿ�Ժ� gvs ����
	TMap<EGsServiceType, FGsGVSInfo> _mapTypeGVS;
};

DECLARE_LOG_CATEGORY_EXTERN(LogGVSInfo, Log, All);