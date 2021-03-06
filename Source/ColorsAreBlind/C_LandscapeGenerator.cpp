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
#include "Math/Color.h"
#include "Misc/Paths.h"
#include "HAL/FileManagerGeneric.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"





// Sets default values
AC_LandscapeGenerator::AC_LandscapeGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_worldStringToEnum.Add(FString("DESERT"), WorldType::DESERT);
	m_worldStringToEnum.Add(FString("BANQUISE"), WorldType::BANQUISE);
	m_worldStringToEnum.Add(FString("FORET"), WorldType::FORET);



	//Init data 
	m_worldDatas.Add(WorldType::DESERT, { FString("DESERT"), FString("Desert"), 450.0f, 500.0f, 0.005f, 0.01f, 0.2f, 0.5f, 0.2f, FWorldModel(), {}, {} });
	m_worldDatas.Add(WorldType::BANQUISE, { FString("BANQUISE"), FString("Banquise"), 50.0f, 120.0f, 0.005f, 0.01f, 1.0f, 0.5f, 0.2f, FWorldModel(), {}, {} });
	m_worldDatas.Add(WorldType::FORET, { FString("FORET"), FString("Foret"), 10.0f, 200.0f, 0.005f, 0.01f, 1.0f, 0.5f, 0.2f, FWorldModel(), {}, {} });

	
	
	FString FullFilePath(FPaths::ProjectContentDir() + "Models/");
	TArray<FString> FolderWorld;
	FolderWorld.Empty();

	FString PathWorld = FullFilePath + "*";

	FPaths::NormalizeDirectoryName(PathWorld);
	FFileManagerGeneric::Get().FindFiles(FolderWorld, *PathWorld, true, true);
	for (int i = 0; i < FolderWorld.Num(); i++)
	{
		FString PathCurrentWorld = FullFilePath + FolderWorld[i];
		if (FolderWorld[i] == "PLAYER")
		{
			continue;
		}
		FString PathModelDirectory = PathCurrentWorld + "/*";
		FPaths::NormalizeDirectoryName(PathModelDirectory);
		TArray<FString> FolderDirectoryModel;
		FolderDirectoryModel.Empty();
		FFileManagerGeneric::Get().FindFiles(FolderDirectoryModel, *PathModelDirectory, true, true);

		for (int j = 0; j < FolderDirectoryModel.Num(); j++)
		{
			FString PathCurrentDirectoryModel = PathCurrentWorld + "/"+ FolderDirectoryModel[j];
			TArray<FString> FilesModel;
			FilesModel.Empty();

			FString PathSearchCurrentModelDirectory = PathCurrentDirectoryModel + "/*";
			FPaths::NormalizeDirectoryName(PathSearchCurrentModelDirectory);
			FFileManagerGeneric::Get().FindFiles(FilesModel, *PathSearchCurrentModelDirectory, true, true);

			WorldType world = *m_worldStringToEnum.Find(FolderWorld[i]);
			FWorldDataStruct* temporaryDataStruct = m_worldDatas.Find(world);
			
			for (int k = 0; k < FilesModel.Num(); k++)
			{
				FilesModel[k].RemoveFromEnd(".uasset");
				FString pathToModel = PathCurrentDirectoryModel + "/" + FilesModel[k] + "." + FilesModel[k];
				pathToModel.RemoveFromStart(FPaths::ProjectContentDir());
				pathToModel.InsertAt(0, "/Game/");
				
				if (FolderDirectoryModel[j] == "Main")
				{
					temporaryDataStruct->mainModel = { FilesModel[k], pathToModel, true, 40.0f, 10.0f };

				}
				else if (FolderDirectoryModel[j] == "SubModelMedium")
				{
					temporaryDataStruct->subModelsMedium.Add({ FilesModel[k], pathToModel, true, 20.0f, 14.0f });
				}
				else
				{
					temporaryDataStruct->subModelsSmall.Add({ FilesModel[k], pathToModel, false, 5.0f, 0.0f });
				}
			}
		}
	}



	m_mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = m_mesh;
	m_mesh->bUseAsyncCooking = true;

	m_meshAround = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMeshAround"));
	m_meshAround->SetupAttachment(m_mesh);
	m_meshAround->bUseAsyncCooking = true;

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

	m_props.Empty();
	m_vertices.Empty();
	m_normals.Empty();
	m_triangles.Empty();
	m_verticesTexture.Empty();

	m_vertexColors.Empty();

	generateLandscape();
	
	if (m_usePerlinProb)
		fillPropsPerlin();
	else
		fillPropsRelative();

	createSurfaceProps();
	m_materialDynamic->GetScalarParameterValue(FName(TEXT("dissolve")), m_currentThresholdDissolve);
	
	TArray<AActor*> actorsFound;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADirectionalLight::StaticClass(), actorsFound);
	ADirectionalLight* light = (ADirectionalLight*)actorsFound[0];
	m_light = Cast<UDirectionalLightComponent>(light->GetComponentByClass(UDirectionalLightComponent::StaticClass()));
	
	
}



