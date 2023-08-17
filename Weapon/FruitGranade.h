// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "FruitGranade.generated.h"

/**
 * 
 */
UCLASS()
class MYBLASTER_API AFruitGranade : public AProjectile
{
	GENERATED_BODY()

public:
	AFruitGranade();
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*> FruitsArray;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpuse, const FHitResult& Hit) override;

	UFUNCTION(NetMulticast, Reliable)
	void OwnMulticast_OnHit(AActor* OtherActor);

private:

	UPROPERTY(EditAnywhere)
	USoundCue* BounceSound;

	bool ProjectileHits = false;

	int32 bounceTimes = 0;

};