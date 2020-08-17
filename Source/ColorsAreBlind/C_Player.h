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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float m_speedRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float m_speedGravity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		float m_speedZoom = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float m_anglePlayerCamera ;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float m_springArmLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float m_springArmOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		float m_cameraAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
		class USpringArmComponent* m_springArmCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
		class UCameraComponent* m_camera;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
		USceneComponent* m_meshContainer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
		class USkeletalMeshComponent* m_playerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
		class UC_MovementComponent* m_playerMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
		class UParticleSystemComponent* m_particlesPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure")
		class UBoxComponent* m_playerCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float m_animationDelta = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		float m_speedAnimation = 0.3f;

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

	void Walk();

	void Run();

	void MoveOnX(float value);

	void MovePlayerForward();

	void MoveOnY(float value);

	void ZoomOnPlayer(float value);

	virtual UPawnMovementComponent* GetMovementComponent() const override;

};
