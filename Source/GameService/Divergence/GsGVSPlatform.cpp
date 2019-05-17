// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGVSPlatform.h"
#include "Kismet/GameplayStatics.h"

// 로그 출력
void FGsGVSInfo::PrintLog()
{
	UE_LOG(LogGVSInfo, Warning, 
		TEXT("platform: %s, service_type : %s, server_id: %s, auth_agent_port: %s, cdn_suburl: %s "),
		*platform, *service_type, *server_id, *auth_agent_port, *cdn_suburl);

	for (auto& iter: version)
	{
		UE_LOG(LogGVSInfo, Warning,
			TEXT("version: %s"), *iter);
	}

	for (auto& iter : auth_agent)
	{
		UE_LOG(LogGVSInfo, Warning,
			TEXT("auth_agent: %s"), *iter);
	}
}
// 현재 플랫폼과 같은지?
bool FGsGVSInfo::IsCurrentPlatform()
{
	// 현재 기기의 플랫폼 이름 가져오기
	FString findVal = UGameplayStatics::GetPlatformName();
	
	// 안드면
	if (findVal.Compare(TEXT("Android")) == 0)
	{
		// 인포의 텍스트랑 비교
		if (platform.Compare(TEXT("android")) == 0)
		{
			return true;
		}
	}
	// 나머진 ios다
	else
	{
		// 인포의 텍스트랑 비교
		if (platform.Compare(TEXT("ios")) == 0)
		{
			return true;
		}
	}

	return false;
}

// 서비스 타입이 동일한지?
bool FGsGVSInfo::IsSameServiceType(const FString& In_str)
{
	// 같으면 true, 다르면 false
	return (service_type.Compare(In_str) == 0) ? true : false;
}


// 버전 정보 파일을 가지고 있는지 검사
bool FGsGVSInfo::IsIncludeVersion(const FString& In_versetText)
{
	for (auto& iter:version)
	{
		// 같은지?
		if (iter.Compare(In_versetText) == 0)
		{
			return true;
		}
	}

	// 해당 버전이 없다
	return false;
}



GsGVSPlatform::GsGVSPlatform()
{
	_mapTypeStr.Add(EGsServiceType::live, TEXT("live"));
	_mapTypeStr.Add(EGsServiceType::submission, TEXT("submission"));
	_mapTypeStr.Add(EGsServiceType::qa, TEXT("qa"));
	_mapTypeStr.Add(EGsServiceType::dev, TEXT("dev"));
	_mapTypeStr.Add(EGsServiceType::Test01, TEXT("Test01"));
	_mapTypeStr.Add(EGsServiceType::Test02, TEXT("Test02"));
	_mapTypeStr.Add(EGsServiceType::MAX, TEXT("MAX"));
}

GsGVSPlatform::~GsGVSPlatform()
{
}


void GsGVSPlatform::SetInfo(TArray<FGsGVSInfo>& In_arrInfo)
{
	// 초기화
	// struct 메모리 초기화는 언제??
	_mapTypeGVS.Reset();

	// 모든 gvs에서 
	for (auto& iter:In_arrInfo)
	{
		// 현재 플랫폼과 같다면
		if (iter.IsCurrentPlatform() == true)
		{
			for (auto& typeIter: _mapTypeStr)
			{
				// 서비스 타입이 같으면
				if (iter.IsSameServiceType(typeIter.Value) == true)
				{
					// 추가
					_mapTypeGVS.Add(typeIter.Key, iter);

					UE_LOG(LogGVSInfo, Warning,
						TEXT("typeStr12345: %s, platform: %s, cdn_suburl: %s" ), 
						*typeIter.Value,
						*iter.platform, 
						*iter.cdn_suburl);
				}
				
			}
		}
	}
}
// 기준이 되는 라이브 gvs 정보가 존재하는지 확인 
bool GsGVSPlatform::VerifyliveGvs()
{
	// 키값이 있는지?
	bool isExist = _mapTypeGVS.Contains(EGsServiceType::live);

	if (isExist == false)
	{
		return false;
	}
	// 값이 nullptr이 아닌지??
	return (_mapTypeGVS.Find(EGsServiceType::live) != nullptr);
}

// 접근자
FGsGVSInfo* GsGVSPlatform::GetTypeGVS(EGsServiceType In_type)
{
	return _mapTypeGVS.Find(In_type);
}

DEFINE_LOG_CATEGORY(LogGVSInfo);