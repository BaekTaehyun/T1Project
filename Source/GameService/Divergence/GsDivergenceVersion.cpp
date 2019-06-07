// Fill out your copyright notice in the Description page of Project Settings.


#include "GsDivergenceVersion.h"
#include "FileDownloadManager.h"
#include "Paths.h"
#include "FileHelper.h"
#include "GsGVSPlatform.h"
#include "GsGVSSelector.h"
#include "JsonObjectConverter.h"


GsDivergenceVersion::GsDivergenceVersion()
{
	_fileDownloadManager = NewObject<UFileDownloadManager>();

	// 플랫폼 생성
	_platform = new GsGVSPlatform();
	// 선택 생성
	_selector = new GsGVSSelector();

	UE_LOG(LogDivergenceVersion, Warning,
		TEXT("DivergenceVersion construct 724"));
}

GsDivergenceVersion::~GsDivergenceVersion()
{
	// 포인터 해제
	delete _platform;
	_platform = nullptr;

	delete _selector;
	_selector = nullptr;

	// Uobject 해제
	_fileDownloadManager = NULL;

	UE_LOG(LogDivergenceVersion, Warning,
		TEXT("~GsDivergenceVersion()"));
}
// gvs 로드
void GsDivergenceVersion::LoadGVS()
{
	FString path =
		FPaths::ProjectContentDir() + FString("/Game/GVS");

	if (_fileDownloadManager != nullptr)
	{
		FGuid resVal =
			_fileDownloadManager->AddTaskByUrlCpp(
				FString("https://dlrybgl.gamevil.com/info/global_version_RB.gvs"),
				path,
				TEXT(""),
				false,
				[this](ETaskEvent InEvent, const FTaskInformation & InInfo)
				{
					//FString enumText = EnumToString(TEXT("ETaskEvent"), static_cast<uint8>(InEvent));

					UE_LOG(LogDivergenceVersion, Warning,
						TEXT("load gvs callback ok - state:  fileName: %s 123")
						//, *enumText
						, *InInfo.FileName);

					FString fullFilePath =
						InInfo.DestDirectory + FString("/") +
						InInfo.FileName;

					this->ParseGVS(fullFilePath);
				});

		if (resVal.IsValid() == true)
		{
			UE_LOG(LogDivergenceVersion, Warning, TEXT("load gvs proc ok 123"));
		}
	}
}

// gvs 파싱
void GsDivergenceVersion::ParseGVS(const FString& In_filePath)
{
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *In_filePath);


	TArray<FGsGVSInfo> arrGvsInfo;

	if (FJsonObjectConverter::JsonArrayStringToUStruct(JsonString, &arrGvsInfo, 0, 0))
	{
		// 테스트로 파싱한 정보 출력
		for (auto& iter : arrGvsInfo)
		{
			iter.PrintLog();
		}

		// 유효하지 않는 멤버 있다
		if (_platform == nullptr)
		{
			UE_LOG(LogDivergenceVersion, Error,
				TEXT("_platform == nullptr"));
			return;
		}

		// 플랫폼에 세팅
		_platform->SetInfo(arrGvsInfo);

		// 라이브 값이 있는건가??
		if (false == _platform->VerifyliveGvs())
		{
			UE_LOG(LogDivergenceVersion, Error,
				TEXT("Error - gvs file is wrong or missing"));
			return;
		}			
		
		// 유효하지 않음
		if (_selector == nullptr)
		{
			return;
		}
		// 함수 채워야 함
		_selector->LoadClientVersion();

		// select 채우기
		_selector->DoSelect(_platform);

		// 선택되지 않았으면
		if (false == _selector->IsSelected())
		{

		}
		FGsGVSInfo* selInfo = _selector->Selected();
		if (selInfo == nullptr)
		{
			UE_LOG(LogDivergenceVersion, Error,
				TEXT("_selector->Selected() == nullptr"));
		}
		else
		{
			UE_LOG(LogDivergenceVersion, Warning,
				TEXT("selInfo->cdn_suburl: %s, selInfo->version, selInfo->service_type: %s"),
				*selInfo->cdn_suburl,
				*(selInfo->version[0]),
				*selInfo->service_type);
		}
		
		_bInit = true;
	}
	else
	{
		UE_LOG(LogDivergenceVersion, Warning, TEXT("ParseGVS JsonArrayStringToUStruct failed"));
	}
}

DEFINE_LOG_CATEGORY(LogDivergenceVersion);