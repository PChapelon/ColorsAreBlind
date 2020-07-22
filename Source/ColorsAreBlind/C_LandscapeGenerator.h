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
	float amplitudeMin;
	float amplitudeMax;
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
	
public:	
	// Sets default values for this actor's properties
	AC_LandscapeGenerator();
	~AC_LandscapeGenerator();

	void createSurfaceProps();
	void fillProps();

	UPROPERTY(EditAnywhere, Category="Mesh")
		class UProceduralMeshComponent* m_mesh;

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

	UPROPERTY(EditAnywhere, Category = "SettingsWorld")
		float m_probabilityMedium = 0.2f;

	UPROPERTY(EditAnywhere, Category = "SettingsWorld")
		float m_probabilitySmall = 0.5f;

	UPROPERTY(EditAnywhere, Category = "SettingsWorld")
		float m_placeProps = 0.04f;


	
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

	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		unsigned int  m_gradiantLevel = 10.0f;

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
		FRandomStream m_random;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void generateLandscape();


	//Variables
	TArray<FVector> m_vertices;
	TArray<FVector> m_normals;
	TArray<int32> m_triangles;
	TArray<FVector2D> m_verticesTexture;

	TArray<FColor> m_vertexColors;

	float* m_heightMap;
	int32* m_distributionMap;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostActorCreated() override;
	virtual void PostLoad() override;

};
