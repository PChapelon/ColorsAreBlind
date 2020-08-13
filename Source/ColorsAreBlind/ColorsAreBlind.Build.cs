// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ColorsAreBlind : ModuleRules
{
	public ColorsAreBlind(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "AIModule", "ProceduralMeshComponent", "UMG", "Slate", "SlateCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });



		PublicDependencyModuleNames.AddRange(new string[] {
			 "OnlineSubsystem",
			 "Landscape"
		 });

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
