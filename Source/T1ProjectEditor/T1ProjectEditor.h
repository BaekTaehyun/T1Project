
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FMyEditor.h"

class FT1ProjectEditor : public IModuleInterface
{
public:	

	static inline FT1ProjectEditor* GetPtr()
	{
		auto Module = FModuleManager::GetModulePtr<FT1ProjectEditor>("T1ProjectEditor");
		if (Module == nullptr)
		{
			Module = &FModuleManager::LoadModuleChecked<FT1ProjectEditor>("T1ProjectEditor");
		}
		return reinterpret_cast<FT1ProjectEditor*>(Module);
	}

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	inline void SetMyEditor(TSharedPtr<class FMyEditor> InDataTable)
	{
		MyEditor = InDataTable;
	}

	inline void SetDataTable(UDataTable* InDataTable)
	{
		MyEditor->SetDataTable(InDataTable);
	}

private:
	TSharedPtr<class FUICommandList> MyCommandList;	
	TSharedPtr<class FMyEditor> MyEditor;
};