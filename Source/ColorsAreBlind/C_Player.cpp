// Fill out your copyright notice in the Description page of Project Settings.
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT(text), fstring)) 
#pragma once

#include "EngineGlobals.h"
#include "Engine/Engine.h"

#define print(text)                         if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::White, TEXT(text), false)
#define printc(channel, text)               if (GEngine) GEngine->AddOnScreenDebugMessage(channel, 1.5f, FColor::White, TEXT(text))
#define printf(format, ...)                 if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::White, FString::Printf(TEXT(format), ##__VA_ARGS__), false)
#define printcf(channel, format, ...)       if (GEngine) GEngine->AddOnScreenDebugMessage(channel, 1.5f, FColor::White, FString::Printf(TEXT(format), ##__VA_ARGS__))
#define printwarn(text)                     if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, TEXT(text), false)
#define printcwarn(channel, text)           if (GEngine) GEngine->AddOnScreenDebugMessage(channel, 1.5f, FColor::Yellow, TEXT(text))
#define printfwarn(format, ...)             if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, FString::Printf(TEXT(format), ##__VA_ARGS__), false)
#define printcfwarn(channel, format, ...)   if (GEngine) GEngine->AddOnScreenDebugMessage(channel, 1.5f, FColor::Yellow, FString::Printf(TEXT(format), ##__VA_ARGS__))
#define printerr(text)                      if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, TEXT(text), false)
#define printcerr(channel, text)            if (GEngine) GEngine->AddOnScreenDebugMessage(channel, 1.5f, FColor::Red, TEXT(text))
#define printferr(format, ...)              if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red, FString::Printf(TEXT(format), ##__VA_ARGS__), false)
#define printcferr(channel, format, ...)    if (GEngine) GEngine->AddOnScreenDebugMessage(channel, 1.5f, FColor::Red, FString::Printf(TEXT(format), ##__VA_ARGS__))
#define printfloat(variable)                if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, FString::Printf(TEXT(#variable ": %f"), variable), false)
#define printcfloat(channel, variable)      if (GEngine) GEngine->AddOnScreenDebugMessage(channel, 1.5f, FColor::Cyan, FString::Printf(TEXT(#variable ": %f"), variable))
#define printvector(variable)               if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, FString::Printf(TEXT(#variable ": %s"), *variable.ToCompactString()), false)
#define printcvector(channel, variable)     if (GEngine) GEngine->AddOnScreenDebugMessage(channel, 1.5f, FColor::Green, FString::Printf(TEXT(#variable ": %s"), *variable.ToCompactString()))
#define printquat(variable)                 if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, FString::Printf(TEXT(#variable ": %s"), *variable.ToCompactString()), false)
#define printcquat(channel, variable)       if (GEngine) GEngine->AddOnScreenDebugMessage(channel, 1.5f, FColor::Green, FString::Printf(TEXT(#variable ": %s"), *variable.ToCompactString()))



#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "string"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "C_Player.h"

// Sets default values
AC_Player::AC_Player() : m_speedMovement(10.0f)
{


	
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Init Blueprint
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent->bEditableWhenInherited = true;

	USceneComponent* sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BasePlayer"));
	sceneComponent->SetupAttachment(RootComponent);
	sceneComponent->bEditableWhenInherited = false;

	m_playerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshPlayer"));
	m_playerMesh->SetupAttachment(sceneComponent);

	m_playerMesh->bEditableWhenInherited = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshLoader(TEXT("/Engine/BasicShapes/Cube.Cube"));

	if (meshLoader.Succeeded()) {
		m_playerMesh->SetStaticMesh(meshLoader.Object);
		m_playerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_playerMesh->SetWorldScale3D(FVector(1.f));
	}
	// m_playerMesh->SetSimulatePhysics(true);
	// m_playerMesh->SetEnableGravity(true);

	m_playerCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PlayerCollision"));
	m_playerCollision->SetupAttachment(m_playerMesh); 
	m_playerCollision->SetBoxExtent(FVector(32.f, 32.f, 196.f));
	m_playerCollision->bDynamicObstacle = false;
	



	m_springArmCamera = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmCameraPlayer"));
	m_springArmCamera->SetupAttachment(sceneComponent);

	m_springArmCamera->bEditableWhenInherited = false;
	m_springArmCamera->TargetArmLength = 1950.0f;
	m_springArmCamera->bDoCollisionTest = false;
	m_springArmCamera->SocketOffset = FVector(0, 0, 1600);
	UE_LOG(LogTemp, Warning, TEXT("Testbaaa"));

	FQuat rotation(FVector(0, 1, 0), PI / 5);

	m_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraPlayer"));
	m_camera->SetupAttachment(m_springArmCamera);
	m_camera->SetRelativeRotation(rotation);

	m_camera->bEditableWhenInherited = false;

	// Init Values
	m_forwardRotator = FRotator(0.0f, 0.0f, 0.0f);
	m_speedRotation = 0.1f;

}

AC_Player::~AC_Player()
{

}

// Called when the game starts or when spawned
void AC_Player::BeginPlay()
{
	Super::BeginPlay();
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Entering the game"));
	UE_LOG(LogTemp, Warning, TEXT("I'm the player"));
	print("Initialization target camera");

	AutoPossessPlayer = EAutoReceiveInput::Player0;


}

// Called every frame
void AC_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_currentVelocity.IsZero())
	{
		FVector positionMovement = GetActorLocation() + (m_currentVelocity * DeltaTime);
		SetActorLocation(positionMovement);

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
}

void AC_Player::MoveOnX(float value)
{
	m_currentVelocity.X = FMath::Clamp(value, -1.0f, 1.0f) * 100.0f * m_speedMovement;
}

void AC_Player::MoveOnY(float value)
{
	m_currentVelocity.Y = FMath::Clamp(value, -1.0f, 1.0f) * 100.0f * m_speedMovement;
}

float AC_Player::DegreesToRadians(float degrees)
{
	return  degrees / 180 * PI;
}


