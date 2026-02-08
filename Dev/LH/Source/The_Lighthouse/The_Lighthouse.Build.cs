// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class The_Lighthouse : ModuleRules
{
	public The_Lighthouse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"The_Lighthouse",
			"The_Lighthouse/Variant_Strategy",
			"The_Lighthouse/Variant_Strategy/UI",
			"The_Lighthouse/Variant_TwinStick",
			"The_Lighthouse/Variant_TwinStick/AI",
			"The_Lighthouse/Variant_TwinStick/Gameplay",
			"The_Lighthouse/Variant_TwinStick/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
