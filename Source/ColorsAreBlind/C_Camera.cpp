// Fill out your copyright notice in the Description page of Project Settings.
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green,text)
#define printFString(text, fstring) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, FString::Printf(TEXT(text), fstring)) 
#include "C_Player.h"
#include "Kismet/GameplayStatics.h"
#include "C_Camera.h"

// Sets default values
AC_Camera::AC_Camera() : m_cameraTarget(nullptr)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AC_Camera::BeginPlay()
{
	Super::BeginPlay();
	print("Initialization target camera");
	TSubclassOf<AC_Player> playerToFind;
	playerToFind = AC_Player::StaticClass();
	TArray<AActor*> playerFound;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), playerToFind, playerFound);

	if (playerFound.Max() > 0)
	{
		m_cameraTarget = playerFound.Pop();
		SetActorLocation(m_cameraTarget->GetActorLocation()  + FVector(0, 0, 20));

	}
}

// Called every frame
void AC_Camera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	check(m_cameraTarget != nullptr);
	SetActorLocation(m_cameraTarget->GetActorLocation() + FVector(0, 0, 20));
}

// Called to bind functionality to input
void AC_Camera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

