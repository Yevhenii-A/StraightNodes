// Copyright 2026 Yevhenii Amelchenko | Silent Symmetry Team. All Rights Reserved.

using UnrealBuildTool;

public class StraightNodes : ModuleRules
{
    public StraightNodes(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "UnrealEd",
            "BlueprintGraph",
            "GraphEditor",
            "EditorStyle",
            "InputCore",
            "DeveloperSettings",
        });
    }
}
