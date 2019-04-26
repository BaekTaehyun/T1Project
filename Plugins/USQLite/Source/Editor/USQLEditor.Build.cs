//  Copyright 2018 (C) Bruno Xavier B. Leite

using UnrealBuildTool;
using System.IO;

namespace UnrealBuildTool.Rules {

public class USQLEditor : ModuleRules {
    public USQLEditor(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PrivatePCHHeaderFile = "Public/USQLEditor.h";
		bEnforceIWYU = true;
		//
        PublicDependencyModuleNames.AddRange(new string[] {"Core","Engine","CoreUObject","USQLite"});
        //
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Slate",
                "Projects",
                "UnrealEd",
                "SlateCore",
                "InputCore",
                "AssetTools",
                "EditorStyle",
                "LevelEditor",
				"GraphEditor",
                "PropertyEditor",
				"BlueprintGraph",
            }
        );
        //
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory,"Private"));
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory,"Public"));
    }
}

}