// Fill out your copyright notice in the Description page of Project Settings.

#include "T1ProjectEditor.h"

#include "MyActor.h"
#include "FMyDetailsCustomization.h"
#include "FMyExtensionCommands.h"
#include "FMyExtensionStyle.h"

#include "../PropertyEditor/Public/PropertyEditorModule.h"

#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "LevelEditor.h"

IMPLEMENT_MODULE(FT1ProjectEditor, "T1ProjectEditor");

#define LOCTEXT_NAMESPACE "MyMenu"

void FT1ProjectEditor::StartupModule()
{	
	// 프로퍼티 에디터 얻어오기, 연결할 커스텀 class 등록하기.
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(AMyActor::StaticClass()->GetFName() , FOnGetDetailCustomizationInstance::CreateStatic(&FMyDetailsCustomization::MakeInstance));	
	PropertyModule.NotifyCustomizationModuleChanged();

	// 스타일 등록.
	FMyExtensionStyle::Initialize();
	FMyExtensionStyle::ReloadTextures();

	// 커맨드 등록.
	FMyExtensionCommands::Register();

	// 커맨드와 액션을 서로 묶어주기.
	MyCommandList = MakeShareable(new FUICommandList());
	MyCommandList->MapAction(FMyExtensionCommands::Get().Command1, FExecuteAction::CreateStatic(&FMyExtensionActions::Action1), FCanExecuteAction());
	MyCommandList->MapAction(FMyExtensionCommands::Get().Command2, FExecuteAction::CreateStatic(&FMyExtensionActions::Action2), FCanExecuteAction());
	MyCommandList->MapAction(FMyExtensionCommands::Get().Command3, FExecuteAction::CreateStatic(&FMyExtensionActions::Action3), FCanExecuteAction());
	MyCommandList->MapAction(FMyExtensionCommands::Get().Command4, FExecuteAction::CreateStatic(&FMyExtensionActions::Action3), FCanExecuteAction());

	// 메뉴 생성을 위한 델리게이트 함수 선언.
	struct MyCMenu
	{
		static void CreateFMyMenu(FMenuBuilder& MenuBuilder)
		{
			// 첫 번째 섹션.
			MenuBuilder.BeginSection("MySection1", LOCTEXT("MyMenu", "My Menu Section1"));

			MenuBuilder.AddMenuEntry(FMyExtensionCommands::Get().Command1);
			MenuBuilder.AddMenuEntry(FMyExtensionCommands::Get().Command2);

			MenuBuilder.EndSection();

			// 두 번째 섹션.
			MenuBuilder.BeginSection("MySection2", LOCTEXT("MyMenu", "My Menu Section2"));

			{
				// 서브 메뉴 생성을 위한 델리게이트 함수 선언.
				struct MySubMenu
				{
					static void CreateFMySubMenu(FMenuBuilder& SubMenuBuilder)
					{
						SubMenuBuilder.AddMenuEntry(FMyExtensionCommands::Get().Command3);
						SubMenuBuilder.AddMenuEntry(FMyExtensionCommands::Get().Command4);
					}
				};

				// 서브 메뉴 등록.
				MenuBuilder.AddSubMenu(
					LOCTEXT("MyMenu", "My SubMenu"),
					LOCTEXT("MyMenu", "My SubMenu Tooltip"),
					FNewMenuDelegate::CreateStatic(&MySubMenu::CreateFMySubMenu), false, FSlateIcon());
			}

			MenuBuilder.EndSection();
		}
	};

	// 레벨 에디터 얻어오기. 
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	// 메뉴 생성.
	TSharedPtr<FExtender> IGCMenuExtender = MakeShareable(new FExtender());
	IGCMenuExtender->AddMenuExtension("WindowLayout" /*메뉴를삽입할위치*/, EExtensionHook::After, MyCommandList,
		FMenuExtensionDelegate::CreateStatic(&MyCMenu::CreateFMyMenu));

	// 메뉴 추가.
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(IGCMenuExtender);


	// 툴바 생성을 위한 델리게이트 함수 선언.
	struct MyToolbar
	{
		static void CreateIGCToolbar(FToolBarBuilder& ToolbarBuilder)
		{
			ToolbarBuilder.BeginSection("MyToolbar");
			{
				ToolbarBuilder.AddToolBarButton(FMyExtensionCommands::Get().Command1,
					NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FMyExtensionStyle::GetStyleSetName(), "MyToolbarIcon.Command1"), NAME_None);
				ToolbarBuilder.AddToolBarButton(FMyExtensionCommands::Get().Command2,
					NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FMyExtensionStyle::GetStyleSetName(), "MyToolbarIcon.Command2"), NAME_None);
				ToolbarBuilder.AddToolBarButton(FMyExtensionCommands::Get().Command3,
					NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FMyExtensionStyle::GetStyleSetName(), "MyToolbarIcon.Command3"), NAME_None);
				ToolbarBuilder.AddToolBarButton(FMyExtensionCommands::Get().Command4,
					NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FMyExtensionStyle::GetStyleSetName(), "MyToolbarIcon.Command4"), NAME_None);
			}
			ToolbarBuilder.EndSection();
		}
	};

	// 툴바 생성.
	TSharedPtr<FExtender> IGCToolbarExtender = MakeShareable(new FExtender());
	IGCToolbarExtender->AddToolBarExtension("Settings" /*툴바를삽입할위치*/, EExtensionHook::After, MyCommandList,
		FToolBarExtensionDelegate::CreateStatic(&MyToolbar::CreateIGCToolbar));

	// 툴바 추가.
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(IGCToolbarExtender);
}

void FT1ProjectEditor::ShutdownModule()
{
	// 커맨드 등록 해제.
	FMyExtensionCommands::Unregister();

	// 스타일 등록 해제.
	FMyExtensionStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE