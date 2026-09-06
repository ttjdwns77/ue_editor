// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetThumbnail.h"

class SUniformGridPanel;
class SQuickSlotWidget;

class SActorQuickManager : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SActorQuickManager) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

    bool IsAlreadyRegistered(const UObject* InObject) const;

private:
    TSharedPtr<SUniformGridPanel> GridPanel;
    TSharedPtr<FAssetThumbnailPool> ThumbnailPool;

    int32 NumColumns = 4;
    TArray<TSharedPtr<FAssetData>> QuickSlotAssets;

    FReply OnAddSlot();
    void RefreshQuickSlotGrid();
};