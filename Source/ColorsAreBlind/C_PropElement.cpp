// Fill out your copyright notice in the Description page of Project Settings.


#include "C_PropElement.h"
#include "Components/CapsuleComponent.h"
#include "C_LandscapeGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
AC_PropElement::AC_PropElement() 
{
	PrimaryActorTick.bCanEverTick = true;

	m_radiusPlacement = 0.0f;
	m_radiusPlacementGradient = 0.0f;
	m_pathToObject = FString("/Game/Models/Temple/temple.temple");
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	m_mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshProps"));
	m_mesh->SetRelativeScale3D(FVector(1.0f));
	m_mesh->SetWorldScale3D(FVector(1.0f));
	m_mesh->SetupAttachment(RootComponent);

	m_trigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerProps"));
	m_trigger->SetCollisionProfileName(TEXT("TriggerCollision"));
	m_trigger->SetRelativeScale3D(FVector(1.0f));
	m_trigger->SetWorldScale3D(FVector(1.0f));
	m_trigger->SetRelativeScale3D(FVector(1.0f));
	m_trigger->OnComponentBeginOverlap.AddDynamic(this, &AC_PropElement::OnOverlapBegin);
	m_trigger->OnComponentEndOverlap.AddDynamic(this, &AC_PropElement::OnOverlapEnd);

	m_trigger->InitCapsuleSize(1.0f, 1.0f);
	m_trigger->SetupAttachment(RootComponent);
	

	m_particlesCompleted = CreateDefaultSubobject<UParticleSystemComponent>("ParticlesSystem");
	m_particlesCompleted->SetupAttachment(m_mesh);
	m_particlesCompleted->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));

	static ConstructorHelpers::FObjectFinder<USoundCue> soundLoader(TEXT("/Game/Sounds/SoundCompletedCue.SoundCompletedCue"));
	if (soundLoader.Succeeded())
	{
		m_audioCompleted = soundLoader.Object;
		m_audioCompletedComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
		m_audioCompletedComponent->SetupAttachment(RootComponent);

	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> particlesLoader(TEXT("/Game/Particles/P_SparksMedium.P_SparksMedium"));
	if (particlesLoader.Succeeded()) {
		m_particlesToAffectMedium = particlesLoader.Object;
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> particlesLoaderMain(TEXT("/Game/Particles/P_SparksMain.P_SparksMain"));
	if (particlesLoaderMain.Succeeded()) {
		m_particlesToAffectMain = particlesLoaderMain.Object;
	}
	//m_particlesCompleted->Template = m_particlesToAffect;
	m_particlesCompleted->Deactivate();

	
}

void AC_PropElement::setPropertiesProp(float radius, float radiusTrigger, float radiusGradient, FString path, FVector center, unsigned int heightImage, unsigned int widthImage, FRandomStream* random, bool flat, bool mainModel, FVector scale )
{

	m_radiusPlacement = radius;


	m_radiusPlacementGradient = radius * 2/3 < radiusGradient ? radius * 2/3 : radiusGradient;

	m_radiusTrigger = radiusTrigger;
	
	m_pathToObject = path;
	m_center = center;

	m_flatGround = flat;

	m_trigger->SetVisibility(true);
	
	m_trigger->SetCapsuleRadius(m_radiusTrigger);
	m_trigger->SetCapsuleHalfHeight(m_radiusTrigger);
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
		FVector scale3D(random->FRandRange(0.85f, 1.15f));
		m_mesh->SetWorldScale3D(scale3D);
		m_mesh->SetRelativeScale3D(scale3D);
		SetActorScale3D(scale3D);
	}

	m_dynamicMaterial = UMaterialInstanceDynamic::Create(m_mesh->GetMaterial(0), this);
	m_mesh->SetMaterial(0, m_dynamicMaterial);

	
	if (!m_flatGround)
	{
		m_dynamicMaterial->SetScalarParameterValue(FName(TEXT("desaturation")), 0.0f);
		m_isCompleted = true;
	}
	else
	{
		if (mainModel)
			m_particlesCompleted->Template = m_particlesToAffectMain;
		else
			m_particlesCompleted->Template = m_particlesToAffectMedium;
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
	if (m_audioCompletedComponent && m_audioCompleted)
	{
		m_audioCompletedComponent->PitchMultiplier = 0.5f;
		m_audioCompletedComponent->SetSound(m_audioCompleted);
	}
}

// Called every frame
void AC_PropElement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!m_isCompleted && m_isInside )
	{
		float f;
		m_dynamicMaterial->GetScalarParameterValue(FName(TEXT("desaturation")), f);

		f -= 0.01f;
		m_dynamicMaterial->SetScalarParameterValue(FName(TEXT("desaturation")), f);


		if (f <= 0.0f)
		{
			m_isCompleted = true;
			TArray<AActor*> actorsFound;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AC_LandscapeGenerator::StaticClass(), actorsFound);
			AC_LandscapeGenerator* landscapeTemporary = (AC_LandscapeGenerator*) actorsFound[0];
			landscapeTemporary->increaseMaterialSaturation();
			landscapeTemporary->increaseCompletedTarget();
			m_audioCompletedComponent->Play();
			m_particlesCompleted->Activate();

		}
	}

}


void AC_PropElement::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
//void AC_PropElement::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{

	if (OtherActor && (OtherActor != this) )
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			m_isInside = true;

		}
	}
}

void AC_PropElement::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
//void AC_PropElement::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)

{

	if (OtherActor && (OtherActor != this) )
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			m_isInside = false;

		}

	}
}


void AC_PropElement::setRotation(FRotator rotation)
{
	SetActorRotation(rotation);
}

bool AC_PropElement::isFlatGround()
{
	return m_flatGround;
}

bool AC_PropElement::isCompleted()
{
	return m_isCompleted;
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
