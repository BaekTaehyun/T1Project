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

	// �÷��� ����
	_platform = MakeShareable(new GsGVSPlatform());
	// ���� ����
	_selector = MakeShareable(new GsGVSSelector());

	UE_LOG(LogDivergenceVersion, Warning,
		TEXT("DivergenceVersion construct 123"));
}

GsDivergenceVersion::~GsDivergenceVersion()
{
}
// gvs �ε�
void GsDivergenceVersion::LoadGVS()
{
	FString path =
		FPaths::GameContentDir() + FString("/Game/GVS");

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

// gvs �Ľ�
void GsDivergenceVersion::ParseGVS(const FString& In_filePath)
{
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *In_filePath);


	TArray<FGsGVSInfo> arrGvsInfo;

	if (FJsonObjectConverter::JsonArrayStringToUStruct(JsonString, &arrGvsInfo, 0, 0))
	{
		// �׽�Ʈ�� �Ľ��� ���� ���
		for (auto& iter : arrGvsInfo)
		{
			iter.PrintLog();
		}

		// ��ȿ���� �ʴ� ��� �ִ�
		if (_platform.IsValid() == false)
		{
			UE_LOG(LogDivergenceVersion, Error,
				TEXT("_platform.IsValid() == false"));
			return;
		}

		// �÷����� ����
		_platform->SetInfo(arrGvsInfo);

		// ���̺� ���� �ִ°ǰ�??
		if (false == _platform->VerifyliveGvs())
		{
			UE_LOG(LogDivergenceVersion, Error,
				TEXT("Error - gvs file is wrong or missing"));
			return;
		}			
		
		// ��ȿ���� ����
		if (_selector.IsValid() == false)
		{
			return;
		}
		// �Լ� ä���� ��
		_selector->LoadClientVersion();

		// select ä���
		_selector->DoSelect(_platform.Get());

		// ���õ��� �ʾ�����
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