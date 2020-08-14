// Fill out your copyright notice in the Description page of Project Settings.


#include "C_HUD.h"
#include "Engine/Canvas.h"
#include "C_LandscapeGenerator.h"
#include "Kismet/GameplayStatics.h"


AC_HUD::AC_HUD()
{
	ConstructorHelpers::FObjectFinder<UFont> fontFinder(TEXT("Font'/Game/UI/FontScore.FontScore'"));
	if (fontFinder.Object)
	{
		m_fontScore = fontFinder.Object;
	}
}

/*
AC_HUD::~AC_HUD()
{
	delete m_targetsTotal;
	delete m_completed;
	delete m_fontScore;
	delete m_landscape;
}*/

void AC_HUD::DrawHUD()
{
	Super::DrawHUD();
	FString str;
	if (m_landscape != nullptr)
	{
		float f = 5.0f;
		str = "Monuments decouverts : ";
		if (*m_completed >= *m_targetsTotal)
		{
			str += "termine";
		}
		else
		{
			str += FString::FromInt(static_cast<int>(*m_completed)) + " /  " + FString::FromInt(static_cast<int>(*m_targetsTotal));
		}
	}
	else
	{
		str = FString("Error Loading");
	}
	DrawRect(FLinearColor(0.0f,0.0f,0.0f,0.5f), 10, 10, 450, 50);

	DrawText(str, FLinearColor(1.0f, 1.0f, 0.0f), 20.0f, 20.0f, m_fontScore, 1.0f);
	//FCanvasBoxItem backGround(FVector2D(10.0f,10.0f), F)
	//Canvas->DrawText(font, str , 100, 100);
	/*FCanvasBoxItem ProgressBar(FVector2D(5, 25), FVector2D(100, 5));
	Canvas->DrawItem(ProgressBar);
	DrawRect(FLinearColor::Blue, 5, 25, 100, 5);*/


}

void AC_HUD::PostActorCreated()
{
	Super::PostActorCreated();
	TArray<AActor*> actorsFound;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AC_LandscapeGenerator::StaticClass(), actorsFound);
	if (actorsFound.Num() > 0)
	{
		m_landscape = (AC_LandscapeGenerator*)actorsFound[0];
		m_completed = m_landscape->getNumberCompletedTargets();
		m_targetsTotal = m_landscape->getNumberTargets();
	}
	
}

void AC_HUD::PostLoad()
{
	Super::PostLoad();
}

