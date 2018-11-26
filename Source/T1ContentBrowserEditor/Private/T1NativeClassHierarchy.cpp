// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "T1NativeClassHierarchy.h"
#include "Modules/ModuleManager.h"
#include "UObject/Class.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"
#include "UObject/Package.h"
#include "Misc/PackageName.h"
#include "T1ContentBrowserLog.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "GameProjectGenerationModule.h"
#include "Misc/HotReloadInterface.h"
#include "SourceCodeNavigation.h"
#include "Interfaces/IPluginManager.h"

TSharedRef<FT1NativeClassHierarchyNode> FT1NativeClassHierarchyNode::MakeFolderEntry(FName InEntryName, FString InEntryPath)
{
	TSharedRef<FT1NativeClassHierarchyNode> NewEntry = MakeShareable(new FT1NativeClassHierarchyNode());
	NewEntry->Type = ET1NativeClassHierarchyNodeType::Folder;
	NewEntry->Class = nullptr;
	NewEntry->EntryName = InEntryName;
	NewEntry->EntryPath = MoveTemp(InEntryPath);
	return NewEntry;
}

TSharedRef<FT1NativeClassHierarchyNode> FT1NativeClassHierarchyNode::MakeClassEntry(UClass* InClass, FName InClassModuleName, FString InClassModuleRelativePath, FString InEntryPath)
{
	TSharedRef<FT1NativeClassHierarchyNode> NewEntry = MakeShareable(new FT1NativeClassHierarchyNode());
	NewEntry->Type = ET1NativeClassHierarchyNodeType::Class;
	NewEntry->Class = InClass;
	NewEntry->ClassModuleName = MoveTemp(InClassModuleName);
	NewEntry->ClassModuleRelativePath = MoveTemp(InClassModuleRelativePath);
	NewEntry->EntryName = InClass->GetFName();
	NewEntry->EntryPath = MoveTemp(InEntryPath);
	return NewEntry;
}

void FT1NativeClassHierarchyNode::AddChild(TSharedRef<FT1NativeClassHierarchyNode> ChildEntry)
{
	check(Type == ET1NativeClassHierarchyNodeType::Folder);
	Children.Add(FT1NativeClassHierarchyNodeKey(ChildEntry->EntryName, ChildEntry->Type), MoveTemp(ChildEntry));
}

FT1NativeClassHierarchy::FT1NativeClassHierarchy()
{
	PopulateHierarchy();

	// Register to be notified of module changes
	FModuleManager::Get().OnModulesChanged().AddRaw(this, &FT1NativeClassHierarchy::OnModulesChanged);

	// Register to be notified of hot reloads
	IHotReloadInterface& HotReloadSupport = FModuleManager::LoadModuleChecked<IHotReloadInterface>("HotReload");
	HotReloadSupport.OnHotReload().AddRaw(this, &FT1NativeClassHierarchy::OnHotReload);
}

FT1NativeClassHierarchy::~FT1NativeClassHierarchy()
{
	FModuleManager::Get().OnModulesChanged().RemoveAll(this);

	if(FModuleManager::Get().IsModuleLoaded("HotReload"))
	{
		IHotReloadInterface& HotReloadSupport = FModuleManager::GetModuleChecked<IHotReloadInterface>("HotReload");
		HotReloadSupport.OnHotReload().RemoveAll(this);
	}
}

bool FT1NativeClassHierarchy::HasClasses(const FName InClassPath, const bool bRecursive) const
{
	TArray<TSharedRef<FT1NativeClassHierarchyNode>, TInlineAllocator<4>> NodesToSearch;
	GatherMatchingNodesForPaths(TArrayView<const FName>(&InClassPath, 1), NodesToSearch);

	for(const auto& NodeToSearch : NodesToSearch)
	{
		if(HasClassesRecursive(NodeToSearch, bRecursive))
		{
			return true;
		}
	}

	return false;
}

bool FT1NativeClassHierarchy::HasFolders(const FName InClassPath, const bool bRecursive) const
{
	TArray<TSharedRef<FT1NativeClassHierarchyNode>, TInlineAllocator<4>> NodesToSearch;
	GatherMatchingNodesForPaths(TArrayView<const FName>(&InClassPath, 1), NodesToSearch);

	for(const auto& NodeToSearch : NodesToSearch)
	{
		if(HasFoldersRecursive(NodeToSearch, bRecursive))
		{
			return true;
		}
	}

	return false;
}

