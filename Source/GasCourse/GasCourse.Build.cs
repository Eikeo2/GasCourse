// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GasCourse : ModuleRules
{
	public GasCourse(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", 
			"CoreUObject",
			"Engine",
			"InputCore", 
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"UMG"
			
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

	}
}
