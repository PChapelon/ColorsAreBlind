// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "C_Player.generated.h"

UCLASS()
class COLORSAREBLIND_API AC_Player : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AC_Player();
	~AC_Player();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float m_speedMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float m_angleRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float m_speedRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
		FVector NewLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
		FQuat NewRotation;





protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float DegreesToRadians(float degrees);
	FVector m_currentVelocity;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveOnX(float value);

	void MovePlayerForward();

	void MoveOnY(float value);

};
