// Fill out your copyright notice in the Description page of Project Settings.

#include "Asset/CleanupAssets.h"
#include "SlateBasics.h"
#include "DebugHeader.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserItemPath.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"

#include "EditorAssetLibrary.h"
//#include "../../../../../../../../Program Files/Epic Games/UE_5.3/Engine/Plugins/Editor/EditorScriptingUtilities/Source/EditorScriptingUtilities/Public/EditorAssetLibrary.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"

#define LOCTEXT_NAMESPACE "CleanupAssets"


#pragma region Column ID
const FName CleanupAssets::ColumnID_CheckState(TEXT("Check State"));
const FName CleanupAssets::ColumnID_AssetClassName(TEXT("Asset Class"));
const FName CleanupAssets::ColumnID_AssetName(TEXT("Asset Name"));
const FName CleanupAssets::ColumnID_AssetPath(TEXT("Asset Path"));
#pragma endregion


#pragma region Construct
void CleanupAssets::Construct(const FArguments& InArgs)
{
	DisplayedAssetsData.Empty();
	CheckAssetsArray.Empty();
	CheckBoxesArray.Empty();
	ColumnWidths = { 100.f, 200.f, 300.f, 400.f };

	FSlateFontInfo TitleTextFont = GetEmboseedTextFont();
	TitleTextFont.Size = 30;

	ChildSlot
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock).Text(FText::FromString("CleanUp Assets")).Justification(ETextJustify::Center).Font(TitleTextFont)
				]

				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().AutoWidth().Padding(3.f)
						[
							CreateTextForButton(TEXT("Folder Path : "))
						]
					+ SHorizontalBox::Slot().FillWidth(1.f).Padding(3.f)
						[
							CreateSelectedFolderPathTextBox()
						]
					+ SHorizontalBox::Slot().AutoWidth().Padding(3.f)
						[
							SelectedFolderPathButton()
						]
				]

				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SHorizontalBox)
						+ SHorizontalBox::Slot().AutoWidth().Padding(3.f)
						[
							CreateTextForButton(TEXT("Destination Folder Path : "))
						]
					+ SHorizontalBox::Slot().FillWidth(1.f).Padding(3.f)
						[
							CreateDestinationFolderPathTextBox()
						]
					+ SHorizontalBox::Slot().AutoWidth().Padding(3.f)
						[
							SNew(SButton).Text(FText::FromString("CleanUp")).OnClicked(this, &CleanupAssets::OnExecuteMoveButton)
						]
				]


				+ SVerticalBox::Slot().VAlign(VAlign_Fill)
				[
					SNew(SScrollBox) + SScrollBox::Slot()[ConstructAssetListView()]
				]

				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock).Text(this, &CleanupAssets::GetAssetCounText)
				]
		];
}
#pragma endregion


#pragma region Menu Action
TSharedRef<SEditableTextBox> CleanupAssets::CreateSelectedFolderPathTextBox()
{
	return SAssignNew(SelectedFolderPathTextBox, SEditableTextBox)
		.HintText(FText::FromString("Select Content Browser Folder"))
		.Justification(ETextJustify::Center)
		.IsReadOnly(true);
}

TSharedRef<SButton> CleanupAssets::SelectedFolderPathButton()
{
	return SNew(SButton)
		.OnClicked(this, &CleanupAssets::OnSelectedFolderPathButton)
		[CreateTextForButton(TEXT("Select CB Folder"))];
}

FReply CleanupAssets::OnSelectedFolderPathButton()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	IContentBrowserSingleton& ContentBrowserSingleton = ContentBrowserModule.Get();
	FString CurrentPath = ContentBrowserSingleton.GetCurrentPath(EContentBrowserPathType::Internal);

	if (CurrentPath.IsEmpty())
	{
		DebugHeader::ShowMsgdialog(EAppMsgType::Ok, TEXT("Please select one folder"));
		return FReply::Handled();
	}

	SelectedFolderPathTextBox->SetText(FText::FromString(CurrentPath));
	DisplayedAssetsData = GetAssetDataUnderSelectedFodler();

	if (ConstructedAssetListView.IsValid())
		ConstructedAssetListView->RebuildList();

	return FReply::Handled();
}

