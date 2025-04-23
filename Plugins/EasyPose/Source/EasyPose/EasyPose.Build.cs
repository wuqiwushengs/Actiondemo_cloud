// Copyright longlt00502@gmail.com 2023. All rights reserved.
using UnrealBuildTool;
public class EasyPose : ModuleRules
{
	public EasyPose(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateIncludePaths.AddRange(
			new string[] {
				System.IO.Path.Combine(GetModuleDirectory("Persona"), "Private"),
			}
			);
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"EditorSubsystem",
				"DeveloperSettings",
				"EditorFramework",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"Persona",
				"Core",
				"ToolWidgets",
				"MessageLog",
				"IKRig",
				"IKRigEditor",
			}
			);
	}
}
