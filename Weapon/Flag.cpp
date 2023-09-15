// Fill out your copyright notice in the Description page of Project Settings.


#include "Flag.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "MyBlaster/Character/BlasterCharacter.h"
#include "MyBlaster/MyBlaster.h"

AFlag::AFlag()
{
	WeaponMeshed = GetWeaponMesh();
	PoleFlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));
	PoleFlagMesh->SetupAttachment(WeaponMeshed);
	PoleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PoleMesh"));
	PoleMesh->SetupAttachment(WeaponMeshed);
	GetAreaSphere()->SetupAttachment(WeaponMeshed);
	GetPickupWidget()->SetupAttachment(WeaponMeshed);
	WeaponMeshed->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponMeshed->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PoleFlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PoleFlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PoleMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PoleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFlag::BeginPlay()
{
	Super::BeginPlay();
	InitialTransform = GetActorTransform();
}

void AFlag::Dropped()
{
	BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : BlasterOwnerCharacter;
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMeshed->DetachFromComponent(DetachRules);
	//WeaponMeshed->AddImpulse(BlasterOwnerCharacter->GetActorForwardVector() * ImpulseDropAmount);
	//PoleFlagMesh->DetachFromComponent(DetachRules);
	//PoleMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	BlasterOwnerCharacter = nullptr;
	BlasterOwnerController = nullptr;
}

void AFlag::ResetFlag()
{
	ABlasterCharacter* FlagBearer = Cast<ABlasterCharacter>(GetOwner());
	if (FlagBearer)
	{
		FlagBearer->SetHoldingTheFlag(false);
		FlagBearer->SetOverlappingWeapon(nullptr);
		FlagBearer->UnCrouch();
	}

	if (!HasAuthority()) return; 

	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMeshed->DetachFromComponent(DetachRules);
	SetWeaponState(EWeaponState::EWS_Initial);

	SetOwner(nullptr);
	BlasterOwnerCharacter = nullptr;
	BlasterOwnerController = nullptr;

	SetActorTransform(InitialTransform);
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AFlag::OnEquipped()
{
	ShowPickupWidget(false);
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMeshed->SetSimulatePhysics(false);
	WeaponMeshed->SetEnableGravity(false);
	WeaponMeshed->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMeshed->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	//PoleFlagMesh->SetSimulatePhysics(false);
	//PoleFlagMesh->SetEnableGravity(false);
	PoleFlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PoleFlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	//PoleMesh->SetSimulatePhysics(false);
	//PoleMesh->SetEnableGravity(false);
	PoleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnableCustomDepth(false);
	
}

void AFlag::OnDropped()
{
	if (HasAuthority())
	{
		GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	GetWeaponMesh()->bDisableClothSimulation;
	WeaponMeshed->SetSimulatePhysics(true);
	WeaponMeshed->SetEnableGravity(true);
	WeaponMeshed->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMeshed->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMeshed->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMeshed->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	WeaponMeshed->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Ignore);
	WeaponMeshed->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMeshed->MarkRenderStateDirty();

	//PoleFlagMesh->SetSimulatePhysics(true);
	//PoleFlagMesh->SetEnableGravity(true);
	PoleFlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PoleFlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	PoleFlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	PoleFlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	PoleFlagMesh->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Ignore);
	PoleFlagMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	PoleFlagMesh->MarkRenderStateDirty();

	//PoleMesh->SetSimulatePhysics(true);
	//PoleMesh->SetEnableGravity(true);
	PoleMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PoleMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	PoleMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	PoleMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	PoleMesh->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Ignore);
	PoleMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	PoleMesh->MarkRenderStateDirty();

	EnableCustomDepth(true);
}