void FT1NativeClassHierarchy::GetMatchingClasses(const FT1NativeClassHierarchyFilter& Filter, TArray<UClass*>& OutClasses) const
{
	TArray<TSharedRef<FT1NativeClassHierarchyNode>, TInlineAllocator<4>> NodesToSearch;
	GatherMatchingNodesForPaths(Filter.ClassPaths, NodesToSearch);

	for(const auto& NodeToSearch : NodesToSearch)
	{
		GetClassesRecursive(NodeToSearch, OutClasses, Filter.bRecursivePaths);
	}
}

void FT1NativeClassHierarchy::GetMatchingFolders(const FT1NativeClassHierarchyFilter& Filter, TArray<FString>& OutFolders) const
{
	TArray<TSharedRef<FT1NativeClassHierarchyNode>, TInlineAllocator<4>> NodesToSearch;
	GatherMatchingNodesForPaths(Filter.ClassPaths, NodesToSearch);

	for(const auto& NodeToSearch : NodesToSearch)
	{
		GetFoldersRecursive(NodeToSearch, OutFolders, Filter.bRecursivePaths);
	}
}

void FT1NativeClassHierarchy::GetClassFolders(TArray<FName>& OutClassRoots, TArray<FString>& OutClassFolders, const bool bIncludeEngineClasses, const bool bIncludePluginClasses) const
{
	static const FName EngineRootNodeName = "Classes_Engine";
	static const FName GameRootNodeName = "Classes_Game";

	for(const auto& RootNode : RootNodes)
	{
		bool bRootNodePassesFilter =
			(RootNode.Key == GameRootNodeName) ||								// Always include game classes
			(RootNode.Key == EngineRootNodeName && bIncludeEngineClasses) ||  	// Only include engine classes if we were asked for them
			(bIncludePluginClasses && RootNode.Value->LoadedFrom == EPluginLoadedFrom::Project) || // Only include Game plugin classes if we were asked for them
			(bIncludePluginClasses && bIncludeEngineClasses && RootNode.Value->LoadedFrom == EPluginLoadedFrom::Engine); //  Only include engine plugin classes if we were asked for them

		if(bRootNodePassesFilter)
		{
			OutClassRoots.Add(RootNode.Key);
			GetFoldersRecursive(RootNode.Value.ToSharedRef(), OutClassFolders);
		}
	}
}

bool FT1NativeClassHierarchy::HasClassesRecursive(const TSharedRef<FT1NativeClassHierarchyNode>& HierarchyNode, const bool bRecurse)
{
	for(const auto& ChildNode : HierarchyNode->Children)
	{
		if(ChildNode.Value->Type == ET1NativeClassHierarchyNodeType::Class)
		{
			return true;
		}

		if(bRecurse && HasClassesRecursive(ChildNode.Value.ToSharedRef()))
		{
			return true;
		}
	}

	return false;
}

bool FT1NativeClassHierarchy::HasFoldersRecursive(const TSharedRef<FT1NativeClassHierarchyNode>& HierarchyNode, const bool bRecurse)
{
	for(const auto& ChildNode : HierarchyNode->Children)
	{
		if(ChildNode.Value->Type == ET1NativeClassHierarchyNodeType::Folder)
		{
			return true;
		}

		if(bRecurse && HasFoldersRecursive(ChildNode.Value.ToSharedRef()))
		{
			return true;
		}
	}

	return false;
}

void FT1NativeClassHierarchy::GetClassesRecursive(const TSharedRef<FT1NativeClassHierarchyNode>& HierarchyNode, TArray<UClass*>& OutClasses, const bool bRecurse)
{
	for(const auto& ChildNode : HierarchyNode->Children)
	{
		if(ChildNode.Value->Type == ET1NativeClassHierarchyNodeType::Class)
		{
			OutClasses.Add(ChildNode.Value->Class);
		}

		if(bRecurse)
		{
			GetClassesRecursive(ChildNode.Value.ToSharedRef(), OutClasses);
		}
	}
}

