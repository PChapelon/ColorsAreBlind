// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C_PropElement.generated.h"

UCLASS()
class COLORSAREBLIND_API AC_PropElement : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AC_PropElement();
	void setPropertiesProp(float radius, float radiusGradient, FString path, FVector center, FVector scale = FVector(1.0f) );
	
	float getRadiusPlacement();
	
	float getRadiusPlacementGradient();

	void setCenter(FVector v);

	float getCenterX();

	float getCenterY();

	void setCenterZ(float z);

	FString getModelPath();

	FVector getCenterPlacement();

	UPROPERTY(EditAnywhere, Category="Mesh")
		class UStaticMeshComponent* m_mesh;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
		float m_radiusPlacement;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		float m_radiusPlacementGradient;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		FVector m_center;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		FString m_pathToObject;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
