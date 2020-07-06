// Fill out your copyright notice in the Description page of Project Settings.


#include "C_LandscapeGenerator.h"
#include "Landscape.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "TextureResource.h"
#include "Math/UnrealMathUtility.h"




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

	m_heightMap = new float[m_imageHeight * m_imageWidth];

	/*static ConstructorHelpers::FObjectFinder<UMaterialInstance> materialFinder(TEXT("/Game/Materials/Desert/Desert_MAT.Desert_MAT"));
	if (materialFinder.Succeeded())
	{
		
	}*/


}

AC_LandscapeGenerator::~AC_LandscapeGenerator()
{
	delete[] m_heightMap;
}

// Called when the game starts or when spawned
void AC_LandscapeGenerator::BeginPlay()
{
	Super::BeginPlay();
	generateLandscape();

	
	createSurfaceProps(FVector(0.0f, 0.0f, 0.0f), 50.0);
	
	

	
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
	
	unsigned int i = 0;
	unsigned int j = 0;

	
	FVector *normalsFaces = new FVector[m_imageWidth * m_imageHeight];
	
	
	
	float perlinX = 0.0f;
	float perlinY = 0.0f;
	for ( i = 0; i < m_imageHeight; i++)
	{
		for ( j = 0; j < m_imageWidth; j++)
		{
			float x = (i * m_triangleSize);
			float y = (j * m_triangleSize);
			float z = FMath::PerlinNoise2D(FVector2D(perlinX,perlinY)) * amplitude;
			m_heightMap[i * m_imageWidth + j ] = z;


			m_vertices.Add(FVector(x, y, z));

			normalsFaces[i * m_imageWidth + j] = FVector(0.0f, 0.0f, 0.0f);

			perlinY += detailsLevel;
		}
		perlinY = 0.0f;
		perlinX += detailsLevel;
	}
	for ( i = 0; i < m_imageHeight - 1; i++)
	{
		for ( j = 0; j < m_imageWidth - 1; j++)
		{
			float distance = m_triangleSize;
			float coordX = i * distance;
			float coordY = j * distance;
			float coordZ = m_heightMap[i * m_imageWidth + j];

			m_triangles.Add(j + (m_imageWidth * i));
			m_triangles.Add( (j+1) + (m_imageWidth * i) );
			m_triangles.Add( j + (m_imageWidth * (i + 1 )));
			
			FVector v1 = FVector(coordX, coordY + distance, m_heightMap[(i * m_imageWidth ) + (j + 1)]) - FVector(coordX, coordY, m_heightMap[i * m_imageWidth + j]);
			FVector v2 = FVector(coordX + distance, coordY, m_heightMap[ (i + 1) * m_imageWidth + j]) - FVector(coordX, coordY, m_heightMap[i * m_imageWidth + j]);
			FVector n1 = FVector::CrossProduct(v2, v1);

			normalsFaces[i * m_imageWidth + j] = normalsFaces[i * m_imageWidth + j] + n1;
			normalsFaces[i * m_imageWidth + (j + 1)] = normalsFaces[i * m_imageWidth + (j + 1)] + n1;
			normalsFaces[(i + 1) * m_imageWidth + j] = normalsFaces[(i + 1) * m_imageWidth + j] + n1;


			m_triangles.Add((j + 1) + (m_imageWidth * i));
			m_triangles.Add((j + 1) + (m_imageWidth * (i+1)));
			m_triangles.Add(j + (m_imageWidth * (i + 1)));
			FVector v3 = FVector(coordX, coordY + distance, m_heightMap[i * m_imageWidth + (j + 1)]) - FVector(coordX + distance, coordY + distance, m_heightMap[ (i + 1 ) * m_imageWidth + ( j + 1 )]);
			FVector v4 = FVector(coordX + distance, coordY, m_heightMap[( i + 1 ) * m_imageWidth + j]) - FVector(coordX + distance, coordY + distance, m_heightMap[(i + 1) * m_imageWidth + (j + 1)]);
			FVector n2 = FVector::CrossProduct(v3, v4);
			normalsFaces[i * m_imageWidth + (j + 1)] = normalsFaces[i * m_imageWidth + (j + 1)] + n2;
			normalsFaces[(i + 1) * m_imageWidth + j] = normalsFaces[(i + 1) * m_imageWidth + j]  + n2;
			normalsFaces[(i + 1) * m_imageWidth + (j+1)] = normalsFaces[(i + 1) * m_imageWidth + (j + 1)] + n2;
		}
	}



	for ( i = 0; i < m_imageHeight; i++)
	{
		for (j = 0; j < m_imageWidth; j++)
		{
			float x = i / static_cast<float>(m_imageHeight - 1);
			float y = j / static_cast<float>(m_imageWidth - 1);
			m_verticesTexture.Add(FVector2D(x, y));

			FVector vec = normalsFaces[i * m_imageWidth + j] ;
			vec.Normalize();
			m_normals.Add(vec);
		}
	}

	TArray<FProcMeshTangent> tangents;
	TArray<FColor> vertexColors;

	m_mesh->CreateMeshSection(0, m_vertices, m_triangles, m_normals, m_verticesTexture, vertexColors, tangents, true);

	FStringAssetReference materialFinder(TEXT("/Game/Materials/Desert/Desert_MAT.Desert_MAT"));
	UMaterialInstance* materialObject = Cast<UMaterialInstance>( materialFinder.TryLoad());
	m_materialDynamic = UMaterialInstanceDynamic::Create(materialObject, m_mesh);

	m_mesh->SetMaterial(0, m_materialDynamic);

	delete[] normalsFaces;


	
	/*static ConstructorHelpers::FObjectFinder<UStaticMesh> meshLoader(TEXT("/Game/Models/Ground.Ground"));
	if (meshLoader.Succeeded()) {
		m_staticMesh->SetStaticMesh(meshLoader.Object);
		m_staticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		m_staticMesh->SetWorldScale3D(FVector(1.f));
	}*/

	//static ConstructorHelpers::FObjectFinder<UStaticMesh> meshLoader(TEXT("/Game/pyramide.pyramide"));
	

	
	//m_mesh->SetMaterial(0, DynamicMaterial)

	/*UMaterial* material;
	static ConstructorHelpers::FObjectFinder<UMaterial> materialFinder(TEXT("Material'/Game/Materials/Desert/Desert_MAT.Desert_MAT'"));

	if (materialFinder.Object != NULL)
	{
		material = (UMaterial*)materialFinder.Object;

		UMaterialInstanceDynamic* materialToAffect = UMaterialInstanceDynamic::Create(material, this);
		m_mesh->SetMaterial(0, materialToAffect);
	}*/


	
}