TArray<TSharedPtr<FAssetData>> CleanupAssets::GetAssetDataUnderSelectedFodler()
{
	TArray<TSharedPtr<FAssetData>> AvailableAssetsData;
	TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(SelectedFolderPathTextBox->GetText().ToString());

	for (FString& AssetPathName : AssetsPathNames)
	{
		if (AssetPathName.Contains(TEXT("Developers")) || AssetPathName.Contains(TEXT("Collections")))
			continue;
		if (!UEditorAssetLibrary::DoesAssetExist(AssetPathName))
			continue;
		const FAssetData Data = UEditorAssetLibrary::FindAssetData(AssetPathName);
		AvailableAssetsData.Add(MakeShared<FAssetData>(Data));
	}
	return AvailableAssetsData;
}
#pragma endregion


#pragma region ListView
TSharedRef<SListView<TSharedPtr<FAssetData>>> CleanupAssets::ConstructAssetListView()
{
	ConstructedAssetListView = SNew(SListView<TSharedPtr<FAssetData>>)
		.SelectionMode(ESelectionMode::Multi)
		.ItemHeight(24.f)
		.ListItemsSource(&DisplayedAssetsData)
		.OnGenerateRow(this, &CleanupAssets::OnGenerateRowForList)
		.OnContextMenuOpening(this, &CleanupAssets::ConstructContextMenu)
		.HeaderRow(
			SNew(SHeaderRow)
			+ SHeaderRow::Column(ColumnID_CheckState).DefaultLabel(LOCTEXT("Checked", "Checked")).ManualWidth(ColumnWidths[0])
			+ SHeaderRow::Column(ColumnID_AssetClassName).DefaultLabel(LOCTEXT("Class", "Class")).ManualWidth(ColumnWidths[1])
			+ SHeaderRow::Column(ColumnID_AssetName).DefaultLabel(LOCTEXT("Name", "Name")).ManualWidth(ColumnWidths[2])
			+ SHeaderRow::Column(ColumnID_AssetPath).DefaultLabel(LOCTEXT("Path", "Path")).ManualWidth(ColumnWidths[3])
		);

	return ConstructedAssetListView.ToSharedRef();
}

TSharedPtr<SWidget> CleanupAssets::ConstructContextMenu()
{
	FMenuBuilder MenuBuilder(true, nullptr);
	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Check All")),
		FText::GetEmpty(),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([this]()
			{
				for (auto& CheckBox : CheckBoxesArray)
					CheckBox->SetIsChecked(ECheckBoxState::Checked);
			}))
	);

	MenuBuilder.AddMenuEntry(
		FText::FromString(TEXT("Uncheck All")),
		FText::GetEmpty(),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateLambda([this]()
			{
				for (auto& CheckBox : CheckBoxesArray)
					CheckBox->SetIsChecked(ECheckBoxState::Unchecked);
			}))
	);

	return MenuBuilder.MakeWidget();
}

TSharedRef<ITableRow> CleanupAssets::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay.IsValid()) return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	
	const FString DisplayAssetClassName = AssetDataToDisplay->AssetClassPath.GetAssetName().ToString();
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();
	const FString DisplayAssetPath = AssetDataToDisplay->PackageName.ToString();

	FSlateFontInfo Font = GetEmboseedTextFont();
	Font.Size = 15;

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget =
		SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable).Padding(FMargin(5.f))
		[
			SNew(SHorizontalBox)

				// First Check Box
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride_Lambda([this]() { return ColumnWidths[0]; })
					[
						ConstructCheckBox(AssetDataToDisplay)
					]
				]

				// Second Asset class
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Fill)
				[
					SNew(SBox)
					.WidthOverride_Lambda([this]() { return ColumnWidths[1]; })
					[
						CreateTextForRowWidget(DisplayAssetClassName, Font)
					]
				]

				// Third Asset name
				+ SHorizontalBox::Slot().AutoWidth()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Fill)
				[
					SNew(SBox)
						.WidthOverride_Lambda([this]() { return ColumnWidths[2]; })
						[
							CreateTextForRowWidget(DisplayAssetName, Font)
						]
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Fill)
				[
					CreateTextForRowWidget(DisplayAssetPath, Font)
				]
		];

	return ListViewRowWidget;
}