void FT1NativeClassHierarchy::GetFoldersRecursive(const TSharedRef<FT1NativeClassHierarchyNode>& HierarchyNode, TArray<FString>& OutFolders, const bool bRecurse)
{
	for(const auto& ChildNode : HierarchyNode->Children)
	{
		if(ChildNode.Value->Type == ET1NativeClassHierarchyNodeType::Folder)
		{
			OutFolders.Add(ChildNode.Value->EntryPath);
		}

		if(bRecurse)
		{
			GetFoldersRecursive(ChildNode.Value.ToSharedRef(), OutFolders);
		}
	}
}

void FT1NativeClassHierarchy::GatherMatchingNodesForPaths(const TArrayView<const FName>& InClassPaths, TArray<TSharedRef<FT1NativeClassHierarchyNode>, TInlineAllocator<4>>& OutMatchingNodes) const
{
	if(InClassPaths.Num() == 0)
	{
		// No paths means search all roots
		OutMatchingNodes.Reserve(RootNodes.Num());
		for(const auto& RootNode : RootNodes)
		{
			OutMatchingNodes.Add(RootNode.Value.ToSharedRef());
		}
	}
	else
	{
		for(const FName& ClassPath : InClassPaths)
		{
			TSharedPtr<FT1NativeClassHierarchyNode> CurrentNode;

			TArray<FString> ClassPathParts;
			ClassPath.ToString().ParseIntoArray(ClassPathParts, TEXT("/"), true);
			for(const FString& ClassPathPart : ClassPathParts)
			{
				// Try and find the node associated with this part of the path...
				const FName ClassPathPartName = *ClassPathPart;
				CurrentNode = (CurrentNode.IsValid()) ? CurrentNode->Children.FindRef(FT1NativeClassHierarchyNodeKey(ClassPathPartName, ET1NativeClassHierarchyNodeType::Folder)) : RootNodes.FindRef(ClassPathPartName);

				// ... bail out if we didn't find a valid node
				if(!CurrentNode.IsValid())
				{
					break;
				}
			}

			if(CurrentNode.IsValid())
			{
				OutMatchingNodes.Add(CurrentNode.ToSharedRef());
			}
		}
	}
}

void FT1NativeClassHierarchy::PopulateHierarchy()
{
	FAddClassMetrics AddClassMetrics;

	RootNodes.Empty();

	TSet<FName> GameModules = GetGameModules();
	TMap<FName, FT1NativeClassHierarchyPluginModuleInfo> PluginModules = GetPluginModules();

	for(TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		UClass* const CurrentClass = *ClassIt;
		AddClass(CurrentClass, GameModules, PluginModules, AddClassMetrics);
	}

	UE_LOG(LogContentBrowser, Log, TEXT("Native class hierarchy populated in %0.4f seconds. Added %d classes and %d folders."), FPlatformTime::Seconds() - AddClassMetrics.StartTime, AddClassMetrics.NumClassesAdded, AddClassMetrics.NumFoldersAdded);

	ClassHierarchyUpdatedDelegate.Broadcast();
}

void FT1NativeClassHierarchy::AddClassesForModule(const FName& InModuleName)
{
	FAddClassMetrics AddClassMetrics;

	// Find the class package for this module
	UPackage* const ClassPackage = FindPackage(nullptr, *(FString("/Script/") + InModuleName.ToString()));
	if(!ClassPackage)
	{
		return;
	}

	TSet<FName> GameModules = GetGameModules();
	TMap<FName, FT1NativeClassHierarchyPluginModuleInfo> PluginModules = GetPluginModules();

	TArray<UObject*> PackageObjects;
	GetObjectsWithOuter(ClassPackage, PackageObjects, false);
	for(UObject* Object : PackageObjects)
	{
		UClass* const CurrentClass = Cast<UClass>(Object);
		if(CurrentClass)
		{
			AddClass(CurrentClass, GameModules, PluginModules, AddClassMetrics);
		}
	}

	UE_LOG(LogContentBrowser, Log, TEXT("Native class hierarchy updated for '%s' in %0.4f seconds. Added %d classes and %d folders."), *InModuleName.ToString(), FPlatformTime::Seconds() - AddClassMetrics.StartTime, AddClassMetrics.NumClassesAdded, AddClassMetrics.NumFoldersAdded);

	ClassHierarchyUpdatedDelegate.Broadcast();
}

