// Fill out your copyright notice in the Description page of Project Settings.


#include "FruitGranade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Projectile.h"
#include "MyBlaster/MyBlaster.h"
#include "MyBlaster/Character/BlasterCharacter.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"

AFruitGranade::AFruitGranade()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Fuit Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
}

void AFruitGranade::BeginPlay()
{
	AActor::BeginPlay();

	if (!HasAuthority())
	{
		CollisionBox = GetCollisionBox();
		CollisionBox->OnComponentHit.AddDynamic(this, &AFruitGranade::OnHit);
	}

	int32 randFruit = FMath::RandRange(0, 6);
	if (FruitsArray.Num() > 0) {
		ProjectileMesh->SetStaticMesh(FruitsArray[randFruit]);
	}
	SpawnTrailSystem();
	StartDestroyTimer();

	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AFruitGranade::OnBounce);
}

void AFruitGranade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (bounceTimes <= 3) {
		bounceTimes++;
		if (BounceSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				BounceSound,
				GetActorLocation()
			);
		}
	}
	
}

void AFruitGranade::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpuse, const FHitResult& Hit)
{
	ABlasterCharacter* OwnerCharacter = Cast<ABlasterCharacter>(GetOwner());
	//UE_LOG(LogTemp, Error, TEXT("%s"), *OwnerCharacter->GetName());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->GetController();
		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
		}
	}

	OwnMulticast_OnHit(OtherActor);

}

void AFruitGranade::OwnMulticast_OnHit_Implementation(AActor* OtherActor)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
		}
	}
}

void AFruitGranade::Destroyed()
{
	Super::Destroyed();
}
