// Fill out your copyright notice in the Description page of Project Settings.

#include "FMyExtensionCommands.h"
#include "FMyEditor.h"
#include "MyUI.h"

#define LOCTEXT_NAMESPACE "MyCommand"

void FMyExtensionCommands::RegisterCommands()
{
	UI_COMMAND(Command1, "MyCommand1", "My Command1 Tooltip", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(Command2, "MyCommand2", "My Command2 Tooltip", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(Command3, "MyCommand3", "My Command3 Tooltip", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(Command4, "MyCommand4", "My Command4 Tooltip", EUserInterfaceActionType::Button, FInputGesture());
}

void FMyExtensionActions::Action1()
{
	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("MyCommand", "My Editor Extension Action 1 Message"));
}

void FMyExtensionActions::Action2()
{
	FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("MyCommand", "My Editor Extension Action 2 Message"));
}

void FMyExtensionActions::Action3()
{
	//FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("MyCommand", "My Editor Extension Action 3 Message"));
	TSharedRef< FMyEditor > NewMyEditor(new FMyEditor());
	NewMyEditor->InitFMyEditor(EToolkitMode::Standalone, TSharedPtr<IToolkitHost>(), NewObject<UMyUI>());
}

#undef LOCTEXT_NAMESPACE