// Fill out your copyright notice in the Description page of Project Settings.


#include "Act_Gamemode.h"
#include "actiondemo/Character/Act_Character.h"
#include "actiondemo/Character/Act_Controller.h"

AAct_Gamemode::AAct_Gamemode()
{
	DefaultPawnClass=AAct_Character::StaticClass();
	PlayerControllerClass=AAct_Controller::StaticClass();
	
}
