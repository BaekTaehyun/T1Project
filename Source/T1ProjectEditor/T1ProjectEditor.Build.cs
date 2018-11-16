// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class T1ProjectEditor : ModuleRules
{
	public T1ProjectEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{				
				"Core",
				"CoreUObject",				
				"SlateCore",
				"Slate",
				"UnrealEd",
				"LevelEditor",
				"T1Project",
				"InputCore",
				"Engine",
                "AdvancedPreviewScene",
                "EditorStyle",
				"PropertyEditor",
				"Json"
			}
		);

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}