// Fill out your copyright notice in the Description page of Project Settings.


#include "C_LandscapeGenerator.h"
#include "Landscape.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "TextureResource.h"



#define printfloat(variable)                if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Cyan, FString::Printf(TEXT(#variable ": %f"), variable), false)
#define printcfloat(channel, variable)      if (GEngine) GEngine->AddOnScreenDebugMessage(channel, 1.5f, FColor::Cyan, FString::Printf(TEXT(#variable ": %f"), variable))

// Sets default values
AC_LandscapeGenerator::AC_LandscapeGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = m_mesh;
	m_mesh->bUseAsyncCooking = true;


	

}

// Called when the game starts or when spawned
void AC_LandscapeGenerator::BeginPlay()
{
	Super::BeginPlay();
	generateLandscape();
	

	
}

// Called every frame
void AC_LandscapeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AC_LandscapeGenerator::PostActorCreated()
{
	Super::PostActorCreated();
}

void AC_LandscapeGenerator::PostLoad()
{
	Super::PostLoad();

}

void AC_LandscapeGenerator::generateLandscape()
{
	unsigned int triangleSize = 100;
	unsigned int imageWidth = 51;
	unsigned int imageHeight = 51;
	unsigned int i = 0;
	unsigned int j = 0;
	TArray<FVector> vertices;
	TArray<FVector> normals;
	for ( i = 0; i < imageHeight; i++)
	{
		for ( j = 0; j < imageWidth; j++)
		{
			float x = (i * triangleSize);
			float y = (j * triangleSize);
			vertices.Add(FVector(x, y, 0));
			normals.Add(FVector(0, 0, 1));

		}
	}
	TArray<int32> triangles;
	for ( i = 0; i < imageHeight - 1; i++)
	{
		for ( j = 0; j < imageWidth - 1; j++)
		{
			triangles.Add(j + (imageWidth * i));
			triangles.Add( (j+1) + (imageWidth * i) );
			triangles.Add( j + (imageWidth * (i + 1 )));
			triangles.Add((j + 1) + (imageWidth * i));
			triangles.Add((j + 1) + (imageWidth * (i+1)));
			triangles.Add(j + (imageWidth * (i + 1)));
		}
	}
	TArray<FVector2D> verticesTexture;
	for ( i = 0; i < imageHeight; i++)
	{
		for ( j = 0; j < imageWidth; j++)
		{
			float x = i / static_cast<float>(imageHeight - 1);
			float y = j / static_cast<float>(imageWidth - 1);
			verticesTexture.Add(FVector2D( x , y));
		}
	}
	TArray<FProcMeshTangent> tangents;
	TArray<FColor> vertexColors;
	m_mesh->CreateMeshSection(0, vertices, triangles, normals, verticesTexture, vertexColors, tangents, true);
	m_mesh->ContainsPhysicsTriMeshData(true);
	
	



	//static ConstructorHelpers::FObjectFinder<UMaterial> materialFinder(TEXT("Material'/Game/Materials/Desert/Desert_MAT.Desert_MAT'"));

	/*static ConstructorHelpers::FObjectFinder<UStaticMesh> meshLoader(TEXT("/Game/Models/Ground.Ground"));
	if (meshLoader.Succeeded()) {
		m_staticMesh->SetStaticMesh(meshLoader.Object);
		m_staticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_staticMesh->SetWorldScale3D(FVector(1.f));
	}*/

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> meshLoader(TEXT("/Game/pyramide.pyramide"));
	


	//FProceduralMeshComponentDetails::ClickedOnConvertToStaticMesh();

	
	//m_mesh->SetMaterial(0, DynamicMaterial)

	/*UMaterial* material;
	static ConstructorHelpers::FObjectFinder<UMaterial> materialFinder(TEXT("Material'/Game/Materials/Desert/Desert_MAT.Desert_MAT'"));

	if (materialFinder.Object != NULL)
	{
		material = (UMaterial*)materialFinder.Object;

		UMaterialInstanceDynamic* materialToAffect = UMaterialInstanceDynamic::Create(material, this);
		m_mesh->SetMaterial(0, materialToAffect);
	}*/



	
	//static ConstructorHelpers::FObjectFinder<UTexture2D> BarFillObj(TEXT("/Game/UI/HUD/BarFill"));
	//static FProceduralMeshComponentDetails::ClickedOnConvertToStaticMesh();
	
}