// Called every frame
void AC_LandscapeGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_currentThresholdDissolve > m_maxCurrentThresholdDissolve)
	{
		increaseMaterialDissolve();
	}

	if (m_currentLightIntensity < m_maxLightIntensity)
	{
		increaseLightIntensity();
	}


	if (m_numberCompletedTargets >= m_numberTargets)
	{
		UGameplayStatics::OpenLevel(this, "ScreenEnd");
	}
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
	

	m_random.Initialize(m_seed);

	if (!m_seedManually)
	{
		m_seed = FMath::RandRange(0, 99999999);
		m_random.Initialize(m_seed);
	}

	if(!m_landscapeSettingManually)
	{
		m_worldType = static_cast<WorldType>(m_random.FRandRange(0, static_cast<float>(WorldType::last)));
		//m_worldType = WorldType::DESERT;
		m_dataTemp = *m_worldDatas.Find(m_worldType);
		

		detailsLevel = m_random.FRandRange(0.01f, 0.09f);
		amplitude = m_random.FRandRange(m_dataTemp.amplitudeMin, m_dataTemp.amplitudeMax);
		
		UE_LOG(LogTemp, Warning, TEXT("%i   seed auto  amplitude %f     detailsLevel   %f"), m_seed, amplitude, detailsLevel);
	}
	else
	{
		m_dataTemp = *m_worldDatas.Find(m_worldType);
	}


	UE_LOG(LogTemp, Warning, TEXT("%s  value world"), *m_dataTemp.name);

	


	float perlinY = m_random.FRandRange(0.0f, 50.0f);
	float perlinX = m_random.FRandRange(0.0f, 50.0f);

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

			if (i == 0)
			{
				m_verticesAroundFace.Add(FVector(x, y, z));
				m_verticesAroundFace.Add(FVector(x, y, z - m_widthPlane));
			}
			if (j == 0)
			{
				m_verticesAroundLeft.Add(FVector(x, y, z));
				m_verticesAroundLeft.Add(FVector(x, y, z - m_widthPlane));
			}
			if (j == m_imageWidth - 1)
			{
				m_verticesAroundRight.Add(FVector(x, y, z));
				m_verticesAroundRight.Add(FVector(x, y, z - m_widthPlane));
			}



			m_vertices.Add(FVector(x, y, z));
			
			m_vertexColors.Add(FColor(ForceInitToZero));

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

	for (unsigned int k = 0; k < m_imageHeight * 2; k+=2)
	{
		m_trianglesAroundFace.Add(k);
		m_trianglesAroundFace.Add(k + 1);
		m_trianglesAroundFace.Add(k + 2);

		m_trianglesAroundFace.Add(k + 1);
		m_trianglesAroundFace.Add(k + 3);
		m_trianglesAroundFace.Add(k + 2);


	}

	for (unsigned int k = 0; k < m_imageWidth * 2; k += 2)
	{
		m_trianglesAroundLeft.Add(k);
		m_trianglesAroundLeft.Add(k + 2);
		m_trianglesAroundLeft.Add(k + 1);

		m_trianglesAroundLeft.Add(k + 1);
		m_trianglesAroundLeft.Add(k + 2);
		m_trianglesAroundLeft.Add(k + 3);

		m_trianglesAroundRight.Add(k);
		m_trianglesAroundRight.Add(k + 1);
		m_trianglesAroundRight.Add(k + 2);

		m_trianglesAroundRight.Add(k + 1);
		m_trianglesAroundRight.Add(k + 3);
		m_trianglesAroundRight.Add(k + 2);

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

			if (i == 0)
			{
				m_normalsAroundFace.Add(FVector(-1, 0, 0));
				m_normalsAroundFace.Add(FVector(-1, 0, 0));

				float xText = j / static_cast<float>(m_imageWidth - 1);
				

				m_verticesTextureAroundFace.Add(FVector2D(xText, 0.0f));
				m_verticesTextureAroundFace.Add(FVector2D(xText, 1.0f / static_cast<float>(m_imageWidth - 1)));
			}

			if (j == 0)
			{
				m_normalsAroundLeft.Add(FVector(0, -1, 0));
				m_normalsAroundLeft.Add(FVector(0, -1, 0));

				float xText = i / static_cast<float>(m_imageHeight - 1);

				m_verticesTextureAroundLeft.Add(FVector2D(xText, 0.0f));
				m_verticesTextureAroundLeft.Add(FVector2D(xText, 1.0f / static_cast<float>(m_imageWidth - 1)));
			}

			if (j == m_imageWidth - 1)
			{
				m_normalsAroundRight.Add(FVector(0, 1, 0));
				m_normalsAroundRight.Add(FVector(0, 1, 0));

				float xText = i / static_cast<float>(m_imageHeight - 1);

				m_verticesTextureAroundRight.Add(FVector2D(xText, 0.0f));
				m_verticesTextureAroundRight.Add(FVector2D(xText, 1.0f / static_cast<float>(m_imageWidth - 1)));
			}
		}
	}

	TArray<FProcMeshTangent> tangents;

	m_mesh->CreateMeshSection(0, m_vertices, m_triangles, m_normals, m_verticesTexture, m_vertexColors, tangents, true);
	
	
	delete[] normalsFaces;
}

