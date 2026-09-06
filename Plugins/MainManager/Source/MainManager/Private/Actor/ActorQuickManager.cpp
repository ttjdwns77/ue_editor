// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/ActorQuickManager.h"
#include "Actor/SQuickSlotWidget.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SActorQuickManager::Construct(const FArguments& InArgs)
{
    ThumbnailPool = MakeShareable(new FAssetThumbnailPool(16));

    ChildSlot
    [
    SNew(SBorder)
    .Padding(10)
    .BorderBackgroundColor(FLinearColor::Black)
        [
        SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight().Padding(5)
            [
                SAssignNew(GridPanel, SUniformGridPanel)
                .SlotPadding(5)
                .MinDesiredSlotWidth(80)
                .MinDesiredSlotHeight(80)
            ]
        + SVerticalBox::Slot().AutoHeight().HAlign(HAlign_Right).Padding(5)
            [
                SNew(SButton).Text(FText::FromString(TEXT("Add Slot")))
                    .OnClicked(this, &SActorQuickManager::OnAddSlot)
            ]
        ]
    ];

    RefreshQuickSlotGrid();
}

FReply SActorQuickManager::OnAddSlot()
{
    QuickSlotAssets.Add(nullptr);
    RefreshQuickSlotGrid();
    return FReply::Handled();
}

bool SActorQuickManager::IsAlreadyRegistered(const UObject* InObject) const
{
    for (const auto& AssetData : QuickSlotAssets)
    {
        if (AssetData.IsValid() && AssetData->GetAsset() == InObject)
        {
            return true;
        }
    }
    return false;
}

void SActorQuickManager::RefreshQuickSlotGrid()
{
    GridPanel->ClearChildren();

    for (int32 i = 0; i < QuickSlotAssets.Num(); ++i)
    {
        int32 Row = i / NumColumns;
        int32 Col = i % NumColumns;

        TSharedRef<SQuickSlotWidget> NewSlot = SNew(SQuickSlotWidget)
            .ThumbnailPool(ThumbnailPool)
            .AssetData(QuickSlotAssets[i])
            .OnAssetChanged_Lambda([this, i](TSharedPtr<FAssetData> NewData)
                {
                    QuickSlotAssets[i] = NewData;
                })
            .OnRemoveRequested_Lambda([this, i]()
                {
                    QuickSlotAssets.RemoveAt(i);
                    RefreshQuickSlotGrid();
                })
            .IsDuplicateCheck_Lambda([this](UObject* InObj) { return IsAlreadyRegistered(InObj); });

        GridPanel->AddSlot(Col, Row)[NewSlot];
    }
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
