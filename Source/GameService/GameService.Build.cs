// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class GameService : ModuleRules
{
	public GameService(ReadOnlyTargetRules Target) : base(Target)
	{
        //http://api.unrealengine.com/KOR/Programming/UnrealBuildSystem/IWYUReferenceGuide/index.html
        //IWYU 모드를 켜려면, 모듈이 IWYU 규칙으로 컴파일되는지 확인하고, 
        //모듈의 *.build.cs 파일을 열어 PCHUsage 를 PCHUsageMode.UseExplicitOrSharedPCHs 로 설정하면 됩니다.
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
