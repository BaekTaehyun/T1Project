// Fill out your copyright notice in the Description page of Project Settings.

#include "FileDownloadManager.h"
#include "DownloadTask.h"
#include "Paths.h"

void UFileDownloadManager::Tick(float DeltaTime)
{
	if (bStopAll)
	{
		return;
	}
	static float TimeCount = 0.f;
	TimeCount += DeltaTime;
	if (TimeCount >= TickInterval)
	{
		TimeCount = 0.f;
		//broadcast event

		//find task to do
		if (CurrentDoingWorks < MaxParallelTask && TaskList.Num())
		{
			int32 Idx = FindTaskToDo();
			if (Idx > INDEX_NONE)
			{
				TaskList[Idx]->Start();
				++CurrentDoingWorks;
			}
		}
	}
}

TStatId UFileDownloadManager::GetStatId() const
{
	return TStatId();
}

UFileDownloadManager::UFileDownloadManager()
{

}

void UFileDownloadManager::StartAll()
{
	bStopAll = false;

	for (int32 i =0; i < TaskList.Num(); ++i)
	{
		TaskList[i]->SetNeedStop(false);
	}
}

void UFileDownloadManager::StartTask(const FGuid& InGuid)
{
	int32 ret = FindTaskByGuid(InGuid);
	if (ret > INDEX_NONE)
	{
		TaskList[ret]->SetNeedStop(false);
	}
}

void UFileDownloadManager::StopAll()
{
	for (int32 i = 0; i < TaskList.Num(); ++i)
	{
		TaskList[i]->Stop();
	}

	bStopAll = true;
	CurrentDoingWorks = 0;
}

void UFileDownloadManager::StopTask(const FGuid& InGuid)
{
	int32 ret = FindTaskByGuid(InGuid);
	if (ret >= 0)
	{
		if (TaskList[ret]->GetState() == ETaskState::DOWNLOADING)
		{
			TaskList[ret]->Stop();
			--CurrentDoingWorks;
		}
	}
}

int32 UFileDownloadManager::GetTotalPercent() const
{
	int32 CurrentSize = 0;
	int32 TotalSize = 0;

	for (int32 i = 0; i < TaskList.Num(); ++i)
	{
		CurrentSize += TaskList[i]->GetCurrentSize();
		TotalSize += TaskList[i]->GetTotalSize();
	}

	if (TotalSize < 1)
	{
		return 0;
	}

	return (float)(CurrentSize) / TotalSize * 100.f;
}

void UFileDownloadManager::Clear()
{
	StopAll();
	TaskList.Reset();
	ErrorCount = 0;
}

bool UFileDownloadManager::SaveTaskToJsonFile(const FGuid& InGuid, const FString& InFileName /*= TEXT("")*/)
{
	int32 ret = FindTaskByGuid(InGuid);
	if (ret < 0)
	{
		return false;
	}

	return TaskList[ret]->SaveTaskToJsonFile(InFileName);
}

TArray<FTaskInformation> UFileDownloadManager::GetAllTaskInformation() const
{
	TArray<FTaskInformation> Ret;
	for (int32 i = 0; i < TaskList.Num(); ++i)
	{
		Ret.Add(TaskList[i]->GetTaskInformation());
	}

	return Ret;
}

FGuid UFileDownloadManager::AddTaskByUrl(const FString& InUrl, const FString& InDirectory, const FString& InFileName, bool InOverride)
{
	FString TmpDir = InDirectory;
	if (TmpDir.IsEmpty())
	{
		//https://www.google.com/
		static int32 URLTag = 8;
		int32 StartSlash = InUrl.Find(FString("/"), ESearchCase::IgnoreCase, ESearchDir::FromStart, URLTag);
		int32 LastSlash = InUrl.Find(FString("/"), ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		FString UrlDirectory = InUrl.Mid(StartSlash, LastSlash - StartSlash);

		TmpDir = FPaths::ProjectDir() + UrlDirectory;
	}
	TSharedPtr<DownloadTask>Task = MakeShareable(new DownloadTask(InUrl, TmpDir, InFileName, InOverride));

	if (Task.IsValid() == false)
	{
		FGuid ret;
		ret.Invalidate();
		return ret;
	}

	for (int32 i = 0; i < TaskList.Num(); ++i)
	{
		if (TaskList[i]->GetSourceUrl() == Task->GetSourceUrl())
		{
			//��������������б�
			return TaskList[i]->GetGuid();
		}
	}

	Task->ProcessTaskEvent = [this](ETaskEvent InEvent, const FTaskInformation& InInfo)
	{
		if (this != nullptr)
		{
			this->OnTaskEvent(InEvent, InInfo);
		}
	};

	TaskList.Add(Task);
	return Task->GetGuid();
}

void UFileDownloadManager::OnTaskEvent(ETaskEvent InEvent, const FTaskInformation& InInfo)
{
	OnDlManagerEvent.Broadcast(InEvent, InInfo);
	if (InEvent >= ETaskEvent::DOWNLOAD_COMPLETED)
	{
		if (CurrentDoingWorks > 0)
		{
			--CurrentDoingWorks;
		}

		if (InEvent == ETaskEvent::ERROR_OCCUR)
		{
			++ErrorCount;
		}

		/*int32 Idx = FindTaskByGuid(InInfo.GetGuid());
		if (Idx > INDEX_NONE)
		{
			TaskList.RemoveAt(Idx);
		}*/

		if (CurrentDoingWorks < 1)
		{
			OnAllTaskCompleted.Broadcast(ErrorCount);
			ErrorCount = 0;
		}
	}
	return ;
}

FString UFileDownloadManager::GetDownloadDirectory() const
{
	return DefaultDirectory;
}

int32 UFileDownloadManager::FindTaskToDo() const
{
	int32 ret = INDEX_NONE;
	for (int32 i = 0; i < TaskList.Num(); ++i)
	{
		if (TaskList[i]->GetState() == ETaskState::WAIT && TaskList[i]->GetNeedStop() == false)
		{
			ret = i;
		}
	}

	return ret;
}

int32 UFileDownloadManager::FindTaskByGuid(const FGuid& InGuid) const
{
	int32 ret = INDEX_NONE;
	for (int32 i = 0; i < TaskList.Num(); ++i)
	{
		if (TaskList[i]->GetGuid() == InGuid)
		{
			ret = i;
		}
	}

	return ret;
}

