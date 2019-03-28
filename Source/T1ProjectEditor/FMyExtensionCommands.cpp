// Fill out your copyright notice in the Description page of Project Settings.

#include "FMyExtensionCommands.h"
#include "FMyEditor.h"
#include "DataActorComponent.h"

#include "Modules/ModuleManager.h"
#include "T1ProjectEditor.h"

#define LOCTEXT_NAMESPACE "MyCommand"

void FMyExtensionCommands::RegisterCommands()
{
	UI_COMMAND(Command1, "DataTableCenter", "DataTableCenter Tooltip", EUserInterfaceActionType::Button, FInputGesture());
}

void FMyExtensionActions::Action1()
{
	FT1ProjectEditor* T1ProjectEditorModule = FT1ProjectEditor::GetPtr();
	T1ProjectEditorModule->Create();
	T1ProjectEditorModule->GetMyEditor()->InitFMyEditor(EToolkitMode::Standalone, TSharedPtr<IToolkitHost>(), NewObject<UDataActorComponent>());
	/*TSharedRef<FMyEditor> NewMyEditor(new FMyEditor());
	NewMyEditor->InitFMyEditor(EToolkitMode::Standalone, TSharedPtr<IToolkitHost>(), NewObject<UDataActorComponent>());
	T1ProjectEditorModule->SetMyEditor(NewMyEditor);*/
}

#undef LOCTEXT_NAMESPACE