void AC_LandscapeGenerator::fillPropsRelative()
{
	FTransform SpawnLocation(FQuat(FRotator(0.0f)), FVector(0.0f), FVector(0.0f));

	bool* placeAvailable = new bool[m_imageWidth * m_imageHeight];
	float* probabilityMediumSpawn = new float[m_imageWidth * m_imageHeight];
	float* probabilitySmallSpawn = new float[m_imageWidth * m_imageHeight];


	m_probabilitySpawnDefault = m_dataTemp.defaultProbability;
	m_increaseProbabilityMedium = m_dataTemp.increaseMedium;
	m_increaseProbabilitySmall = m_dataTemp.increaseSmall;
	m_addingNearMain = m_dataTemp.addingNearMain;
	m_addingNearMedium = m_dataTemp.addingNearMedium;

	for (unsigned int i = 0; i < m_imageHeight; i++)
	{
		for (unsigned int j = 0; j < m_imageWidth; j++)
		{
			placeAvailable[i * m_imageWidth + j] = true;
			probabilityMediumSpawn[i * m_imageWidth + j] = m_probabilitySpawnDefault + m_increaseProbabilityMedium;
			probabilitySmallSpawn[i * m_imageWidth + j] = m_probabilitySpawnDefault + m_increaseProbabilitySmall;
		}

	}
	UE_LOG(LogTemp, Warning, TEXT("prob1"));
	FVector positionMain(FMath::TruncToFloat(m_random.FRandRange(0.0f, m_imageHeight)), FMath::TruncToFloat(m_random.FRandRange(0.0f, m_imageWidth)), 0.0f);

	m_spawnProp = GetWorld()->SpawnActor<AC_PropElement>(AC_PropElement::StaticClass(), SpawnLocation);
	float convertedRadius = conversionRelativeTriangleSize(m_dataTemp.mainModel.radius);
	m_spawnProp->setPropertiesProp(convertedRadius, (convertedRadius - conversionRelativeTriangleSize(m_dataTemp.mainModel.subRadius)) * m_triangleSize, conversionRelativeTriangleSize(m_dataTemp.mainModel.subRadius), *m_dataTemp.mainModel.path, FVector(positionMain.X, positionMain.Y, 0.0f), m_imageHeight, m_imageWidth, &m_random);
	//UE_LOG(LogTemp, Warning, TEXT("%f    conversion"), m_spawnProp->getRadiusPlacement());
	m_props.Add(m_spawnProp);
	m_numberTargets += 1.0f;

	float smallRadius = m_spawnProp->getRadiusPlacement() - m_spawnProp->getRadiusPlacementGradient();
	for (int i = m_spawnProp->getCenterPlacement().X - m_spawnProp->getRadiusPlacement(); i < m_spawnProp->getCenterPlacement().X + m_spawnProp->getRadiusPlacement(); i++)
		for (int j = m_spawnProp->getCenterPlacement().Y - m_spawnProp->getRadiusPlacement(); j < m_spawnProp->getCenterPlacement().Y + m_spawnProp->getRadiusPlacement(); j++)
		{
			if(FMath::Sqrt(FMath::Pow(i - m_spawnProp->getCenterPlacement().X, 2) + FMath::Pow(j - m_spawnProp->getCenterPlacement().Y, 2)) < smallRadius)
				placeAvailable[i * m_imageWidth + j] = false;  //Unavailable to place a structure on point

			probabilityMediumSpawn[i * m_imageWidth + j] = probabilityMediumSpawn[i * m_imageWidth + j] + m_addingNearMain;
			probabilitySmallSpawn[i * m_imageWidth + j] = probabilitySmallSpawn[i * m_imageWidth + j] + m_addingNearMain;
		}

	


	for (unsigned int i = 0; i < m_imageHeight; i++)
	{
		for (unsigned int j = 0; j < m_imageWidth; j++)
		{
			if (placeAvailable[i * m_imageWidth + j])
			{
				float rand = m_random.FRandRange(0.0f, 100.0f);
				//probabilitySmallSpawn[i * m_imageWidth + j] += m_increaseProbabilitySmall;


				if ( rand < probabilityMediumSpawn[i * m_imageWidth + j] ) // Medium
				{

					
					m_spawnProp = GetWorld()->SpawnActor<AC_PropElement>(AC_PropElement::StaticClass(), SpawnLocation); //Spawn an Actor at SpawnLocation
					int randomModel = m_random.RandRange(0, m_dataTemp.subModelsMedium.Num() - 1);

					//Set properties of the model 
					convertedRadius = conversionRelativeTriangleSize(m_dataTemp.subModelsMedium[randomModel].radius);
					m_spawnProp->setPropertiesProp(convertedRadius, (convertedRadius - conversionRelativeTriangleSize(m_dataTemp.subModelsMedium[randomModel].subRadius)) * m_triangleSize, conversionRelativeTriangleSize(m_dataTemp.subModelsMedium[randomModel].subRadius), *m_dataTemp.subModelsMedium[randomModel].path, FVector(i, j, 0.0f), m_imageHeight, m_imageWidth, &m_random, m_dataTemp.subModelsMedium[randomModel].flat, false);
					m_props.Add(m_spawnProp);
					m_numberTargets += 1.0f;
					smallRadius = m_spawnProp->getRadiusPlacement() - m_spawnProp->getRadiusPlacementGradient();
					for (int k = m_spawnProp->getCenterPlacement().X - m_spawnProp->getRadiusPlacement(); k < m_spawnProp->getCenterPlacement().X + m_spawnProp->getRadiusPlacement(); k++)
						for (int l = m_spawnProp->getCenterPlacement().Y - m_spawnProp->getRadiusPlacement(); l < m_spawnProp->getCenterPlacement().Y + m_spawnProp->getRadiusPlacement(); l++)
						{
							if (FMath::Sqrt(FMath::Pow(i - m_spawnProp->getCenterPlacement().X, 2) + FMath::Pow(j - m_spawnProp->getCenterPlacement().Y, 2)) < smallRadius)
								placeAvailable[i * m_imageWidth + j] = false;  //Unavailable to place a structure on point
							probabilitySmallSpawn[i * m_imageWidth + j] += m_addingNearMedium ;
						}
				}
			}
		}
	}

	for (unsigned int i = 0; i < m_imageHeight; i++)
	{
		for (unsigned int j = 0; j < m_imageWidth; j++)
		{
			if (placeAvailable[i * m_imageWidth + j])
			{
				float rand = m_random.FRandRange(0.0f, 100.0f);
				//UE_LOG(LogTemp, Warning, TEXT("%f   %f   rand"), rand, probabilityMediumSpawn[i * m_imageWidth + j]);
				if (rand < probabilitySmallSpawn[i * m_imageWidth + j]) // Medium
				{
					m_spawnProp = GetWorld()->SpawnActor<AC_PropElement>(AC_PropElement::StaticClass(), SpawnLocation);
					int randomModel = m_random.RandRange(0, m_dataTemp.subModelsSmall.Num() - 1);
					//UE_LOG(LogTemp, Warning, TEXT("prob1medium %d"), m_dataTemp.subModelsMedium.Num() - 1);

					convertedRadius = conversionRelativeTriangleSize(m_dataTemp.subModelsSmall[randomModel].radius);
					m_spawnProp->setPropertiesProp(convertedRadius, (convertedRadius - conversionRelativeTriangleSize(m_dataTemp.subModelsSmall[randomModel].subRadius)) * m_triangleSize, conversionRelativeTriangleSize(m_dataTemp.subModelsSmall[randomModel].subRadius), *m_dataTemp.subModelsSmall[randomModel].path, FVector(i, j, 0.0f), m_imageHeight, m_imageWidth, &m_random, m_dataTemp.subModelsSmall[randomModel].flat, false);
					m_props.Add(m_spawnProp);

					for (int k = m_spawnProp->getCenterPlacement().X - m_spawnProp->getRadiusPlacement(); k < m_spawnProp->getCenterPlacement().X + m_spawnProp->getRadiusPlacement(); k++)
						for (int l = m_spawnProp->getCenterPlacement().Y - m_spawnProp->getRadiusPlacement(); l < m_spawnProp->getCenterPlacement().Y + m_spawnProp->getRadiusPlacement(); l++)
						{
							if (FMath::Sqrt(FMath::Pow(i - m_spawnProp->getCenterPlacement().X, 2) + FMath::Pow(j - m_spawnProp->getCenterPlacement().Y, 2)) < m_spawnProp->getRadiusPlacement())
								placeAvailable[i * m_imageWidth + j] = false;
						}
				}
			}
		}
	}


	delete[] placeAvailable;
}

