// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GsServiceType.h"
#include "GsGVSPlatform.generated.h"
/**
 * 
 */

// gvs 기본 정보
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
	// 로그 출력
	void PrintLog();

	// 현재 플랫폼과 같은지?
	bool IsCurrentPlatform();
	// 서비스 타입이 동일한지?
	bool IsSameServiceType(const FString& In_str);
	// 버전 정보 파일을 가지고 있는지 검사
	bool IsIncludeVersion(const FString& In_versetText);
};



// 플랫폼 정보
class  GsGVSPlatform
{
public:
		
	GsGVSPlatform();
	~GsGVSPlatform();

	// gvs json 데이터 가공
	void SetInfo(TArray<FGsGVSInfo>& In_arrInfo);
	// 기준이 되는 라이브 gvs 정보가 존재하는지 확인 
	bool VerifyliveGvs();
	// 접근자
	FGsGVSInfo* GetTypeGVS(EGsServiceType In_type);
private:

	// 타입별 스트링
	TMap<EGsServiceType, FString> _mapTypeStr;
	// 타입별 gvs 정보
	TMap<EGsServiceType, FGsGVSInfo> _mapTypeGVS;
};

DECLARE_LOG_CATEGORY_EXTERN(LogGVSInfo, Log, All);