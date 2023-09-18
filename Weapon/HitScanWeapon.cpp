// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "MyBlaster/Character/BlasterCharacter.h"
#include "MyBlaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "WeaponTypes.h"
#include "MyBlaster/BlasterComponents/LagCompensationComponent.h"
#include "DrawDebugHelpers.h"
#include "Projectile.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);

		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
		if (BlasterCharacter && InstigatorController)
		{
			bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
			if (HasAuthority() && bCauseAuthDamage)
			{
				const float DamageToCause = FireHit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;

				UGameplayStatics::ApplyDamage(
					BlasterCharacter,
					DamageToCause,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
			if (!HasAuthority() && bUseServerSideRewind)
			{
				BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
				BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
				if (BlasterOwnerController && BlasterOwnerCharacter && BlasterOwnerCharacter->GetLagCompensation() && BlasterOwnerCharacter->IsLocallyControlled())
				{
					BlasterOwnerCharacter->GetLagCompensation()->ServerScoreRequest(
						BlasterCharacter,
						Start,
						HitTarget,
						BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime
					);
				}
			}

		}
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
					UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Player, GetActorLocation());
				}
				break;
			case ESurfaceType::EST_Stone:
				if (ImpactParticles_Stone) {
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles_Stone, FireHit.ImpactPoint,
						FireHit.ImpactNormal.Rotation());
				}
				if (ImpactSound_Stone) {
					UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Stone, FireHit.ImpactPoint);
				}
				break;
			case ESurfaceType::EST_Metal:
				if (ImpactParticles_Metal) {
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles_Metal, FireHit.ImpactPoint,
						FireHit.ImpactNormal.Rotation());
				}
				if (ImpactSound_Metal) {
					UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Metal, FireHit.ImpactPoint);
				}
				break;
			case ESurfaceType::EST_Wood:
				if (ImpactParticles_Wood) {
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles_Wood, FireHit.ImpactPoint,
						FireHit.ImpactNormal.Rotation());
				}
				if (ImpactSound_Wood) {
					UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Wood, FireHit.ImpactPoint);
				}
				break;
			}
		}

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform
			);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				FireSound,
				GetActorLocation()
			);
		}

	}
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;
		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility
		);
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		else
		{
			OutHit.ImpactPoint = End;
		}
		//DrawDebugSphere(GetWorld(), BeamEnd, 16.f, 12, FColor::Orange, true);
		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticles,
				TraceStart,
				FRotator::ZeroRotator,
				true
			);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}









