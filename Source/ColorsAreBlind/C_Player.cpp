// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "C_Player.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "string"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/EngineTypes.h"
#include "C_MovementComponent.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"




// Sets default values
// Other settings 1950f 1600f
AC_Player::AC_Player() : m_speedMovement(10.0f), m_speedRotation(0.1f), m_springArmLength(1250.0f), m_springArmOffset(1250.0f), m_cameraAngle(PI/4)
{
	// Init Values
	m_forwardRotator = FRotator(0.0f, 0.0f, 0.0f);
	
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Init Structure blueprint

	m_playerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = m_playerCollision;
	m_playerCollision->SetCollisionProfileName(TEXT("Pawn"));
	m_playerCollision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	RootComponent->bEditableWhenInherited = true;
	m_playerCollision->bEditableWhenInherited = true;

	m_playerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshPlayer"));
	m_playerMesh->SetupAttachment(RootComponent);
	m_playerMesh->bEditableWhenInherited = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshLoader(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (meshLoader.Succeeded()) {
		m_playerMesh->SetStaticMesh(meshLoader.Object);
		m_playerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_playerMesh->SetWorldScale3D(FVector(1.f));		
	}
	
	m_playerMovement = CreateDefaultSubobject<UC_MovementComponent>(TEXT("Movement"));
	m_playerMovement->UpdatedComponent = RootComponent;

	m_springArmCamera = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmCameraPlayer"));
	m_springArmCamera->SetupAttachment(RootComponent);
	m_springArmCamera->bEditableWhenInherited = false;
	m_springArmCamera->TargetArmLength = m_springArmLength;
	m_springArmCamera->bDoCollisionTest = false;
	m_springArmCamera->SocketOffset = FVector(0, 0, m_springArmOffset);
	m_springArmCamera->bEnableCameraLag = true;
	m_springArmCamera->CameraLagSpeed = 2.5f;

	FQuat rotation(FVector(0, 1, 0), m_cameraAngle);
	m_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraPlayer"));
	m_camera->SetupAttachment(m_springArmCamera);
	m_camera->SetRelativeRotation(rotation);
	m_camera->bEditableWhenInherited = false;

	Tags.Add("Player");
	

}

AC_Player::~AC_Player()
{

}

// Called when the game starts or when spawned
void AC_Player::BeginPlay()
{
	Super::BeginPlay();

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called every frame
void AC_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_currentVelocity.IsZero())
	{
		// FVector positionMovement = GetActorLocation() + (m_currentVelocity * DeltaTime);
		// SetActorLocation(positionMovement);

		m_forwardRotator = FMath::Lerp(m_forwardRotator, m_currentVelocity.Rotation(), m_speedRotation);
		m_playerMesh->SetWorldRotation(m_forwardRotator);
	}
}

// Called to bind functionality to input
void AC_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("MoveX", this, &AC_Player::MoveOnX);
	InputComponent->BindAxis("MoveY", this, &AC_Player::MoveOnY);
	InputComponent->BindAction("Run", IE_Pressed, this, &AC_Player::Run);
	InputComponent->BindAction("Run", IE_Released, this, &AC_Player::Walk);
}

void AC_Player::Walk()
{
	m_speedMovement = 5.0f;
}

void AC_Player::Run()
{
	m_speedMovement = 10.0f;
}

void AC_Player::MoveOnX(float value)
{
	if (m_playerMovement && (m_playerMovement->UpdatedComponent == RootComponent))
	{
		m_currentVelocity.X = FMath::Clamp(value, -1.0f, 1.0f) * 100.0f * m_speedMovement;
		m_playerMovement->AddInputVector(GetActorForwardVector() * value * m_speedMovement * 100.0f);
	}
	//m_currentVelocity.X = FMath::Clamp(value, -1.0f, 1.0f) * 100.0f * m_speedMovement;
}

void AC_Player::MoveOnY(float value)
{
	if (m_playerMovement && (m_playerMovement->UpdatedComponent == RootComponent))
	{
		m_currentVelocity.Y = FMath::Clamp(value, -1.0f, 1.0f) * 100.0f * m_speedMovement;
		m_playerMovement->AddInputVector(GetActorRightVector() * value * m_speedMovement * 100.0f);
	}
	//m_currentVelocity.Y = FMath::Clamp(value, -1.0f, 1.0f) * 100.0f * m_speedMovement;
}

float AC_Player::DegreesToRadians(float degrees)
{
	return  degrees / 180 * PI;
}

UPawnMovementComponent* AC_Player::GetMovementComponent() const
{
	return m_playerMovement;
}
