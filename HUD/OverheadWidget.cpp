// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay, FString PlayerNameString)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}

	if (DisplayText2)
	{
		DisplayText2->SetText(FText::FromString(PlayerNameString));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Authority Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}
	FString LocalRole = FString::Printf(TEXT("Remote Role: %s"), *Role);

	APlayerState* PlayerState = InPawn->GetPlayerState();
	FString PlayerName = "";
	if (PlayerState) {
		PlayerName = PlayerState->GetPlayerName();
	}

	FString LocalPlayerName = FString::Printf(TEXT("PlayerName: %s"), *PlayerName);
	SetDisplayText(LocalRole, LocalPlayerName);
	
}

void UOverheadWidget::NativeDestruct()
{
	RemoveFromParent();

	Super::NativeDestruct();
}