TSharedRef<SCheckBox> CleanupAssets::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> CheckBox = SNew(SCheckBox)
		.OnCheckStateChanged(this, &CleanupAssets::OnCheckBoxStateChanged, AssetDataToDisplay);
	CheckBoxesArray.Add(CheckBox);
	return CheckBox;
}

void CleanupAssets::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	if (NewState == ECheckBoxState::Checked)
		CheckAssetsArray.AddUnique(AssetData);
	else if (NewState == ECheckBoxState::Unchecked)
		CheckAssetsArray.Remove(AssetData);
}
#pragma endregion


#pragma region Widget Create
TSharedRef<STextBlock> CleanupAssets::CreateTextForButton(const FString& TextContent)
{
	FSlateFontInfo Font = GetEmboseedTextFont();
	Font.Size = 12;
	return SNew(STextBlock).Text(FText::FromString(TextContent)).Font(Font).Justification(ETextJustify::Center);
}

TSharedRef<STextBlock> CleanupAssets::CreateTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse)
{
	return SNew(STextBlock).Text(FText::FromString(TextContent)).Font(FontToUse).ColorAndOpacity(FColor::White);
}

FText CleanupAssets::GetAssetCounText() const
{
	return FText::Format(LOCTEXT("ListItemCount", "{0} items"), FText::AsNumber(DisplayedAssetsData.Num()));
}
#pragma endregion


#pragma region Asset Move Action
TSharedRef<SEditableTextBox> CleanupAssets::CreateDestinationFolderPathTextBox()
{
	return SAssignNew(DestinationFolderPathTextBox, SEditableTextBox)
		.Text(FText::FromString("/Game/_DstFolder"))
		.Justification(ETextJustify::Center);
}

FReply CleanupAssets::OnExecuteMoveButton()
{
	if (FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(TEXT("Are you sure you want to clean up the checked assets?"))) != EAppReturnType::Yes)
	{
		return FReply::Handled();
	}

	FString DstBasePath = DestinationFolderPathTextBox->GetText().ToString();

	// 목적지 폴더를 기입하지 않을경우 리턴
	if (DstBasePath.IsEmpty()) 
	{
		DebugHeader::ShowMsgdialog(EAppMsgType::Ok, TEXT("Please select Destination folder."));
		return FReply::Handled();
	}

	if (!UEditorAssetLibrary::DoesDirectoryExist(DstBasePath))
		UEditorAssetLibrary::MakeDirectory(DstBasePath);

	// 체크한 어셋이 없는경우 리턴
	if (CheckAssetsArray.Num() == 0)
	{
		DebugHeader::ShowMsgdialog(EAppMsgType::Ok, TEXT("No Assets Checked."));
		return FReply::Handled();
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	TMap<FString, TArray<FAssetData>> AssetsByClass;

	for (const TSharedPtr<FAssetData>& Asset : CheckAssetsArray)
	{
		FString ClassName = Asset->AssetClassPath.GetAssetName().ToString();
		AssetsByClass.FindOrAdd(ClassName).Add(*Asset);
	}

	for (auto& Pair : AssetsByClass)
	{
		FString ClassName = Pair.Key;
		if (ClassName.IsEmpty() || ClassName == "None")
			continue;

		FString TargetPath = DstBasePath / ClassName;
		if (!UEditorAssetLibrary::DoesDirectoryExist(TargetPath))
			UEditorAssetLibrary::MakeDirectory(TargetPath);

		TArray<FAssetRenameData> AssetsToRename;
		for (const FAssetData& Asset : Pair.Value)
		{
			const FString NewName = Asset.AssetName.ToString();
			AssetsToRename.Add(FAssetRenameData(Asset.GetAsset(), TargetPath, NewName));
		}
		AssetToolsModule.Get().RenameAssetsWithDialog(AssetsToRename);
	}

	DisplayedAssetsData = GetAssetDataUnderSelectedFodler();
	ConstructedAssetListView->RebuildList();

	FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("CleanUp Completed.")));
	return FReply::Handled();
}
#pragma endregion


#undef LOCTEXT_NAMESPACE
