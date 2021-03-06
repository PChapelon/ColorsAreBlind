// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_LandscapeGenerator.generated.h"

UENUM(BlueprintType)
enum class WorldType : uint8
{
	DESERT = 0	UMETA(DisplayName = "Desert"),
	BANQUISE	UMETA(DisplayName = "Banquise"),
	FORET		UMETA(DisplayName = "Foret"),
	last		UMETA(DisplayName = "Last")
};

USTRUCT(BlueprintType)
struct  FWorldModel
{
	GENERATED_BODY()
public:
	FString name;
	FString path;
	bool flat;
	float radius;
	float subRadius;
};

USTRUCT(BlueprintType)
struct  FWorldDataStruct
{
	GENERATED_BODY()
public:
	FString name;
	FString nameCamelCase;
	float amplitudeMin;
	float amplitudeMax;
	float defaultProbability;
	float increaseMedium;
	float increaseSmall;
	float addingNearMain;
	float addingNearMedium;
	FWorldModel mainModel;
	TArray<FWorldModel> subModelsMedium;
	TArray<FWorldModel> subModelsSmall;
};






UCLASS()
class COLORSAREBLIND_API AC_LandscapeGenerator : public AActor
{
	GENERATED_BODY()


private: 
	UPROPERTY()
		TSubclassOf<AActor> m_temp;

	UPROPERTY()
		float m_currentThresholdDissolve = 1.5f;

	UPROPERTY()
		float m_maxCurrentThresholdDissolve = 1.5f;

	UPROPERTY()
		float m_currentLightIntensity = 0.1f;

	UPROPERTY()
		float m_maxLightIntensity = 0.01f;

	UPROPERTY()
		float m_speedLightChange = 0.01f;
	
public:	
	// Sets default values for this actor's properties
	AC_LandscapeGenerator();
	~AC_LandscapeGenerator();

	void createSurfaceProps();
	void fillPropsPerlin();
	void fillPropsRelative();

	UPROPERTY(EditAnywhere, Category="Mesh")
		class UProceduralMeshComponent* m_mesh;
	UPROPERTY()
		class UProceduralMeshComponent* m_meshAround;

	UPROPERTY(EditAnywhere, Category = "Dissolve")
		float m_speedDissolve = 0.001f;

	UPROPERTY(EditAnywhere, Category = "Probability Distribution Relative")
		float m_probabilitySpawnDefault = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Probability Distribution Relative")
		float m_increaseProbabilityMedium = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Probability Distribution Relative")
		float m_increaseProbabilitySmall = 0.3f;

	UPROPERTY(EditAnywhere, Category = "Probability Distribution Relative")
		float m_addingNearMain = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Probability Distribution Relative")
		float m_addingNearMedium = 0.2f;
	
	UPROPERTY(EditAnywhere, Category = "Probability Distribution Relative")
		bool m_usePerlinProb = true;



	UPROPERTY(EditAnywhere, Category = "SettingsWorld")
		WorldType m_worldType ;

	UPROPERTY(EditAnywhere, Category = "SettingsWorld")
		TMap<WorldType, FWorldDataStruct> m_worldDatas;

	UPROPERTY(EditAnywhere, Category = "SettingsWorld")
		TMap<FString, WorldType> m_worldStringToEnum;

	UPROPERTY(EditAnywhere, Category = "SettingsWorld")
		FWorldDataStruct m_dataTemp;

	UPROPERTY(EditAnywhere, Category = "SettingsWorld")
		FWorldModel m_modelTemp;

	UPROPERTY(EditAnywhere, Category = "Probability Distribution Perlin")
		float m_probabilityMedium = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Probability Distribution Perlin")
		float m_probabilitySmall = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Probability Distribution Perlin")
		float m_placeProps = 0.04f;

	UPROPERTY(EditAnywhere, Category = "Probability Distribution Perlin")
		float m_thresholdMedium = 0.7f;

	UPROPERTY(EditAnywhere, Category = "Probability Distribution Perlin")
		float m_thresholdSmall = 0.2f;

	UPROPERTY(EditAnywhere, Category = "SettingsWorld")
		float m_coeffOrientation = 1.0f / 2.0f;

	UPROPERTY(EditAnywhere, Category = "Target")
		float m_numberTargets = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Target")
		float m_numberCompletedTargets = 0.0f;

	
	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		float amplitude = 200.0f;

	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		float detailsLevel = 0.05f;

	UPROPERTY(EditAnywhere, Category = "SettingsGenerator")
		int32 m_seed;

	UPROPERTY(EditAnywhere, Category = "SettingsGenerator")
		bool m_seedManually = false;

	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		bool m_landscapeSettingManually = false;

	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		unsigned int m_imageWidth = 200;

	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		unsigned int m_imageHeight = 200;

	/*UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		unsigned int  m_gradiantLevel = 10.0f;*/

	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		unsigned int  m_triangleSize = 50;

	UPROPERTY(EditAnywhere, Category = "Render")
		UMaterialInstanceDynamic* m_materialDynamic;

	UPROPERTY(EditAnywhere, Category = "Render")
		UMaterial* m_material;

	UPROPERTY()
		class AC_PropElement* m_spawnProp;

	UPROPERTY()
		TArray<AC_PropElement*> m_props;
	
	UPROPERTY()
		class UDirectionalLightComponent* m_light;

	UPROPERTY()
		FRandomStream m_random;
	
	UPROPERTY()
		float m_REFERENCE_CIRCLE = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Mesh")
		float m_widthPlane = 150.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void generateLandscape();
	float conversionRelativeTriangleSize(float radius);


	//Variables
	TArray<FVector> m_vertices;
	TArray<FVector> m_normals;
	TArray<int32> m_triangles;

	TArray<FVector2D> m_verticesTexture;

	TArray<FColor> m_vertexColors;

	TArray<FVector> m_verticesAroundFace;
	TArray<FVector> m_verticesAroundLeft;
	TArray<FVector> m_verticesAroundRight;

	TArray<FVector> m_normalsAroundFace;
	TArray<FVector> m_normalsAroundLeft;
	TArray<FVector> m_normalsAroundRight;

	TArray<int32> m_trianglesAroundFace;
	TArray<int32> m_trianglesAroundLeft;
	TArray<int32> m_trianglesAroundRight;

	TArray<FVector2D> m_verticesTextureAroundFace;
	TArray<FVector2D> m_verticesTextureAroundLeft;
	TArray<FVector2D> m_verticesTextureAroundRight;

	float* m_heightMap;
	int32* m_distributionMap;



public:	


	void increaseMaterialDissolve();
	void decreaseMaterialDissolve();
	void setMaxCurrentThresholdDissolve();

	void increaseLightIntensity();
	void setMaxLightIntensity();

	float* getNumberTargets();
	float* getNumberCompletedTargets();
 
	void increaseCompletedTarget();
	void decreaseCompletedTarget();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostActorCreated() override;
	virtual void PostLoad() override;

};
