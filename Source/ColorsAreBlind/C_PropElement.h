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
	void setPropertiesProp(float radius, float radiusTrigger, float radiusGradient, FString path, FVector center,  unsigned int heightImage, unsigned int widthImage, FRandomStream* random, bool flat = true, bool mainModel = true, FVector scale = FVector(1.0f));
	
	float getRadiusPlacement();
	
	float getRadiusPlacementGradient();

	void setCenter(FVector v);

	float getCenterX();

	float getCenterY();

	void setCenterZ(float z);

	void setRotation(FRotator rotation);

	bool isFlatGround();

	bool isCompleted();

	FString getModelPath();

	FVector getCenterPlacement();

	UPROPERTY(EditAnywhere, Category="Mesh")
		class UStaticMeshComponent* m_mesh;
	
	UPROPERTY(EditAnywhere, Category="Material")
		class UMaterialInstanceDynamic* m_dynamicMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
		class UParticleSystemComponent* m_particlesCompleted;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
		class UParticleSystem* m_particlesToAffectMedium;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
		class UParticleSystem* m_particlesToAffectMain;

	UPROPERTY(EditAnywhere, Category = "Trigger")
		class UCapsuleComponent* m_trigger;

	UPROPERTY(EditAnywhere, Category = "Audio")
		class USoundCue* m_audioCompleted;

	UPROPERTY(EditAnywhere, Category = "Audio")
		class UAudioComponent* m_audioCompletedComponent;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
		float m_radiusPlacement;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		float m_radiusPlacementGradient;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		FVector m_center;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		FString m_pathToObject;

	UPROPERTY(VisibleAnywhere, Category = "Ground")
		bool m_flatGround = true;
	
	UPROPERTY(VisibleAnywhere, Category = "Trigger")
		float m_radiusTrigger = 55.0f;

	UPROPERTY(VisibleAnywhere, Category = "Trigger")
		bool m_isInside = false;

	UPROPERTY(VisibleAnywhere, Category = "Target")
		bool m_isCompleted = false;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
