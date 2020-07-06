// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_LandscapeGenerator.generated.h"

UCLASS()
class COLORSAREBLIND_API AC_LandscapeGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_LandscapeGenerator();
	~AC_LandscapeGenerator();

	void createSurfaceProps(FVector centerPoint, float radius);

	UPROPERTY(EditAnywhere, Category="Mesh")
		class UProceduralMeshComponent* m_mesh;

	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		float amplitude = 200.0f;

	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		float detailsLevel = 0.05f;

	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		unsigned int m_imageWidth = 200;

	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		unsigned int m_imageHeight = 200;

	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		bool m_boolProps = false;

	UPROPERTY(EditAnywhere, Category = "SettingsLandscape")
		unsigned int  m_triangleSize = 50;

	UPROPERTY(EditAnywhere, Category = "Render")
		UMaterialInstanceDynamic* m_materialDynamic;

	UPROPERTY(EditAnywhere, Category = "Render")
		UMaterial* m_material;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void generateLandscape();


	//Variables
	TArray<FVector> m_vertices;
	TArray<FVector> m_normals;
	TArray<int32> m_triangles;
	TArray<FVector2D> m_verticesTexture;

	float* m_heightMap;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostActorCreated() override;
	virtual void PostLoad() override;

};