void FT1NativeClassHierarchy::RemoveClassesForModule(const FName& InModuleName)
{
	// Modules always exist directly under a root
	for(const auto& RootNode : RootNodes)
	{
		TSharedPtr<FT1NativeClassHierarchyNode> ModuleNode = RootNode.Value->Children.FindRef(FT1NativeClassHierarchyNodeKey(InModuleName, ET1NativeClassHierarchyNodeType::Folder));
		if(ModuleNode.IsValid())
		{
			// Remove this module from its root
			RootNode.Value->Children.Remove(FT1NativeClassHierarchyNodeKey(InModuleName, ET1NativeClassHierarchyNodeType::Folder));

			// If this module was the only child of this root, then we need to remove the root as well
			if(RootNode.Value->Children.Num() == 0)
			{
				RootNodes.Remove(RootNode.Key);
			}

			ClassHierarchyUpdatedDelegate.Broadcast();

			// We've found the module - break
			break;
		}
	}
}

void FT1NativeClassHierarchy::AddClass(UClass* InClass, const TSet<FName>& InGameModules, const TMap<FName, FT1NativeClassHierarchyPluginModuleInfo>& InPluginModules, FAddClassMetrics& AddClassMetrics)
{
	// Ignore deprecated and temporary classes
	if(InClass->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists) || FKismetEditorUtilities::IsClassABlueprintSkeleton(InClass))
	{
		return;
	}

	const FName ClassModuleName = GetClassModuleName(InClass);
	if(ClassModuleName.IsNone())
	{
		return;
	}

	static const FName ModuleRelativePathMetaDataKey = "ModuleRelativePath";
	const FString& ClassModuleRelativeIncludePath = InClass->GetMetaData(ModuleRelativePathMetaDataKey);
	if(ClassModuleRelativeIncludePath.IsEmpty())
	{
		return;
	}

	// Work out which root this class should go under
	EPluginLoadedFrom WhereLoadedFrom;
	const FName RootNodeName = GetClassPathRootForModule(ClassModuleName, InGameModules, InPluginModules, WhereLoadedFrom);

	// Work out the final path to this class within the hierarchy (which isn't the same as the path on disk)
	const FString ClassModuleRelativePath = ClassModuleRelativeIncludePath.Left(ClassModuleRelativeIncludePath.Find(TEXT("/"), ESearchCase::CaseSensitive, ESearchDir::FromEnd));
	const FString ClassHierarchyPath = ClassModuleName.ToString() + TEXT("/") + ClassModuleRelativePath;

	// Ensure we've added a valid root node
	TSharedPtr<FT1NativeClassHierarchyNode>& RootNode = RootNodes.FindOrAdd(RootNodeName);
	if(!RootNode.IsValid())
	{
		RootNode = FT1NativeClassHierarchyNode::MakeFolderEntry(RootNodeName, TEXT("/") + RootNodeName.ToString());
		RootNode->LoadedFrom = WhereLoadedFrom;
		++AddClassMetrics.NumFoldersAdded;
	}

	// Split the class path and ensure we have nodes for each part
	TArray<FString> HierarchyPathParts;
	ClassHierarchyPath.ParseIntoArray(HierarchyPathParts, TEXT("/"), true);
	TSharedPtr<FT1NativeClassHierarchyNode> CurrentNode = RootNode;
	for(const FString& HierarchyPathPart : HierarchyPathParts)
	{
		const FName HierarchyPathPartName = *HierarchyPathPart;
		TSharedPtr<FT1NativeClassHierarchyNode>& ChildNode = CurrentNode->Children.FindOrAdd(FT1NativeClassHierarchyNodeKey(HierarchyPathPartName, ET1NativeClassHierarchyNodeType::Folder));
		if(!ChildNode.IsValid())
		{
			ChildNode = FT1NativeClassHierarchyNode::MakeFolderEntry(HierarchyPathPartName, CurrentNode->EntryPath + TEXT("/") + HierarchyPathPart);
			++AddClassMetrics.NumFoldersAdded;
		}
		CurrentNode = ChildNode;
	}

	// Now add the final entry for the class
	CurrentNode->AddChild(FT1NativeClassHierarchyNode::MakeClassEntry(InClass, ClassModuleName, ClassModuleRelativePath, CurrentNode->EntryPath + TEXT("/") + InClass->GetName()));
	++AddClassMetrics.NumClassesAdded;
}

