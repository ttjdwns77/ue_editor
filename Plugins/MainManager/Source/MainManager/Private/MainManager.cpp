// Copyright Epic Games, Inc. All Rights Reserved.

#include "MainManager.h"
#include "DebugHeader.h"

#include "LevelEditor.h"
#include "ToolMenus.h"

#include "Asset/CleanupAssets.h"
#include "Actor/ActorQuickManager.h"

#define LOCTEXT_NAMESPACE "FMainManagerModule"

void FMainManagerModule::StartupModule()
{
	// 메뉴 등록
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMainManagerModule::MakeToolMenuEntry));
	RegisterGlobalTab();
}

void FMainManagerModule::ShutdownModule()
{
	// 메뉴 해제
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName("CleanUpAssets"));
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(FName("ActorQuickMenu"));
}

#pragma region ToolMenus

void FMainManagerModule::MakeToolMenuEntry()
{
	// 모든 메뉴를 관리하기 위한 등록
	UToolMenus* ToolMenus = UToolMenus::Get();
	UToolMenu* ExistingMenu = ToolMenus->ExtendMenu("LevelEditor.MainMenu");

	if (!ExistingMenu)
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelEditor.MainMenu not found"));
		return;
	}

	FToolMenuSection& ExsitingSection = ExistingMenu->FindOrAddSection("JSJNewSection");

	FToolMenuEntry& NewJsjSection =
		ExsitingSection.AddSubMenu(
			"JSJ",
			LOCTEXT("JSJ", "JSJ Menu"),
			LOCTEXT("", ""),
			FNewToolMenuChoice()
		);

	NewJsjSection.InsertPosition = FToolMenuInsert("Help", EToolMenuInsertType::After);

	MakeMenuSectionEntry();
}

// 섹션 등록
void FMainManagerModule::MakeMenuSectionEntry()
{
	UToolMenus* ToolMenus = UToolMenus::Get();
	static const FName JsjSectionName = "LevelEditor.MainMenu.JSJ";
	UToolMenu* JsjSectionMenu = ToolMenus->RegisterMenu(JsjSectionName);

	JsjSectionMenu->AddDynamicSection("ObjectSection",
		FNewToolMenuDelegate::CreateLambda([this](UToolMenu* InMenu)
			{
				FToolMenuSection& SubSection = InMenu->AddSection("ObjectSection", LOCTEXT("ObjectSection", "Object Section"));
				SubSection.AddMenuEntry(
					"CleanUpAssets",
					LOCTEXT("CleanUpAssets", "CleanUpAssets Section"),
					LOCTEXT("", ""),
					FSlateIcon(),
					FUIAction(FExecuteAction::CreateRaw(this, &FMainManagerModule::OnAssetsCleanUpTabInvoke)));

				SubSection.AddMenuEntry(
					"ActorQuick",
					LOCTEXT("ActorQuickMenu", "ActorQuickMenu Section"),
					LOCTEXT("", ""),
					FSlateIcon(),
					FUIAction(FExecuteAction::CreateRaw(this, &FMainManagerModule::OnActorQuickMenuTabInvoke)));
			}
		)
	);
}

#pragma endregion

#pragma region RegisterEntry

void FMainManagerModule::RegisterGlobalTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("CleanUpAssets"),
		FOnSpawnTab::CreateRaw(this, &FMainManagerModule::OnSpawnCleanUpAssetsTab))
		.SetDisplayName(FText::FromString(TEXT("CleanUpAssets")))
		.SetAutoGenerateMenuEntry(false);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("ActorQuick"),
		FOnSpawnTab::CreateRaw(this, &FMainManagerModule::OnSpawnActorQuickMenuTab))
		.SetDisplayName(FText::FromString(TEXT("ActorQuick")))
		.SetAutoGenerateMenuEntry(false);
}

TSharedRef<SDockTab> FMainManagerModule::OnSpawnCleanUpAssetsTab(const FSpawnTabArgs& SpawnTabArgs)
{
	// return SNew(SDockTab).TabRole(ETabRole::NomadTab);

	ConstructedCleanUpTab = SNew(SDockTab).TabRole(ETabRole::NomadTab)
	[
		SNew(CleanupAssets)
	];

	ConstructedCleanUpTab->SetOnTabClosed(SDockTab::FOnTabClosedCallback::CreateRaw(this, &FMainManagerModule::OnAssetsCleanUpTablClose));
	return ConstructedCleanUpTab.ToSharedRef();

}

TSharedRef<SDockTab> FMainManagerModule::OnSpawnActorQuickMenuTab(const FSpawnTabArgs& SpawnTabArgs)
{
	ConstructedActorQuickTab = SNew(SDockTab).TabRole(ETabRole::NomadTab)
		[
			SNew(SActorQuickManager)
		];

	ConstructedActorQuickTab->SetOnTabClosed(
		SDockTab::FOnTabClosedCallback::CreateRaw(this, &FMainManagerModule::OnActorQuickMenuTablClose));
	return ConstructedActorQuickTab.ToSharedRef();
}

#pragma endregion

#pragma region EntryAction

void FMainManagerModule::OnAssetsCleanUpTabInvoke()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("CleanUpAssets"));
}

void FMainManagerModule::OnAssetsCleanUpTablClose(TSharedRef<SDockTab> TabtoClose)
{
	if (ConstructedCleanUpTab.IsValid())
	{
		ConstructedCleanUpTab.Reset();
	}
}

void FMainManagerModule::OnActorQuickMenuTabInvoke()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("ActorQuick"));
}

void FMainManagerModule::OnActorQuickMenuTablClose(TSharedRef<SDockTab> TabtoClose)
{
	if (ConstructedActorQuickTab.IsValid())
	{
		ConstructedActorQuickTab.Reset();
	}
}



#pragma endregion



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMainManagerModule, MainManager)