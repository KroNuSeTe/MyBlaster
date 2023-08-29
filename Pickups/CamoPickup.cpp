// Fill out your copyright notice in the Description page of Project Settings.


#include "CamoPickup.h"
#include "MyBlaster/Character/BlasterCharacter.h"
#include "MyBlaster/BlasterComponents/BuffComponent.h"

void ACamoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor);
	if (BlasterCharacter)
	{
		UBuffComponent* Buff = BlasterCharacter->GetBuff();
		if (Buff)
		{
			Buff->BuffCamo(CamoBuffTime);
		}
	}
	Destroy();
}