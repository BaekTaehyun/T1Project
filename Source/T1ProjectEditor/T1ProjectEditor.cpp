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
	// ������Ƽ ������ ������, ������ Ŀ���� class ����ϱ�.
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(AMyActor::StaticClass()->GetFName() , FOnGetDetailCustomizationInstance::CreateStatic(&FMyDetailsCustomization::MakeInstance));	
	PropertyModule.NotifyCustomizationModuleChanged();

	// ��Ÿ�� ���.
	FMyExtensionStyle::Initialize();
	FMyExtensionStyle::ReloadTextures();

	// Ŀ�ǵ� ���.
	FMyExtensionCommands::Register();

	// Ŀ�ǵ�� �׼��� ���� �����ֱ�.
	MyCommandList = MakeShareable(new FUICommandList());
	MyCommandList->MapAction(FMyExtensionCommands::Get().Command1, FExecuteAction::CreateStatic(&FMyExtensionActions::Action1), FCanExecuteAction());
	MyCommandList->MapAction(FMyExtensionCommands::Get().Command2, FExecuteAction::CreateStatic(&FMyExtensionActions::Action2), FCanExecuteAction());
	MyCommandList->MapAction(FMyExtensionCommands::Get().Command3, FExecuteAction::CreateStatic(&FMyExtensionActions::Action3), FCanExecuteAction());
	MyCommandList->MapAction(FMyExtensionCommands::Get().Command4, FExecuteAction::CreateStatic(&FMyExtensionActions::Action3), FCanExecuteAction());

	// �޴� ������ ���� ��������Ʈ �Լ� ����.
	struct MyCMenu
	{
		static void CreateFMyMenu(FMenuBuilder& MenuBuilder)
		{
			// ù ��° ����.
			MenuBuilder.BeginSection("MySection1", LOCTEXT("MyMenu", "My Menu Section1"));

			MenuBuilder.AddMenuEntry(FMyExtensionCommands::Get().Command1);
			MenuBuilder.AddMenuEntry(FMyExtensionCommands::Get().Command2);

			MenuBuilder.EndSection();

			// �� ��° ����.
			MenuBuilder.BeginSection("MySection2", LOCTEXT("MyMenu", "My Menu Section2"));

			{
				// ���� �޴� ������ ���� ��������Ʈ �Լ� ����.
				struct MySubMenu
				{
					static void CreateFMySubMenu(FMenuBuilder& SubMenuBuilder)
					{
						SubMenuBuilder.AddMenuEntry(FMyExtensionCommands::Get().Command3);
						SubMenuBuilder.AddMenuEntry(FMyExtensionCommands::Get().Command4);
					}
				};

				// ���� �޴� ���.
				MenuBuilder.AddSubMenu(
					LOCTEXT("MyMenu", "My SubMenu"),
					LOCTEXT("MyMenu", "My SubMenu Tooltip"),
					FNewMenuDelegate::CreateStatic(&MySubMenu::CreateFMySubMenu), false, FSlateIcon());
			}

			MenuBuilder.EndSection();
		}
	};

	// ���� ������ ������. 
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	// �޴� ����.
	TSharedPtr<FExtender> IGCMenuExtender = MakeShareable(new FExtender());
	IGCMenuExtender->AddMenuExtension("WindowLayout" /*�޴�����������ġ*/, EExtensionHook::After, MyCommandList,
		FMenuExtensionDelegate::CreateStatic(&MyCMenu::CreateFMyMenu));

	// �޴� �߰�.
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(IGCMenuExtender);


	// ���� ������ ���� ��������Ʈ �Լ� ����.
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

	// ���� ����.
	TSharedPtr<FExtender> IGCToolbarExtender = MakeShareable(new FExtender());
	IGCToolbarExtender->AddToolBarExtension("Settings" /*���ٸ���������ġ*/, EExtensionHook::After, MyCommandList,
		FToolBarExtensionDelegate::CreateStatic(&MyToolbar::CreateIGCToolbar));

	// ���� �߰�.
	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(IGCToolbarExtender);
}

void FT1ProjectEditor::ShutdownModule()
{
	// Ŀ�ǵ� ��� ����.
	FMyExtensionCommands::Unregister();

	// ��Ÿ�� ��� ����.
	FMyExtensionStyle::Shutdown();
}

#undef LOCTEXT_NAMESPACE