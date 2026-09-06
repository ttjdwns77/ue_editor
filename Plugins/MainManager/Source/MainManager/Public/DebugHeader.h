#pragma once

#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

namespace DebugHeader
{
	static void Print(const FString& Message, const FColor& Color)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 8.f, Color, Message);
		}
	}

	static void printlog(const FString& Message)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
	}

	static EAppReturnType::Type ShowMsgdialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMsgasWarning = true)
	{
		if (bShowMsgasWarning)
		{
			FText msgtitle = FText::FromString(TEXT("warning"));

			return FMessageDialog::Open(MsgType, FText::FromString(Message), msgtitle);
		}
		else
		{
			return FMessageDialog::Open(MsgType, FText::FromString(Message));
		}

	}

	static void ShowNotifyInfo(const FString& Message)
	{
		FNotificationInfo NotifyInfo(FText::FromString(Message));
		NotifyInfo.bUseLargeFont = true;
		NotifyInfo.FadeOutDuration = 7.f;

		FSlateNotificationManager::Get().AddNotification(NotifyInfo);

	}
}