// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class MYBLASTER_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShieldBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShieldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BlueTeamScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RedTeamScore;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreSpacerText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatsAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountDownText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadesText;

	UPROPERTY(meta = (BindWidget))
	class UImage* HigPingImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* RifleIcon;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* EliminatedAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* RifleInAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* RifleOutAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* TimerRedBlinkAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* HighPingAnimation;
};





