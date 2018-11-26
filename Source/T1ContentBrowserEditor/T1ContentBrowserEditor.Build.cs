// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class T1ContentBrowserEditor : ModuleRules
{
	public T1ContentBrowserEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		//PrivateIncludePathModuleNames.AddRange(
		//	new string[] {
		//		"T1ContentBrowserEditor",
		//	}
		//);

		PrivateIncludePathModuleNames.AddRange(
			new string[] {
				"AssetRegistry",
				"AssetTools",
				"CollectionManager",
				"EditorWidgets",
				"GameProjectGeneration",
                "MainFrame",
				"PackagesDialog",
				"SourceControl",
				"SourceControlWindows"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
                "AppFramework",
				"Core",
				"CoreUObject",
				"ApplicationCore",
                "InputCore",
				"Engine",
				"Slate",
				"SlateCore",
                "EditorStyle",
				"SourceControl",
				"SourceControlWindows",
				"WorkspaceMenuStructure",
				"UnrealEd",
				"EditorWidgets",
				"Projects",
				"AddContentDialog",
				"DesktopPlatform",				
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[] {
				"PropertyEditor",
				"PackagesDialog",
				"AssetRegistry",
				"AssetTools",
				"CollectionManager",
				"GameProjectGeneration",
                "MainFrame"
			}
		);
		
		//PublicIncludePathModuleNames.AddRange(
  //          new string[] {                
  //              //"IntroTutorials"
		//		"T1ProjectEditor",
  //          }
  //      );
	}
}