void AC_LandscapeGenerator::fillPropsPerlin()
{
	FTransform SpawnLocation(FQuat(FRotator(0.0f)), FVector(0.0f), FVector(0.0f));


	float perlinY = m_random.FRandRange(0.0f, 50.0f);

	float perlinX = m_random.FRandRange(0.0f, 50.0f);


	float initPerlinX = perlinX;
	float initPerlinY = perlinY;
	FVector2D positionMain(0.0f, 0.0f);
	float maxPerlin = 0.0f;

	bool* placeAvailable = new bool[m_imageWidth * m_imageHeight];

	for (unsigned int i = 0; i < m_imageHeight; i++)
	{
		for (unsigned int j = 0; j < m_imageWidth; j++)
		{

			float z = FMath::PerlinNoise2D(FVector2D(perlinX, perlinY)) + 1.0f;
			if (maxPerlin < z)
			{
				maxPerlin = z;
				positionMain.X = i;
				positionMain.Y = j;


			}
			placeAvailable[i * m_imageWidth + j] = true;
			perlinY += m_placeProps;
		}
		perlinY = initPerlinY;
		perlinX += m_placeProps;
		
	}

	UE_LOG(LogTemp, Warning, TEXT("Perlin1"));
	m_spawnProp = GetWorld()->SpawnActor<AC_PropElement>(AC_PropElement::StaticClass(), SpawnLocation);
	float convertedRadius = conversionRelativeTriangleSize(m_dataTemp.mainModel.radius);
	m_spawnProp->setPropertiesProp(convertedRadius, (convertedRadius - conversionRelativeTriangleSize(m_dataTemp.mainModel.subRadius)) * m_triangleSize, conversionRelativeTriangleSize(m_dataTemp.mainModel.subRadius), *m_dataTemp.mainModel.path, FVector(positionMain.X, positionMain.Y, 0.0f), m_imageHeight, m_imageWidth, &m_random);
	m_props.Add(m_spawnProp);
	m_numberTargets += 1.0f;

	float smallRadius = m_spawnProp->getRadiusPlacement() - m_spawnProp->getRadiusPlacementGradient();
	for (int i = m_spawnProp->getCenterPlacement().X - smallRadius ; i < m_spawnProp->getCenterPlacement().X + smallRadius; i++)
		for (int j = m_spawnProp->getCenterPlacement().Y - smallRadius; j < m_spawnProp->getCenterPlacement().Y + smallRadius; j++)
			placeAvailable[i * m_imageWidth + j] = false;  //Unavailable to place a structure on point
		
	perlinX = initPerlinX;
	perlinY = initPerlinY;

	UE_LOG(LogTemp, Warning, TEXT("Perlin2"));

	for (unsigned int i = 0; i < m_imageHeight; i++)
	{
		for (unsigned int j = 0; j < m_imageWidth; j++)
		{
			if (placeAvailable[i * m_imageWidth + j] )
			{
				float rand = m_random.FRandRange(0.0f, 100.0f);
				float perlinValue = FMath::PerlinNoise2D(FVector2D(perlinX, perlinY)) + 1.0f;
				if (perlinValue > m_thresholdMedium && m_probabilityMedium > rand) // Medium
				{
					//UE_LOG(LogTemp, Warning, TEXT("%f   %f   rarara"), rand, m_probabilityMedium);

					//UE_LOG(LogTemp, Warning, TEXT("%f   %f   rand prob"), rand, m_probabilityMedium);
					UE_LOG(LogTemp, Warning, TEXT("Perlin2medium"));

					m_spawnProp = GetWorld()->SpawnActor<AC_PropElement>(AC_PropElement::StaticClass(), SpawnLocation);
					int randomModel = m_random.RandRange(0, m_dataTemp.subModelsMedium.Num() - 1);
					UE_LOG(LogTemp, Warning, TEXT("Perlin num medium %d"), m_dataTemp.subModelsMedium.Num());

					convertedRadius = conversionRelativeTriangleSize(m_dataTemp.subModelsMedium[randomModel].radius);
					m_spawnProp->setPropertiesProp(convertedRadius, (convertedRadius - conversionRelativeTriangleSize(m_dataTemp.subModelsMedium[randomModel].subRadius)) * m_triangleSize, conversionRelativeTriangleSize(m_dataTemp.subModelsMedium[randomModel].subRadius), *m_dataTemp.subModelsMedium[randomModel].path, FVector(i, j, 0.0f), m_imageHeight, m_imageWidth, &m_random, m_dataTemp.subModelsMedium[randomModel].flat, false);
					m_props.Add(m_spawnProp);
					
					m_numberTargets += 1.0f;

					smallRadius = m_spawnProp->getRadiusPlacement() - m_spawnProp->getRadiusPlacementGradient();
					for (int k = m_spawnProp->getCenterPlacement().X - smallRadius; k < m_spawnProp->getCenterPlacement().X + smallRadius; k++)
						for (int l = m_spawnProp->getCenterPlacement().Y - smallRadius; l < m_spawnProp->getCenterPlacement().Y + smallRadius; l++)
							placeAvailable[k * m_imageWidth + l] = false;  //Unavailable to place a structure on point
				}
				else //Small
				{
					if (perlinValue > m_thresholdSmall && m_probabilitySmall > rand)
					{
						UE_LOG(LogTemp, Warning, TEXT("Perlin2Small"));

						m_spawnProp = GetWorld()->SpawnActor<AC_PropElement>(AC_PropElement::StaticClass(), SpawnLocation);
						int randomModel = m_random.RandRange(0, m_dataTemp.subModelsSmall.Num() - 1);
						UE_LOG(LogTemp, Warning, TEXT("Perlin num small %d"), m_dataTemp.subModelsSmall.Num());

						convertedRadius = conversionRelativeTriangleSize(m_dataTemp.subModelsSmall[randomModel].radius);
						m_spawnProp->setPropertiesProp(convertedRadius, (convertedRadius - conversionRelativeTriangleSize(m_dataTemp.subModelsSmall[randomModel].subRadius)) * m_triangleSize, conversionRelativeTriangleSize(m_dataTemp.subModelsSmall[randomModel].subRadius), *m_dataTemp.subModelsSmall[randomModel].path, FVector(i, j, 0.0f), m_imageHeight, m_imageWidth, &m_random, m_dataTemp.subModelsSmall[randomModel].flat, false);
						m_props.Add(m_spawnProp);
						smallRadius = m_spawnProp->getRadiusPlacement() - m_spawnProp->getRadiusPlacementGradient();
						for (int k = m_spawnProp->getCenterPlacement().X - smallRadius; k < m_spawnProp->getCenterPlacement().X + smallRadius; k++)
							for (int l = m_spawnProp->getCenterPlacement().Y - smallRadius; l < m_spawnProp->getCenterPlacement().Y + smallRadius; l++)
								placeAvailable[k * m_imageWidth + l] = false;  //Unavailable to place a structure on point
					}
				}

				
			}
			perlinY += m_placeProps;
		}
		perlinY = initPerlinY;
		perlinX += m_placeProps;
	}
	UE_LOG(LogTemp, Warning, TEXT("Perlinend"));



	delete[] placeAvailable;
}

