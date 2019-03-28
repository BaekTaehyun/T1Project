
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

	inline TSharedPtr<class FMyEditor> Create()
	{
		//TSharedRef<FMyEditor> NewMyEditor(new FMyEditor());
		return MyEditor = TSharedRef<FMyEditor>(new FMyEditor());
		//MyEditor->InitFMyEditor(EToolkitMode::Standalone, TSharedPtr<IToolkitHost>(), NewObject<UDataActorComponent>());
		//T1ProjectEditorModule->SetMyEditor(NewMyEditor);
	}

	inline TSharedPtr<class FMyEditor> GetMyEditor()
	{
		if (MyEditor.IsValid())
		{
			return MyEditor;
		}

		return Create();
	}

	/*inline void SetMyEditor(TSharedPtr<class FMyEditor> InDataTable)
	{
		MyEditor = InDataTable;
	}*/

	/*inline void SetMyEditor(TSharedPtr<class FMyEditor> InDataTable)
	{
		MyEditor = InDataTable;
	}*/

	inline void SetDataTable(UDataTable* InDataTable)
	{
		MyEditor->SetDataTable(InDataTable);
	}

private:
	TSharedPtr<class FUICommandList> MyCommandList;	
	TSharedPtr<class FMyEditor> MyEditor;
};