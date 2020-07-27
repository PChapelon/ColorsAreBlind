// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PropElement.h"

// Sets default values
AC_PropElement::AC_PropElement() 
{
	PrimaryActorTick.bCanEverTick = true;

	m_radiusPlacement = 0.0f;
	m_radiusPlacementGradient = 0.0f;
	m_pathToObject = FString("/Game/Models/Temple/temple.temple");
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	m_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshProps"));
	m_mesh->SetupAttachment(RootComponent);
}

void AC_PropElement::setPropertiesProp(float radius, float radiusGradient, FString path, FVector center, unsigned int heightImage, unsigned int widthImage, FRandomStream* random, bool flat, FVector scale )
{

	m_radiusPlacement = radius;
	m_radiusPlacementGradient = radius * 2/3 < radiusGradient ? radius * 2/3 : radiusGradient;
	
	m_pathToObject = path;
	m_center = center;

	m_flatGround = flat;

	FStringAssetReference meshFinder(m_pathToObject);
	UStaticMesh* meshObject = Cast<UStaticMesh>(meshFinder.TryLoad());

	if (meshObject != nullptr)
	{
		m_mesh->SetStaticMesh(meshObject);
		m_mesh->SetRelativeLocation(FVector(0.0f));
		
		FRotator rotatorRandom;		
		rotatorRandom.Yaw = random->FRandRange(0.0f, 360.0f);
		m_mesh->SetRelativeRotation(rotatorRandom);
		if(!m_flatGround)
			m_mesh->SetRelativeRotation(FQuat(FVector(0,1,0), PI/2));
		m_mesh->SetWorldScale3D(scale);
		m_mesh->SetRelativeScale3D(scale);
		SetActorScale3D(scale);
	}

	FVector correctedCenter;
	if (m_center.X + radius >= heightImage || m_center.X - radius < 0.0f)
	{
		if (m_center.X + radius >= heightImage)
		{
			correctedCenter.X = heightImage - radius - 1.0f;
		}
		else
		{
			correctedCenter.X = radius + 1.0f;
		}
	}
	else
	{
		correctedCenter.X = m_center.X;
	}

	if (m_center.Y + radius >= widthImage || m_center.Y - radius < 0.0f)
	{
		if (m_center.Y + radius >= widthImage)
		{
			correctedCenter.Y = widthImage - radius - 1.0f;
		}
		else
		{
			correctedCenter.Y = radius + 1.0f;
		}
	}
	else
	{
		correctedCenter.Y = m_center.Y;
	}


	setCenter(correctedCenter);
}

// Called when the game starts or when spawned
void AC_PropElement::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AC_PropElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AC_PropElement::setRotation(FRotator rotation)
{
	SetActorRotation(rotation);
}

bool AC_PropElement::isFlatGround()
{
	return m_flatGround;
}

float AC_PropElement::getRadiusPlacement()
{
	return m_radiusPlacement;
}


float AC_PropElement::getRadiusPlacementGradient()
{
	return m_radiusPlacementGradient;
}

FVector AC_PropElement::getCenterPlacement()
{
	return m_center;
}

void AC_PropElement::setCenter(FVector v)
{
	m_center = v;
}

void AC_PropElement::setCenterZ(float z)
{
	m_center.Z = z;
}

float AC_PropElement::getCenterX()
{
	return m_center.X;
}

float AC_PropElement::getCenterY()
{
	return m_center.Y;
}


FString AC_PropElement::getModelPath()
{
	return m_pathToObject;
}
