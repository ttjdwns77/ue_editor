// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FMainManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

public:

#pragma region ToolMenus
	void MakeToolMenuEntry();
	void MakeMenuSectionEntry();

#pragma endregion

#pragma region RegisterEntry

	void RegisterGlobalTab();
	TSharedRef<SDockTab> OnSpawnCleanUpAssetsTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedPtr<SDockTab> ConstructedCleanUpTab;

	TSharedRef<SDockTab> OnSpawnActorQuickMenuTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedPtr<SDockTab> ConstructedActorQuickTab;

	//TSharedRef<SDockTab> OnSpawnActorQuickManager(const FSpawnTabArgs& SpawnTabArgs);

#pragma endregion

#pragma region EntryAction

	void OnAssetsCleanUpTabInvoke();
	void OnAssetsCleanUpTablClose(TSharedRef<SDockTab> TabtoClose);

	void OnActorQuickMenuTabInvoke();
	void OnActorQuickMenuTablClose(TSharedRef<SDockTab> TabtoClose);

#pragma endregion

	

	
};
