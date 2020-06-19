// Fill out your copyright notice in the Description page of Project Settings.
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT(text), fstring)) 
#include "Components/SphereComponent.h"
#include "string"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "C_Player.h"

// Sets default values
AC_Player::AC_Player() : m_speedMovement(10.0f)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	USceneComponent* sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BasePlayer"));
	sceneComponent->SetupAttachment(RootComponent);

	m_playerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshPlayer"));
	m_playerMesh->SetupAttachment(sceneComponent);

	m_playerMesh->bEditableWhenInherited = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshLoader(TEXT("/Engine/BasicShapes/Cone.Cone"));

	if (meshLoader.Succeeded()) {
		m_playerMesh->SetStaticMesh(meshLoader.Object);
		m_playerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_playerMesh ->SetWorldScale3D(FVector(1.f));
	}




	m_springArmCamera = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmCameraPlayer"));
	m_springArmCamera->SetupAttachment(sceneComponent);

	m_springArmCamera->bEditableWhenInherited = false;
	m_springArmCamera->TargetArmLength = 1950.0f;
	m_springArmCamera->bDoCollisionTest = false;
	//m_springArmCamera->SetMobility(EComponentMobility::Movable);
	m_springArmCamera->SocketOffset = FVector(0, 0, 1600);
	UE_LOG(LogTemp, Warning, TEXT("Testbaaa"));

	FQuat rotation(FVector(0, 1, 0), PI / 5);

	m_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraPlayer"));
	m_camera->SetupAttachment(m_springArmCamera);
	m_camera->SetRelativeRotation(rotation);

	m_camera->bEditableWhenInherited = false;


	

	//m_camera->SetWorldLocationAndRotation(FVector(0, 0, 0), rotation);




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


