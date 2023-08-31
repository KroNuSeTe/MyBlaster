// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstance.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "RocketMovementComponent.h"



AProjectileRocket::AProjectileRocket()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));
	RocketMovementComponent->bRotationFollowsVelocity = true;
	RocketMovementComponent->SetIsReplicated(true);
}

void AProjectileRocket::Destroyed()
{

}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectileRocket::OnHit);
	}

	SpawnTrailSystem();

	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
			ProjectileLoop,
			GetRootComponent(),
			FName(),
			GetActorLocation(),
			EAttachLocation::KeepWorldPosition,
			false,
			1.f,
			1.f,
			0.f,
			LoopingSoundAttenuation,
			(USoundConcurrency*)nullptr,
			false
		);
	}
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpuse, const FHitResult& Hit)
{

	if (OtherActor == GetOwner())
	{
		return;
	}

	ExplodeDamage();

	StartDestroyTimer();

	if (Hit.GetActor()->ActorHasTag(TEXT("Player")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player"));
		SurfaceType = ESurfaceType::EST_Player;
	}
	else if (Hit.GetActor()->ActorHasTag(TEXT("Wood")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Wood"));
		SurfaceType = ESurfaceType::EST_Wood;
	}
	else if (Hit.GetActor()->ActorHasTag(TEXT("Stone")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Stone"));
		SurfaceType = ESurfaceType::EST_Stone;
	}
	else if (Hit.GetActor()->ActorHasTag(TEXT("Metal"))) {
		UE_LOG(LogTemp, Warning, TEXT("Metal"));
		SurfaceType = ESurfaceType::EST_Metal;
	}

	switch (SurfaceType)
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

	if (ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
	}

	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (TrailSystemComponent && TrailSystemComponent->GetSystemInstanceController())
	{
		TrailSystemComponent->Deactivate();
	}

	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}
}
