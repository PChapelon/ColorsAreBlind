// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "C_HUD.generated.h"

/**
 * 
 */
UCLASS()
class COLORSAREBLIND_API AC_HUD : public AHUD
{
	GENERATED_BODY()


	public:
		AC_HUD();
		//~AC_HUD();

		virtual void DrawHUD() override;
		
		UPROPERTY()
			class AC_LandscapeGenerator* m_landscape;
		UPROPERTY()
			UFont* m_fontScore;

		float* m_completed;
		float* m_targetsTotal;
	protected:
		virtual void PostActorCreated() override; 
		virtual void PostLoad() override;
};
