// Fill out your copyright notice in the Description page of Project Settings.


#include "GsGVSPlatform.h"
#include "Kismet/GameplayStatics.h"

// �α� ���
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
// ���� �÷����� ������?
bool FGsGVSInfo::IsCurrentPlatform()
{
	// ���� ����� �÷��� �̸� ��������
	FString findVal = UGameplayStatics::GetPlatformName();
	
	// �ȵ��
	if (findVal.Compare(TEXT("Android")) == 0)
	{
		// ������ �ؽ�Ʈ�� ��
		if (platform.Compare(TEXT("android")) == 0)
		{
			return true;
		}
	}
	// ������ ios��
	else
	{
		// ������ �ؽ�Ʈ�� ��
		if (platform.Compare(TEXT("ios")) == 0)
		{
			return true;
		}
	}

	return false;
}

// ���� Ÿ���� ��������?
bool FGsGVSInfo::IsSameServiceType(const FString& In_str)
{
	// ������ true, �ٸ��� false
	return (service_type.Compare(In_str) == 0) ? true : false;
}


// ���� ���� ������ ������ �ִ��� �˻�
bool FGsGVSInfo::IsIncludeVersion(const FString& In_versetText)
{
	for (auto& iter:version)
	{
		// ������?
		if (iter.Compare(In_versetText) == 0)
		{
			return true;
		}
	}

	// �ش� ������ ����
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
	// �ʱ�ȭ
	// struct �޸� �ʱ�ȭ�� ����??
	_mapTypeGVS.Reset();

	// ��� gvs���� 
	for (auto& iter:In_arrInfo)
	{
		// ���� �÷����� ���ٸ�
		if (iter.IsCurrentPlatform() == true)
		{
			for (auto& typeIter: _mapTypeStr)
			{
				// ���� Ÿ���� ������
				if (iter.IsSameServiceType(typeIter.Value) == true)
				{
					// �߰�
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
// ������ �Ǵ� ���̺� gvs ������ �����ϴ��� Ȯ�� 
bool GsGVSPlatform::VerifyliveGvs()
{
	// Ű���� �ִ���?
	bool isExist = _mapTypeGVS.Contains(EGsServiceType::live);

	if (isExist == false)
	{
		return false;
	}
	// ���� nullptr�� �ƴ���??
	return (_mapTypeGVS.Find(EGsServiceType::live) != nullptr);
}

// ������
FGsGVSInfo* GsGVSPlatform::GetTypeGVS(EGsServiceType In_type)
{
	return _mapTypeGVS.Find(In_type);
}

DEFINE_LOG_CATEGORY(LogGVSInfo);