// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PropElement.h"

// Sets default values
AC_PropElement::AC_PropElement() 
{
	PrimaryActorTick.bCanEverTick = true;

	m_radiusPlacement = 0.0f;
	m_radiusPlacementGradient = 0.0f;
	m_pathToObject = FString("/Game/Models/Temple/temple.temple");
	m_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshProps"));
	m_mesh->SetupAttachment(RootComponent);
}

void AC_PropElement::setPropertiesProp(float radius, float radiusGradient, FString path, FVector center) 
{
	m_radiusPlacement = radius;
	m_radiusPlacementGradient = radius * 2/3 < radiusGradient ? radius * 2/3 : radiusGradient;
	
	m_pathToObject = path;
	m_center = center;
	 
	FStringAssetReference meshFinder(m_pathToObject);
	UStaticMesh* meshObject = Cast<UStaticMesh>(meshFinder.TryLoad());

	if (meshObject != nullptr)
	{
		m_mesh->SetStaticMesh(meshObject);;
		m_mesh->SetRelativeLocation(FVector(0.0f));
		m_mesh->SetWorldScale3D(FVector(1.f));

	}
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
