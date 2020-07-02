// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "C_MovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class COLORSAREBLIND_API UC_MovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	

public:

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
