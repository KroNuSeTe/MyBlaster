// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "MyBlaster/Character/BlasterCharacter.h"
#include "MyBlaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFrameWork/PlayerStart.h"
#include "MyBlaster/PlayerState/BlasterPlayerState.h"
#include "MyBlaster/GameState/BlasterGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer)
		{
			BlasterPlayer->OnMatchStateSet(MatchState, bTeamsMatch);
		}
	}
}

float ABlasterGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}


void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
	
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState)
	{
		TArray<ABlasterPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : BlasterGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		AttackerPlayerState->AddToScore(1.f);
		BlasterGameState->UpdateTopScore(AttackerPlayerState);
		if (BlasterGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			ABlasterCharacter* Leader = Cast<ABlasterCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead();
			}
		}

		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!BlasterGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				ABlasterCharacter* Loser = Cast<ABlasterCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABlasterPlayerController* BlasterPlayer = Cast<ABlasterPlayerController>(*It);
		if (BlasterPlayer && AttackerPlayerState && VictimPlayerState)
		{
			BlasterPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		TArray<AActor*> Characters;
		TArray<float> StockedDistances;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		UGameplayStatics::GetAllActorsOfClass(this, ABlasterCharacter::StaticClass(), Characters);

		// Calculate the Closest Character to a PlayerStart, and Add distance to the array StockedDistances
		if (Characters.Num() > 0 && PlayerStarts.Num() > 0) 
		{
			for (int i = 0; i < PlayerStarts.Num(); i++)
			{
				float MinDistance = (PlayerStarts[i]->GetActorLocation() - Characters[0]->GetActorLocation()).Size();
				for (int j = 1; j < Characters.Num(); j++)
				{
					float Distance = (PlayerStarts[i]->GetActorLocation() - Characters[j]->GetActorLocation()).Size();
					if (Distance < MinDistance)
					{
						MinDistance = Distance;
					}
				}
				StockedDistances.Add(MinDistance);
			}

			// Choose the MaxDistance of the Array
			float MaxDistance = StockedDistances[0];
			int32 Selection = 0;

			for (int i = 1; i < StockedDistances.Num(); i++)
			{
				if (MaxDistance < StockedDistances[i])
				{
					MaxDistance = StockedDistances[i];
					Selection = i;
				}
			}

			RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
		}
		else if (PlayerStarts.Num() > 0)
		{
			int32 RandomStart = FMath::RandRange(0, PlayerStarts.Num() - 1);
			RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[RandomStart]);
		}


	}

}

void ABlasterGameMode::PlayerLeftGame(class ABlasterPlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
	if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		BlasterGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	ABlasterCharacter* CharacterLeaving = Cast<ABlasterCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(true);
	}
}








