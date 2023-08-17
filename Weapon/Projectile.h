// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UENUM(BlueprintType)
enum class ESurfaceType : uint8
{
	EST_None UMETA(DisplayName = "None"),
	EST_Player UMETA(DisplayName = "Player"),
	EST_Wood UMETA(DisplayName = "Wood"),
	EST_Stone UMETA(DisplayName = "Stone"),
	EST_Metal UMETA(DisplayName = "Metal"),
	EST_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class MYBLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	ESurfaceType SurfaceType;

protected:
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void SpawnTrailSystem();
	void ExplodeDamage();
	virtual void Destroyed() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpuse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnHit(ESurfaceType SurfaceTypeMC);

	UPROPERTY(EditAnywhere)
	float Damage = 20.f;



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

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* Trail_System;

	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, Category = Damage)
	float MinDamage = 10.f;

	UPROPERTY(EditAnywhere, Category = Damage)
	float DamageInnerRadious = 200.f;

	UPROPERTY(EditAnywhere, Category = Damage)
	float DamageOuterRadious = 500.f;

	UPROPERTY(EditAnywhere, Category = Damage)
	float DamageFallOff = 1.f;

private:

	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	UPROPERTY()
	UParticleSystemComponent* TracerComponent;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime = 3.f;

public:	
	
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	
};