void FT1NativeClassHierarchy::AddFolder(const FString& InClassPath)
{
	bool bHasAddedFolder = false;

	// Split the class path and ensure we have nodes for each part
	TArray<FString> ClassPathParts;
	InClassPath.ParseIntoArray(ClassPathParts, TEXT("/"), true);
	TSharedPtr<FT1NativeClassHierarchyNode> CurrentNode;
	for(const FString& ClassPathPart : ClassPathParts)
	{
		const FName ClassPathPartName = *ClassPathPart;
		TSharedPtr<FT1NativeClassHierarchyNode>& ChildNode = (CurrentNode.IsValid()) ? CurrentNode->Children.FindOrAdd(FT1NativeClassHierarchyNodeKey(ClassPathPartName, ET1NativeClassHierarchyNodeType::Folder)) : RootNodes.FindOrAdd(ClassPathPartName);
		if(!ChildNode.IsValid())
		{
			ChildNode = FT1NativeClassHierarchyNode::MakeFolderEntry(ClassPathPartName, CurrentNode->EntryPath + TEXT("/") + ClassPathPart);
			bHasAddedFolder = true;
		}
		CurrentNode = ChildNode;
	}

	if(bHasAddedFolder)
	{
		ClassHierarchyUpdatedDelegate.Broadcast();
	}
}

bool FT1NativeClassHierarchy::GetFileSystemPath(const FString& InClassPath, FString& OutFileSystemPath) const
{
	// Split the class path into its component parts
	TArray<FString> ClassPathParts;
	InClassPath.ParseIntoArray(ClassPathParts, TEXT("/"), true);
	
	// We need to have at least two sections (a root, and a module name) to be able to resolve a file system path
	if(ClassPathParts.Num() < 2)
	{
		return false;
	}

	// Is this path using a known root?
	TSharedPtr<FT1NativeClassHierarchyNode> RootNode = RootNodes.FindRef(*ClassPathParts[0]);
	if(!RootNode.IsValid())
	{
		return false;
	}

	// Is this path using a known module within that root?
	TSharedPtr<FT1NativeClassHierarchyNode> ModuleNode = RootNode->Children.FindRef(FT1NativeClassHierarchyNodeKey(*ClassPathParts[1], ET1NativeClassHierarchyNodeType::Folder));
	if(!ModuleNode.IsValid())
	{
		return false;
	}

	// Get the base file path to the module, and then append any remaining parts of the class path (as the remaining parts mirror the file system)
	if(FSourceCodeNavigation::FindModulePath(ClassPathParts[1], OutFileSystemPath))
	{
		for(int32 PathPartIndex = 2; PathPartIndex < ClassPathParts.Num(); ++PathPartIndex)
		{
			OutFileSystemPath /= ClassPathParts[PathPartIndex];
		}
		return true;
	}

	return false;
}

bool FT1NativeClassHierarchy::GetClassPath(UClass* InClass, FString& OutClassPath, const bool bIncludeClassName) const
{
	const FName ClassModuleName = GetClassModuleName(InClass);
	if(ClassModuleName.IsNone())
	{
		return false;
	}

	static const FName ModuleRelativePathMetaDataKey = "ModuleRelativePath";
	const FString& ClassModuleRelativeIncludePath = InClass->GetMetaData(ModuleRelativePathMetaDataKey);
	if(ClassModuleRelativeIncludePath.IsEmpty())
	{
		return false;
	}

	TSet<FName> GameModules = GetGameModules();
	TMap<FName, FT1NativeClassHierarchyPluginModuleInfo> PluginModules = GetPluginModules();

	// Work out which root this class should go under
	EPluginLoadedFrom WhereLoadedFrom;
	const FName RootNodeName = GetClassPathRootForModule(ClassModuleName, GameModules, PluginModules, WhereLoadedFrom);

	// Work out the final path to this class within the hierarchy (which isn't the same as the path on disk)
	const FString ClassModuleRelativePath = ClassModuleRelativeIncludePath.Left(ClassModuleRelativeIncludePath.Find(TEXT("/"), ESearchCase::CaseSensitive, ESearchDir::FromEnd));
	OutClassPath = FString(TEXT("/")) + RootNodeName.ToString() + TEXT("/") + ClassModuleName.ToString();

	if(!ClassModuleRelativePath.IsEmpty())
	{
		OutClassPath += TEXT("/") + ClassModuleRelativePath;
	}

	if(bIncludeClassName)
	{
		OutClassPath += TEXT("/") + InClass->GetName();
	}

	return true;
}

