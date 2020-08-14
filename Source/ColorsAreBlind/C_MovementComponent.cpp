// Fill out your copyright notice in the Description page of Project Settings.



#include "C_MovementComponent.h"

void UC_MovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	const float GRAVITY_COEF = 9.81;

	if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	{
		return;
	}


	FVector direction = ConsumeInputVector() * DeltaTime;
	FVector gravity = FVector(0.0f, 0.0f, -1.0f) * DeltaTime * 75.0f * GRAVITY_COEF;

	FHitResult hitGround;

	SafeMoveUpdatedComponent(gravity, UpdatedComponent->GetComponentRotation(), true, hitGround);

	if (hitGround.IsValidBlockingHit())
	{
		SlideAlongSurface(gravity, 1.0f - hitGround.Time, FVector(0.0f, 0.0f, 1.0f), hitGround);
	}

	if (!direction.IsNearlyZero())
	{
		FHitResult hit;
		

		SafeMoveUpdatedComponent(direction, UpdatedComponent->GetComponentRotation(), true, hit);

		if (hit.IsValidBlockingHit())
		{
			SlideAlongSurface(direction, 1.0f - hit.Time, hit.Normal, hit);
		}
	}
}