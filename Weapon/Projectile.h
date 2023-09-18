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
	virtual void Destroyed() override;

	UPROPERTY(EditAnywhere)
	ESurfaceType SurfaceType;

	/**
	*	Used with server-side rewind
	*/
	bool bUseServerSideRewind = false;
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;

	UPROPERTY(EditAnywhere)
	float InitialSpeed = 15000;

	// Only set this for Grenades and Rockets
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	//Doesn't matter for Grenades and Rockets
	UPROPERTY(EditAnywhere)
	float HeadShotDamage = 40.f;

protected:
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void SpawnTrailSystem();
	void ExplodeDamage();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpuse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnHit(ESurfaceType SurfaceTypeMC);

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
	UParticleSystem* ImpactParticles_Explosion;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound_Explosion;

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