void AC_LandscapeGenerator::createSurfaceProps(FVector centerPoint, float radius)
{
	UE_LOG(LogTemp, Warning, TEXT("recreate"));
	UE_LOG(LogTemp, Warning, TEXT("%f : %f"), centerPoint.X, centerPoint.Y);

	FVector correctedCenter = FVector(centerPoint.X, centerPoint.Y, centerPoint.Z);

	correctedCenter.X = centerPoint.X + radius > m_imageHeight ? m_imageHeight - radius - 1.0f : centerPoint.X;
	correctedCenter.X = centerPoint.X - radius < 0.0f ? radius + 1.0f : centerPoint.X;
	correctedCenter.Y = centerPoint.Y + radius > m_imageWidth ? m_imageWidth - radius - 1.0f : centerPoint.Y;
	correctedCenter.Y = centerPoint.Y - radius < 0.0f ? radius + 1.0f : centerPoint.Y;


	


	UE_LOG(LogTemp, Warning, TEXT("%f : %f"), correctedCenter.X, correctedCenter.Y);



	std::vector<float> zValues;
	for (float i = correctedCenter.X - radius; i < correctedCenter.X + radius; i+= 1.0f)
	{
		for (float j = correctedCenter.Y - radius; j < correctedCenter.Y + radius; j++)
		{
			if (FMath::Pow(i - correctedCenter.X, 2) + FMath::Pow(j - correctedCenter.Y, 2) <= radius)
			{
				zValues.push_back(centerPoint.Z);
			}
		}
	}
	float sum = 0.0f;
	float numberElements = 0.0f;
	for (float element : zValues)
	{
		sum += element;
		numberElements += 1.0f;
	}
	float averageValue = sum / numberElements;

	for (float i = correctedCenter.X - radius; i < correctedCenter.X + radius; i += 1.0f)
	{
		for (float j = correctedCenter.Y - radius; j < correctedCenter.Y + radius; j++)
		{
			
			
			float d = FMath::Pow(correctedCenter.X - i, 2) + FMath::Pow(correctedCenter.Y - j, 2);
			if ( d <= FMath::Pow(radius,2))
			{
				int index = i * m_imageWidth + j;
				if (d > FMath::Pow(radius - m_gradiantLevel, 2) && (radius - m_gradiantLevel > 0))
				{
					float coef = radius - FMath::TruncToFloat(FMath::Sqrt(d));
					float z = m_vertices[index].Z + (( averageValue - m_vertices[index].Z ) * (coef / m_gradiantLevel));
					//float z = (m_vertices[index].Z + averageValue) / 2;
					//float z = 0.0f;
					m_vertices[index] = FVector(m_vertices[index].X, m_vertices[index].Y, z);
					m_heightMap[index] = z;
				}
				else
				{
					m_vertices[index] = FVector(m_vertices[index].X, m_vertices[index].Y, averageValue);
					m_heightMap[index] = averageValue;
					
				}
				

			}
		}
	}

	/*

	*/


	FVector* normalsFaces = new FVector[m_imageWidth * m_imageHeight];
	
	for (unsigned int i = 0; i < m_imageHeight; i++)
	{
		for (unsigned int j = 0; j < m_imageWidth; j++)
		{
			normalsFaces[i * m_imageWidth + j] = FVector(0.0f, 0.0f, 0.0f);

		}
	}
	for (unsigned int i = 0; i < m_imageHeight - 1; i++)
	{
		for (unsigned int j = 0; j < m_imageWidth - 1; j++)
		{
			float distance = m_triangleSize;
			float coordX = i * distance;
			float coordY = j * distance;
			float coordZ = m_heightMap[i * m_imageWidth + j];

			FVector v1 = FVector(coordX, coordY + distance, m_heightMap[(i * m_imageWidth) + (j + 1)]) - FVector(coordX, coordY, m_heightMap[i * m_imageWidth + j]);
			FVector v2 = FVector(coordX + distance, coordY, m_heightMap[(i + 1) * m_imageWidth + j]) - FVector(coordX, coordY, m_heightMap[i * m_imageWidth + j]);
			FVector n1 = FVector::CrossProduct(v2, v1);
			normalsFaces[i * m_imageWidth + j] = normalsFaces[i * m_imageWidth + j] + n1;
			normalsFaces[i * m_imageWidth + (j + 1)] = normalsFaces[i * m_imageWidth + (j + 1)] + n1;
			normalsFaces[(i + 1) * m_imageWidth + j] = normalsFaces[(i + 1) * m_imageWidth + j] + n1;

			FVector v3 = FVector(coordX, coordY + distance, m_heightMap[i * m_imageWidth + (j + 1)]) - FVector(coordX + distance, coordY + distance, m_heightMap[(i + 1) * m_imageWidth + (j + 1)]);
			FVector v4 = FVector(coordX + distance, coordY, m_heightMap[(i + 1) * m_imageWidth + j]) - FVector(coordX + distance, coordY + distance, m_heightMap[(i + 1) * m_imageWidth + (j + 1)]);
			FVector n2 = FVector::CrossProduct(v3, v4);
			normalsFaces[i * m_imageWidth + (j + 1)] = normalsFaces[i * m_imageWidth + (j + 1)] + n2;
			normalsFaces[(i + 1) * m_imageWidth + j] = normalsFaces[(i + 1) * m_imageWidth + j] + n2;
			normalsFaces[(i + 1) * m_imageWidth + (j + 1)] = normalsFaces[(i + 1) * m_imageWidth + (j + 1)] + n2;
		}
	}



	for (unsigned int i = 0; i < m_imageHeight; i++)
	{
		for (unsigned int j = 0; j < m_imageWidth; j++)
		{

			FVector vec = normalsFaces[i * m_imageWidth + j];
			vec.Normalize();
			UE_LOG(LogTemp, Warning, TEXT("normal   %f   :   %f   :   %f"), vec.X, vec.Y, vec.Z);
			m_normals[i * m_imageWidth + j] = vec;
		}
	}

	TArray<FProcMeshTangent> tangents;
	TArray<FColor> vertexColors;

	m_mesh->ClearAllMeshSections();
	m_mesh->CreateMeshSection(0, m_vertices, m_triangles, m_normals, m_verticesTexture, vertexColors, tangents, true);

	m_mesh->ContainsPhysicsTriMeshData(true);

}



