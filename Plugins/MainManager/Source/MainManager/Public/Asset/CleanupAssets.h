#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"
#include "Widgets/Views/SListView.h"

// ============================================================================
// CleanupAssets : Asset Á¤¸® À§Á¬
// ============================================================================

class SEditableTextBox;
class SButton;
class SCheckBox;

#pragma region Widget Class
class CleanupAssets : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(CleanupAssets) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
#pragma endregion


#pragma region Menu Action

	TSharedPtr<SEditableTextBox> SelectedFolderPathTextBox;
	TSharedPtr<SEditableTextBox> DestinationFolderPathTextBox;

	TSharedRef<SEditableTextBox> CreateSelectedFolderPathTextBox();
	TSharedRef<SButton> SelectedFolderPathButton();
	TSharedRef<SEditableTextBox> CreateDestinationFolderPathTextBox();

	FReply OnSelectedFolderPathButton();
	FReply OnExecuteMoveButton();

	TArray<TSharedPtr<FAssetData>> GetAssetDataUnderSelectedFodler();

#pragma endregion


#pragma region ListView Body

private:
	static const FName ColumnID_CheckState;
	static const FName ColumnID_AssetClassName;
	static const FName ColumnID_AssetName;
	static const FName ColumnID_AssetPath;

	TArray<TSharedPtr<FAssetData>> DisplayedAssetsData;
	TArray<TSharedPtr<FAssetData>> CheckAssetsArray;
	TArray<TSharedPtr<SCheckBox>> CheckBoxesArray;
	TArray<float> ColumnWidths;

	TSharedPtr<SListView<TSharedPtr<FAssetData>>> ConstructedAssetListView;

	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay);
	TSharedPtr<SWidget> ConstructContextMenu();

	void OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);
	void OnColumnWidthChanged(float NewWidth, int32 ColumnIndex);

#pragma endregion


#pragma region Widget Create (UI Helper)

	TSharedRef<STextBlock> CreateTextForButton(const FString& TextContent);
	TSharedRef<STextBlock> CreateTextForRowWidget(const FString& TextContent, const FSlateFontInfo& FontToUse);
	FText GetAssetCounText() const;
	FSlateFontInfo GetEmboseedTextFont() const { return FCoreStyle::Get().GetFontStyle(FName("EmbossedText")); }

#pragma endregion

};
#pragma endregion
