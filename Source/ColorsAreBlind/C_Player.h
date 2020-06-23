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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float m_speedMovement;
		float m_speedRotation;
		float m_speedGravity;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		class USpringArmComponent* m_springArmCamera;
		class UCameraComponent* m_camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Model")
		class UStaticMeshComponent* m_playerMesh;
		class UBoxComponent* m_playerCollision;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	float DegreesToRadians(float degrees);
	FVector m_currentVelocity;
	FRotator m_forwardRotator;

public:	
	// Sets default values for this pawn's properties
	AC_Player();
	~AC_Player();
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveOnX(float value);

	void MovePlayerForward();

	void MoveOnY(float value);

};
