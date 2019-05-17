// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class GameService : ModuleRules
{
	public GameService(ReadOnlyTargetRules Target) : base(Target)
	{
        //http://api.unrealengine.com/KOR/Programming/UnrealBuildSystem/IWYUReferenceGuide/index.html
        //IWYU ��带 �ѷ���, ����� IWYU ��Ģ���� �����ϵǴ��� Ȯ���ϰ�, 
        //����� *.build.cs ������ ���� PCHUsage �� PCHUsageMode.UseExplicitOrSharedPCHs �� �����ϸ� �˴ϴ�.
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "UMG",
            "FileDownloader",
            "Sockets",
            "AIModule",
            "GameplayTasks",
        });

		PrivateDependencyModuleNames.AddRange(new string[] {
            "OpenSSL",
            "JsonUtilities",

        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
