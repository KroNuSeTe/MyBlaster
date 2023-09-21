// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MyBlaster/Character/BlasterCharacter.h"
#include "MyBlaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyBlaster/BlasterComponents/LagCompensationComponent.h"
#include "Projectile.h"

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		// Maps hit character to number of times hit
		TMap<ABlasterCharacter*, uint32> HitMap;
		TMap<ABlasterCharacter*, uint32> HeadShotHitMap;

		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			ESurfaceType SurfaceType = ESurfaceType::EST_Stone;
			if (FireHit.bBlockingHit)
			{
				if (FireHit.GetActor()->ActorHasTag(TEXT("Player")))
				{
					SurfaceType = ESurfaceType::EST_Player;
				}
				else if (FireHit.GetActor()->ActorHasTag(TEXT("Wood")))
				{
					SurfaceType = ESurfaceType::EST_Wood;
				}
				else if (FireHit.GetActor()->ActorHasTag(TEXT("Stone")))
				{
					UE_LOG(LogTemp, Warning, TEXT("Stone"));
					SurfaceType = ESurfaceType::EST_Stone;
				}
				else if (FireHit.GetActor()->ActorHasTag(TEXT("Metal"))) {
					SurfaceType = ESurfaceType::EST_Metal;
				}

				switch (SurfaceType)
				{
				case ESurfaceType::EST_Player:
					if (ImpactParticles_Player) {
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles_Player, FireHit.ImpactPoint,
							FireHit.ImpactNormal.Rotation());
					}
					if (ImpactSound_Player) {
						UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Player, FireHit.ImpactPoint, .2f, FMath::FRandRange(-.2f, .2f));
					}
					break;
				case ESurfaceType::EST_Stone:
					if (ImpactParticles_Stone) {
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles_Stone, FireHit.ImpactPoint,
							FireHit.ImpactNormal.Rotation());
					}
					if (ImpactSound_Stone) {
						UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Stone, FireHit.ImpactPoint, .2f, FMath::FRandRange(-.2f, .2f));
					}
					break;
				case ESurfaceType::EST_Metal:
					if (ImpactParticles_Metal) {
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles_Metal, FireHit.ImpactPoint,
							FireHit.ImpactNormal.Rotation());
					}
					if (ImpactSound_Metal) {
						UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Metal, FireHit.ImpactPoint, .2f, FMath::FRandRange(-.2f, .2f));
					}
					break;
				case ESurfaceType::EST_Wood:
					if (ImpactParticles_Wood) {
						UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles_Wood, FireHit.ImpactPoint,
							FireHit.ImpactNormal.Rotation());
					}
					if (ImpactSound_Wood) {
						UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Wood, FireHit.ImpactPoint, .2f, FMath::FRandRange(-.2f, .2f));
					}
					break;
				}
			}

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter)
			{
				const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");

				if (bHeadShot)
				{
					if (HeadShotHitMap.Contains(BlasterCharacter)) HeadShotHitMap[BlasterCharacter]++;
					else HeadShotHitMap.Emplace(BlasterCharacter, 1);
				}
				else
				{
					if (HitMap.Contains(BlasterCharacter)) HitMap[BlasterCharacter]++;
					else HitMap.Emplace(BlasterCharacter, 1);
				}
			}
		}
		TArray<ABlasterCharacter*> HitCharacters;

		// Maps Character hit to total damage
		TMap<ABlasterCharacter*, float> DamageMap;

		// Calculate body shot damage by multiplying times hit x Damage - store in DamageMap
		for (auto HitPair : HitMap)
		{
			if (HitPair.Key)
			{
				DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);

				HitCharacters.AddUnique(HitPair.Key);
			}
		}

		// Calculate head shot damage by multiplying times hit x HeadShotDamage - store in DamageMap
		for (auto HeadShotHitPair : HeadShotHitMap)
		{
			if (HeadShotHitPair.Key)
			{
				if (DamageMap.Contains(HeadShotHitPair.Key)) DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * HeadShotDamage;
				else DamageMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * HeadShotDamage);

				HitCharacters.AddUnique(HeadShotHitPair.Key);
			}
		}

		// Loop through DamageMap to get total damage for each character
		for (auto DamagePair : DamageMap)
		{
			if (DamagePair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					UGameplayStatics::ApplyDamage(
						DamagePair.Key,	// Character that was hit
						DamagePair.Value, // Damage calculated in the two for loops above
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
				}
			}
		}

		if (!HasAuthority() && bUseServerSideRewind)
		{
			BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
			BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
			if (BlasterOwnerController && BlasterOwnerCharacter && BlasterOwnerCharacter->GetLagCompensation() && BlasterOwnerCharacter->IsLocallyControlled())
			{
				BlasterOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
					HitCharacters,
					Start,
					HitTargets,
					BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime
				);
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return;

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		FVector ToEndLoc = EndLoc - TraceStart;
		ToEndLoc = TraceStart + ToEndLoc * TRACE_LENGHT / ToEndLoc.Size();

		HitTargets.Add(ToEndLoc);
	}
}

void AShotgun::ShotgunImpactPointEffectAndSound()
{

}

