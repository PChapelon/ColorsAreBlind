// Fill out your copyright notice in the Description page of Project Settings.


#include "C_GameMode.h"
#include "C_HUD.h"


AC_GameMode::AC_GameMode() : AGameModeBase()
{
	HUDClass = AC_HUD::StaticClass();
}