void FT1NativeClassHierarchy::OnModulesChanged(FName InModuleName, EModuleChangeReason InModuleChangeReason)
{
	switch(InModuleChangeReason)
	{
	case EModuleChangeReason::ModuleLoaded:
		AddClassesForModule(InModuleName);
		break;

	case EModuleChangeReason::ModuleUnloaded:
		RemoveClassesForModule(InModuleName);
		break;

	default:
		break;
	}
}

void FT1NativeClassHierarchy::OnHotReload(bool bWasTriggeredAutomatically)
{
	PopulateHierarchy();
}

FName FT1NativeClassHierarchy::GetClassModuleName(UClass* InClass)
{
	UPackage* const ClassPackage = InClass->GetOuterUPackage();

	if(ClassPackage)
	{
		return FPackageName::GetShortFName(ClassPackage->GetFName());
	}

	return NAME_None;
}

FName FT1NativeClassHierarchy::GetClassPathRootForModule(const FName& InModuleName, const TSet<FName>& InGameModules, const TMap<FName, FT1NativeClassHierarchyPluginModuleInfo>& InPluginModules, EPluginLoadedFrom& OutWhereLoadedFrom)
{
	static const FName EngineRootNodeName = "Classes_Engine";
	static const FName GameRootNodeName = "Classes_Game";

	// Work out which root this class should go under (anything that isn't a game or plugin module goes under engine)
	FName RootNodeName = EngineRootNodeName;
	OutWhereLoadedFrom = EPluginLoadedFrom::Engine;

	if(InGameModules.Contains(InModuleName))
	{
		RootNodeName = GameRootNodeName;
		OutWhereLoadedFrom = EPluginLoadedFrom::Project;
	}
	else if(InPluginModules.Contains(InModuleName))
	{
		const FT1NativeClassHierarchyPluginModuleInfo PluginInfo = InPluginModules.FindRef(InModuleName);
		RootNodeName = FName(*(FString(TEXT("Classes_")) + PluginInfo.Name.ToString()));
		OutWhereLoadedFrom = PluginInfo.LoadedFrom;
	}

	return RootNodeName;
}

TSet<FName> FT1NativeClassHierarchy::GetGameModules()
{
	FGameProjectGenerationModule& GameProjectModule = FGameProjectGenerationModule::Get();

	// Build up a set of known game modules - used to work out which modules populate Classes_Game
	TSet<FName> GameModules;
	if(GameProjectModule.ProjectHasCodeFiles())
	{
		TArray<FModuleContextInfo> GameModulesInfo = GameProjectModule.GetCurrentProjectModules();
		for(const auto& GameModuleInfo : GameModulesInfo)
		{
			GameModules.Add(FName(*GameModuleInfo.ModuleName));
		}
	}

	return GameModules;
}

TMap<FName, FT1NativeClassHierarchyPluginModuleInfo> FT1NativeClassHierarchy::GetPluginModules()
{
	IPluginManager& PluginManager = IPluginManager::Get();

	// Build up a map of plugin modules -> plugin names - used to work out which modules populate a given Classes_PluginName
	TMap<FName, FT1NativeClassHierarchyPluginModuleInfo> PluginModules;
	{
		TArray<TSharedRef<IPlugin>> Plugins = PluginManager.GetDiscoveredPlugins();
		for(const TSharedRef<IPlugin>& Plugin: Plugins)
		{
			for(const FModuleDescriptor& PluginModule: Plugin->GetDescriptor().Modules)
			{
				FT1NativeClassHierarchyPluginModuleInfo Info;
				Info.Name = FName(*Plugin->GetName());
				Info.LoadedFrom = Plugin->GetLoadedFrom();
				PluginModules.Add(PluginModule.Name, Info);
			}
		}
	}

	return PluginModules;
}
