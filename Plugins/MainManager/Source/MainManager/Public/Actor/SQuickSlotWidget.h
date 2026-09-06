// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/AssetData.h"
#include "AssetThumbnail.h"

DECLARE_DELEGATE_OneParam(FOnAssetChanged, TSharedPtr<FAssetData>)
DECLARE_DELEGATE(FOnRemoveRequested)
DECLARE_DELEGATE_RetVal_OneParam(bool, FIsDuplicateCheck, UObject*)

class SQuickSlotWidget : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SQuickSlotWidget) {}
        SLATE_ARGUMENT(TSharedPtr<FAssetThumbnailPool>, ThumbnailPool)
        SLATE_ARGUMENT(TSharedPtr<FAssetData>, AssetData)
        SLATE_EVENT(FOnAssetChanged, OnAssetChanged)
        SLATE_EVENT(FOnRemoveRequested, OnRemoveRequested)
        SLATE_EVENT(FIsDuplicateCheck, IsDuplicateCheck)
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    TSharedPtr<FAssetThumbnailPool> ThumbnailPool;
    TSharedPtr<FAssetData> AssetData;
    TSharedPtr<SVerticalBox> SlotContent;

    FOnAssetChanged OnAssetChanged;
    FOnRemoveRequested OnRemoveRequested;
    FIsDuplicateCheck IsDuplicateCheck;

    void RefreshSlot();
    void FocusContentBrowser();
};