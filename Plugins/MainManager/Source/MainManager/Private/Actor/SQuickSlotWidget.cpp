#include "Actor/SQuickSlotWidget.h"
#include "ContentBrowserDataDragDropOp.h"
#include "Editor.h"
#include "AssetThumbnail.h"
#include "Engine/Blueprint.h"
#include "Engine/StaticMeshActor.h"
#include "Misc/MessageDialog.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

void SQuickSlotWidget::Construct(const FArguments& InArgs)
{
    ThumbnailPool = InArgs._ThumbnailPool;
    AssetData = InArgs._AssetData;
    OnAssetChanged = InArgs._OnAssetChanged;
    OnRemoveRequested = InArgs._OnRemoveRequested;
    IsDuplicateCheck = InArgs._IsDuplicateCheck;

    ChildSlot
    [
    SNew(SBorder)
    .Padding(5)
    .BorderBackgroundColor(FLinearColor::Gray)
        [
            SAssignNew(SlotContent, SVerticalBox)
        ]
    ];

    RefreshSlot();
}

void SQuickSlotWidget::RefreshSlot()
{
    SlotContent->ClearChildren();

    SlotContent->AddSlot().AutoHeight()
        [
            SNew(SButton)
                .Text(FText::FromString(TEXT("X")))
                .OnClicked_Lambda([this]()
                    {
                        OnRemoveRequested.ExecuteIfBound();
                        return FReply::Handled();
                    })
        ];

    if (AssetData.IsValid())
    {
        TSharedPtr<FAssetThumbnail> Thumbnail = MakeShareable(
            new FAssetThumbnail(*AssetData, 64, 64, ThumbnailPool)
        );

        SlotContent->AddSlot().AutoHeight()
            [
                Thumbnail->MakeThumbnailWidget()
            ];

        SlotContent->AddSlot().AutoHeight()
            [
                SNew(STextBlock)
                    .Text(FText::FromName(AssetData->AssetName))
                    .Justification(ETextJustify::Center)
            ];
    }

    else
    {
        SlotContent->AddSlot()
            .AutoHeight()
            [
                SNew(STextBlock)
                    .Text(FText::FromString(TEXT("Slot")))
                    .Justification(ETextJustify::Center)
            ];
    }
}

FReply SQuickSlotWidget::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
    if (const TSharedPtr<FAssetDragDropOp> AssetDragDrop = DragDropEvent.GetOperationAs<FAssetDragDropOp>())
    {
        if (AssetDragDrop->GetAssets().Num() > 0)
        {
            UObject* DroppedObject = AssetDragDrop->GetAssets()[0].GetAsset();
            if (IsDuplicateCheck.IsBound() && IsDuplicateCheck.Execute(DroppedObject))
            {
                FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("This asset is already registered")));
                return FReply::Handled();
            }

            AssetData = MakeShared<FAssetData>(AssetDragDrop->GetAssets()[0]);
            RefreshSlot();
            OnAssetChanged.ExecuteIfBound(AssetData);
            return FReply::Handled();
        }
    }
    return FReply::Unhandled();
}

FReply SQuickSlotWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        FMenuBuilder MenuBuilder(true, nullptr);
        MenuBuilder.AddMenuEntry(
            FText::FromString(TEXT("Open in content browser")),
            FText::GetEmpty(),
            FSlateIcon(),
            FUIAction(FExecuteAction::CreateSP(this, &SQuickSlotWidget::FocusContentBrowser))
        );

        FSlateApplication::Get().PushMenu(
            SharedThis(this),
            FWidgetPath(),
            MenuBuilder.MakeWidget(),
            MouseEvent.GetScreenSpacePosition(),
            FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
        );

        return FReply::Handled();
    }


    if (!AssetData.IsValid())
        return FReply::Unhandled();

    UObject* Object = AssetData->GetAsset();
    if (!Object)
        return FReply::Unhandled();

    UWorld* World = GEditor->GetEditorWorldContext().World();
    if (!World)
        return FReply::Unhandled();

    // StaticMesh → StaticMeshActor 예외처리
    if (UStaticMesh* Mesh = Cast<UStaticMesh>(Object))
    {
        FActorSpawnParameters Params;
        AStaticMeshActor* NewActor = World->SpawnActor<AStaticMeshActor>(FVector::ZeroVector, FRotator::ZeroRotator, Params);
        if (NewActor && NewActor->GetStaticMeshComponent())
        {
            NewActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
        }
        return FReply::Handled();
    }

    // Blueprint Class Actor 검사
    UClass* SpawnClass = nullptr;
    if (UBlueprint* BP = Cast<UBlueprint>(Object))
    {
        SpawnClass = BP->GeneratedClass;
    }
    else if (UClass* ClassObj = Cast<UClass>(Object))
    {
        SpawnClass = ClassObj;
    }

    if (!SpawnClass || !SpawnClass->IsChildOf<AActor>())
    {
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Actor로 스폰할 수 없는 에셋입니다.")));
        return FReply::Unhandled();
    }

    // Actor Class 스폰
    FActorSpawnParameters Params;
    World->SpawnActor<AActor>(SpawnClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
    return FReply::Handled();


}

void SQuickSlotWidget::FocusContentBrowser()
{
    if (!AssetData.IsValid())
        return;

    UObject* Object = AssetData->GetAsset();
    if (!Object)
        return;

    FContentBrowserModule& CBModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
    TArray<UObject*> ObjsToSync;
    ObjsToSync.Add(Object);
    CBModule.Get().SyncBrowserToAssets(ObjsToSync);
}