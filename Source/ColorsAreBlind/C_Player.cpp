// Fill out your copyright notice in the Description page of Project Settings.
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT(text), fstring)) 
#include "Components/SphereComponent.h"
#include "string"
#include "Camera/CameraComponent.h"

#include "C_Player.h"

// Sets default values
AC_Player::AC_Player(): m_speedMovement(10.0f)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Create a dummy root component we can attach things to.
	/*RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MyRootComponent"));
	// Create a camera and a visible object
	OurVisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OurVisibleComponent"));
	// Attach our camera and visible object to our root component. Offset and rotate the camera.

	OurVisibleComponent->SetupAttachment(RootComponent);
	OurVisibleComponent->bEditableWhenInherited = true;*/

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




	NewLocation.X = 0;
	NewLocation.Y = 1;
	NewLocation.Z = 100;

	NewRotation = FQuat::Identity;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	
}

// Called every frame
void AC_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!m_currentVelocity.IsZero())
	{
		FVector NewPosition = GetActorLocation() + (m_currentVelocity * DeltaTime);
		SetActorLocation(NewPosition);


	}



	/*NewLocation = FVector(0, 1, 0) * m_speedMovement;
	NewRotation = FQuat(FVector(0, 0, 1), DegreesToRadians(m_angleRotation) * m_speedRotation);

	AddActorLocalRotation(NewRotation);
	//The rotation doesn't impact the movement of the player | rotate the camera

	AddActorLocalOffset(NewLocation);*/
	//SetActorLocationAndRotation(NewLocation, NewRotation, false, 0, ETeleportType::None);

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


