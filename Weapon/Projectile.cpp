// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "MyBlaster/Character/BlasterCharacter.h"
#include "MyBlaster/MyBlaster.h"
#include "NiagaraFunctionLibrary.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(
		Tracer,
		CollisionBox,
		FName(),
		GetActorLocation(),
		GetActorRotation(),
		EAttachLocation::KeepWorldPosition
		);
	}

	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}
	
	
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpuse, const FHitResult& Hit)
{
	if (Hit.GetActor()->ActorHasTag(TEXT("Player")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player"));
		SurfaceType = ESurfaceType::EST_Player;
		Multicast_OnHit(SurfaceType);
	}
	else if (Hit.GetActor()->ActorHasTag(TEXT("Wood")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Wood"));
		SurfaceType = ESurfaceType::EST_Wood;
		Multicast_OnHit(SurfaceType);
	}
	else if (Hit.GetActor()->ActorHasTag(TEXT("Stone")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Stone"));
		SurfaceType = ESurfaceType::EST_Stone;
		Multicast_OnHit(SurfaceType);
	}
	else if (Hit.GetActor()->ActorHasTag(TEXT("Metal"))) {
		UE_LOG(LogTemp, Warning, TEXT("Metal"));
		SurfaceType = ESurfaceType::EST_Metal;
		Multicast_OnHit(SurfaceType);
	}

	Destroyed();
}

void AProjectile::SpawnTrailSystem()
{
	if (Trail_System)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			Trail_System,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
}

void AProjectile::ExplodeDamage()
{
	APawn* FiringPawn = GetInstigator();
	if (FiringPawn && HasAuthority())
	{
		AController* FiringController = FiringPawn->GetController();
		if (FiringController)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,				// World context object
				Damage,				// Base Damage
				MinDamage,
				GetActorLocation(),	// Origin
				DamageInnerRadious,
				DamageOuterRadious,
				DamageFallOff,
				UDamageType::StaticClass(), // DamageTypeClass
				TArray<AActor*>(),  // Ignore Actors
				this,				// Damage Causer
				FiringController	// Instigator Controller
			);
		}

	}
}

void AProjectile::Multicast_OnHit_Implementation(ESurfaceType SurfaceTypeMC)
{
	switch (SurfaceTypeMC)
	{
	case ESurfaceType::EST_Player:
		if (ImpactParticles_Player) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles_Player, GetActorTransform());
		}
		if (ImpactSound_Player) {
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Player, GetActorLocation());
		}
		break;
	case ESurfaceType::EST_Stone:
		if (ImpactParticles_Stone) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles_Stone, GetActorTransform());
		}
		if (ImpactSound_Stone) {
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Stone, GetActorLocation());
		}
		break;
	case ESurfaceType::EST_Metal:
		if (ImpactParticles_Metal) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles_Metal, GetActorTransform());
		}
		if (ImpactSound_Metal) {
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Metal, GetActorLocation());
		}
		break;
	case ESurfaceType::EST_Wood:
		if (ImpactParticles_Wood) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles_Wood, GetActorTransform());
		}
		if (ImpactSound_Wood) {
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Wood, GetActorLocation());
		}
		break;
	}

}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::StartDestroyTimer()
{
	GetWorldTimerManager().SetTimer(
		DestroyTimer,
		this,
		&AProjectile::DestroyTimerFinished,
		DestroyTime
	);
}

void AProjectile::DestroyTimerFinished()
{
	if (ImpactParticles_Stone) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles_Stone, GetActorTransform());
	}
	if (ImpactSound_Stone) {
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound_Stone, GetActorLocation());
	}
	Destroy();
}

void AProjectile::Destroyed()
{
	Destroy();
}