void AC_LandscapeGenerator::createSurfaceProps()
{
	
	for (AC_PropElement* elt : m_props)
	{

		//UE_LOG(LogTemp, Warning, TEXT(" %s   :  %f  %f  %f"), *elt->getModelPath(), elt->getCenterPlacement().X, elt->getCenterPlacement().Y, elt->getCenterPlacement().Z);
		FVector correctedCenter = elt->getCenterPlacement();



		if (correctedCenter.X > m_imageHeight || correctedCenter.Y > m_imageWidth || correctedCenter.X < 0 || correctedCenter.Y < 0 && elt != nullptr)
		{
			elt->Destroy();
			continue;
		}
		if(elt->isFlatGround())
		{
			float radius = elt->getRadiusPlacement();


			float gradientLevel = elt->getRadiusPlacementGradient();




			//UE_LOG(LogTemp, Warning, TEXT(" %s   :  %f  %f  %f"), *elt->getModelPath(), elt->getCenterPlacement().X, elt->getCenterPlacement().Y, elt->getCenterPlacement().Z);


			std::vector<float> zValues;
			for (float i = correctedCenter.X - radius; i < correctedCenter.X + radius; i += 1.0f)
			{
				for (float j = correctedCenter.Y - radius; j < correctedCenter.Y + radius; j++)
				{
					if (FMath::Pow(i - correctedCenter.X, 2) + FMath::Pow(j - correctedCenter.Y, 2) <= radius)
					{
						int index = static_cast<int>(i) * m_imageWidth + static_cast<int>(j);
						zValues.push_back(m_heightMap[index]);
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
					//UE_LOG(LogTemp, Warning, TEXT("%f    %f     value"), i, j);

					float d = FMath::Pow(correctedCenter.X - i, 2) + FMath::Pow(correctedCenter.Y - j, 2);
					if (d <= FMath::Pow(radius, 2))
					{
						int index = i * m_imageWidth + j;

						float gradCoeff = FMath::TruncToFloat(FMath::Sqrt(d)) / radius;
						//m_vertexColors[index] = FColor(255.0f  - 255.0f * gradCoeff, 0, 0, 0);
						if (d > FMath::Pow(radius - gradientLevel, 2) && (radius - gradientLevel > 0))
						{
							float coef = radius - FMath::TruncToFloat(FMath::Sqrt(d));
							float z = m_vertices[index].Z + ((averageValue - m_vertices[index].Z) * (coef / gradientLevel));
							float r = 255.0f * (coef / gradientLevel);
						/*	if (m_vertexColors[index].R == 0.0f)
								m_vertexColors[index] = FColor(r, 0, 0, 0);
							else 
								if(m_vertexColors[index].R != 255.0f)
									m_vertexColors[index] = FColor((r + m_vertexColors[index].R ) / 2.0f, 0, 0, 0);*/
							//m_vertexColors[index] = FColor((m_vertexColors[index].R + m_random.FRandRange(50.0f, 255.0f)) / 2.0f, 0, 0, 0);

							//m_vertexColors[index] = FColor(r, 0, 0, 0);
							if(m_vertexColors[index].R <= r || m_vertexColors[index].R == 0.0f)
								m_vertexColors[index] = FColor(r, 0, 0, 0); 

							m_vertices[index] = FVector(m_vertices[index].X, m_vertices[index].Y, z);


							m_heightMap[index] = z;
						}
						else
						{
							/*if (m_vertexColors[index].R == 0)
								m_vertexColors[index] = FColor(255.0f, 0, 0, 0);
							else
								m_vertexColors[index] = FColor(FMath::Clamp(255.0f + m_vertexColors[index].R, 0.0f, 255.0f), 0, 0, 0);*/
							
							m_vertexColors[index] = FColor(255.0f, 0, 0, 0);

							m_vertices[index] = FVector(m_vertices[index].X, m_vertices[index].Y, averageValue);
							m_heightMap[index] = averageValue;

						}
					}
				}
			}
		}


		int centerZ = elt->getCenterX() * m_imageWidth + elt->getCenterY();
		FVector v(elt->getCenterPlacement().X * m_triangleSize , elt->getCenterPlacement().Y * m_triangleSize, m_heightMap[centerZ] + 20.0f);
		elt->SetActorLocation(v + GetActorLocation());
		//elt->SetActorLocation(v);

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
	UE_LOG(LogTemp, Warning, TEXT("init"));

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

	for (AC_PropElement* elt : m_props)
	{

		if (!elt->isFlatGround())
		{
			unsigned int index = elt->getCenterPlacement().X * m_imageWidth + elt->getCenterPlacement().Y;
			FVector vectorRotation = (FVector(0.0f, 0.0f, 1.0f) + m_normals[index]) * m_coeffOrientation;
			elt->setRotation(vectorRotation.Rotation());
		}
	}

	TArray<FProcMeshTangent> tangents;
	m_mesh->ClearAllMeshSections();
	m_mesh->CreateMeshSection(0, m_vertices, m_triangles, m_normals, m_verticesTexture, m_vertexColors, tangents, true);

	m_meshAround->ClearAllMeshSections();
	m_meshAround->CreateMeshSection(0, m_verticesAroundFace, m_trianglesAroundFace, m_normalsAroundFace, m_verticesTextureAroundFace, TArray<FColor>(), TArray<FProcMeshTangent>(), true);
	m_meshAround->CreateMeshSection(1, m_verticesAroundLeft, m_trianglesAroundLeft, m_normalsAroundLeft, m_verticesTextureAroundLeft, TArray<FColor>(), tangents, true);
	m_meshAround->CreateMeshSection(2, m_verticesAroundRight, m_trianglesAroundRight, m_normalsAroundRight, m_verticesTextureAroundRight, TArray<FColor>(), tangents, true);

	FString pathMaterial = "/Game/Materials/" + m_dataTemp.name + "/MainGround/" + m_dataTemp.nameCamelCase + "_MAT." + m_dataTemp.nameCamelCase + "_MAT";

	//FString pathMaterial = "/Game/Materials/" + m_dataTemp.name + "/MainGround/" + m_dataTemp.nameCamelCase + "_Around_MAT." + m_dataTemp.nameCamelCase + "_Around_MAT";

	FStringAssetReference materialFinder(pathMaterial);
	UMaterialInstance* materialObject = Cast<UMaterialInstance>(materialFinder.TryLoad());




	m_meshAround->SetMaterial(0, UMaterialInstanceDynamic::Create(materialObject, m_mesh));

	/*UE_LOG(LogTemp, Warning, TEXT("Post material ini    %s "), *pathMaterial);

	FStringAssetReference materialFinder(pathMaterial);
	UMaterialInstance* materialObject = Cast<UMaterialInstance>(materialFinder.TryLoad());*/
	m_materialDynamic = UMaterialInstanceDynamic::Create(materialObject, m_mesh);
	m_mesh->SetMaterial(0, m_materialDynamic);
}

float AC_LandscapeGenerator::conversionRelativeTriangleSize(float radius)
{
	return FMath::RoundToFloat( radius * (m_REFERENCE_CIRCLE / static_cast<float>(m_triangleSize)));
}

void AC_LandscapeGenerator::increaseMaterialDissolve()
{
	//float f; 
	
	//m_materialDynamic->GetScalarParameterValue(FName(TEXT("dissolve")), f);
	m_currentThresholdDissolve -= m_speedDissolve;
	m_materialDynamic->SetScalarParameterValue(FName(TEXT("dissolve")), m_currentThresholdDissolve);

}

void AC_LandscapeGenerator::setMaxCurrentThresholdDissolve()
{
	float f;

	m_materialDynamic->GetScalarParameterValue(FName(TEXT("dissolve")), f);
	f -= (1.5f / m_numberTargets);

	m_maxCurrentThresholdDissolve = f;
}

void AC_LandscapeGenerator::increaseLightIntensity()
{
	m_currentLightIntensity += m_speedLightChange;
	m_light->SetIntensity(m_currentLightIntensity);
}

void AC_LandscapeGenerator::setMaxLightIntensity()
{
	m_maxLightIntensity += (10.0f / m_numberTargets);
}

void AC_LandscapeGenerator::decreaseMaterialDissolve()
{
	//float f;
	//m_materialDynamic->GetScalarParameterValue(FName(TEXT("dissolve")), f);


	m_currentThresholdDissolve += (1.5f / m_numberTargets);
	m_materialDynamic->SetScalarParameterValue(FName(TEXT("dissolve")), m_currentThresholdDissolve);

}

void AC_LandscapeGenerator::increaseCompletedTarget()
{
	m_numberCompletedTargets += 1.0f;
}

void AC_LandscapeGenerator::decreaseCompletedTarget()
{
	m_numberCompletedTargets -= 1.0f;

}

float* AC_LandscapeGenerator::getNumberTargets()
{
	return &m_numberTargets;
}

float* AC_LandscapeGenerator::getNumberCompletedTargets()
{
	return &m_numberCompletedTargets;
}



