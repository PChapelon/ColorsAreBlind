// Fill out your copyright notice in the Description page of Project Settings.


#include "C_LandscapeGenerator.h"
#include "Landscape.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "TextureResource.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/StreamableManager.h"
#include "EngineUtils.h"
#include "C_PropElement.h"
#include "Math/RandomStream.h"
#include <map>





// Sets default values
AC_LandscapeGenerator::AC_LandscapeGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Init data 
	m_worldDatas.Add(WorldType::DESERT, { "Desert", 100, 500 });
	m_worldDatas.Add(WorldType::BANQUISE, { "Banquise", 20, 50 });
	m_worldDatas.Add(WorldType::FORET, { "Foret", 50, 100 });

	m_mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = m_mesh;
	m_mesh->bUseAsyncCooking = true;

	m_heightMap = new float[m_imageHeight * m_imageWidth];
	m_seed = FMath::RandRange(0, 99999999);
	UE_LOG(LogTemp, Warning, TEXT("%i   seed man"), m_seed);
	


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

	FTransform SpawnLocation(FQuat(FRotator(0.0f)), FVector(0.0f), FVector(0.0f));
	
	m_props.Add(GetWorld()->SpawnActor<AC_PropElement>(AC_PropElement::StaticClass(), SpawnLocation));
	m_spawnProp = m_props[0];
	m_spawnProp->setPropertiesProp(40.0f, 10.0f, FString("/Game/Models/Desert/temple.temple"), FVector(0.0f));

	m_props.Add(GetWorld()->SpawnActor<AC_PropElement>(AC_PropElement::StaticClass(), SpawnLocation));
	m_spawnProp = m_props[1];
	m_spawnProp->setPropertiesProp(10.0f, 5.0f, FString("/Engine/BasicShapes/Cube.Cube"), FVector(250.0f,150.0f,0.0f));

	m_props.Add(GetWorld()->SpawnActor<AC_PropElement>(AC_PropElement::StaticClass(), SpawnLocation));
	m_spawnProp = m_props[2];
	m_spawnProp->setPropertiesProp(15.0f, 7.0f, FString("/Engine/BasicShapes/Cube.Cube"), FVector(100.0f, 0.0f, 0.0f));

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
	FRandomStream random;

	random.Initialize(m_seed);

	if (!m_seedManually)
	{
		m_seed = FMath::RandRange(0, 99999999);
		random.Initialize(m_seed);
		
		m_worldType = static_cast<WorldType>(random.FRandRange(0, static_cast<float>(WorldType::last)));
		//auto color_name = magic_enum::enum_name(color);


		switch (m_worldType)
		{
		case WorldType::DESERT:
			UE_LOG(LogTemp, Warning, TEXT("World desert"));
			break;
		case WorldType::BANQUISE:
			UE_LOG(LogTemp, Warning, TEXT("World banquise"));
			break;
		case WorldType::FORET:
			UE_LOG(LogTemp, Warning, TEXT("World foret"));
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("default"));
			break;
		}


		m_dataTemp = *m_worldDatas.Find(m_worldType);
		

		detailsLevel = random.FRandRange(0.01f, 0.09f);
		amplitude = random.FRandRange(50.0f, 800.0f);
		
		UE_LOG(LogTemp, Warning, TEXT("%i   seed auto  amplitude %f     detailsLevel   %f"), m_seed, amplitude, detailsLevel);
	}


	


	float perlinY = random.FRandRange(0.0f, 50.0f);
	float perlinX = random.FRandRange(0.0f, 50.0f);

	UE_LOG(LogTemp, Warning, TEXT("%f  "), perlinY);
	UE_LOG(LogTemp, Warning, TEXT("%f  "), perlinX);
	
	float initPerlinY = perlinY;
	
	//float perlinX = 0.0f;
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
		perlinY = initPerlinY;
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
}


void AC_LandscapeGenerator::createSurfaceProps(FVector centerPoint, float radius )
{
	for (AC_PropElement* elt : m_props)
	{

		//UE_LOG(LogTemp, Warning, TEXT(" %s   :  %f  %f  %f"), *elt->getModelPath(), elt->getCenterPlacement().X, elt->getCenterPlacement().Y, elt->getCenterPlacement().Z);
		FVector correctedCenter(0.0f);
		
		
		
		centerPoint = elt->getCenterPlacement();

		if (centerPoint.X > m_imageHeight || centerPoint.Y > m_imageWidth || centerPoint.X < 0 || centerPoint.Y < 0 && elt != nullptr)
		{
			elt->Destroy();
			continue;
		}

		radius = elt->getRadiusPlacement();
		float gradientLevel = elt->getRadiusPlacementGradient();


		
	

		correctedCenter.X = centerPoint.X + radius > m_imageHeight ? m_imageHeight - radius - 1.0f : centerPoint.X;
		correctedCenter.X = centerPoint.X - radius < 0.0f ? radius + 1.0f : centerPoint.X;
		correctedCenter.Y = centerPoint.Y + radius > m_imageWidth ? m_imageWidth - radius - 1.0f : centerPoint.Y;
		correctedCenter.Y = centerPoint.Y - radius < 0.0f ? radius + 1.0f : centerPoint.Y;

		elt->setCenter(correctedCenter);
		UE_LOG(LogTemp, Warning, TEXT(" %s   :  %f  %f  %f"), *elt->getModelPath(), elt->getCenterPlacement().X, elt->getCenterPlacement().Y, elt->getCenterPlacement().Z);


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

					if (d > FMath::Pow(radius - gradientLevel, 2) && (radius - gradientLevel > 0))
					{
						float coef = radius - FMath::TruncToFloat(FMath::Sqrt(d));
						float z = m_vertices[index].Z + (( averageValue - m_vertices[index].Z ) * (coef / gradientLevel));
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
		int centerZ = elt->getCenterX() * m_imageWidth + elt->getCenterY();
		FVector v(elt->getCenterPlacement().X * m_triangleSize , elt->getCenterPlacement().Y * m_triangleSize, m_heightMap[centerZ]);
		elt->SetActorLocation(v + GetActorLocation());

		//UE_LOG(LogTemp, Warning, TEXT(" %s   :  %f  %f  %f"), *elt->getModelPath(), v.X, v.Y, m_heightMap[centerZ]);
	}

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
			m_normals[i * m_imageWidth + j] = vec;
		}
	}

	TArray<FProcMeshTangent> tangents;
	TArray<FColor> vertexColors;
	m_mesh->ClearAllMeshSections();
	m_mesh->CreateMeshSection(0, m_vertices, m_triangles, m_normals, m_verticesTexture, vertexColors, tangents, true);


	
}



