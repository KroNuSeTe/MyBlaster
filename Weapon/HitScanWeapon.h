// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MYBLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& HitTarget) override;

protected:
	
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles_Player;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound_Player;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles_Wood;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound_Wood;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles_Stone;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound_Stone;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticles_Metal;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound_Metal;

private:

	UPROPERTY(EditAnywhere)
	class UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	USoundCue* FireSound;

};








