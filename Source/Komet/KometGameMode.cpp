// Copyright Epic Games, Inc. All Rights Reserved.

#include "KometGameMode.h"
#include "KometCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "KometCharacter.h"


void AKometGameMode::RespawnPlayer(APlayerController* PC)
{
	if (!CharacterClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterClass Missing"));
		return;
	}

	FTransform SpawnLocation = FindPlayerStart(PC)->GetActorTransform();

	AKometCharacter* Character = GetWorld()->SpawnActor<AKometCharacter>(CharacterClass, SpawnLocation);

	int32 index = Players.Find(PC);

	FLinearColor Color = Colors[index];

	Character->SetColor(Color);

	PC->Possess(Character);

}

AKometGameMode::AKometGameMode()
{
	RestartTimeInterval = 5.0f;

	// Disable Tick System
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AKometGameMode::RestartDeadPlayer()
{

	for (APlayerController* PC : Players)
	{
		if (PC && PC->GetPawn() == nullptr)
		{
			RespawnPlayer(PC);
		}
	}

}



void AKometGameMode::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AKometGameMode::RestartDeadPlayer, RestartTimeInterval, true);

}

void AKometGameMode::PostLogin(APlayerController* NewPlayer)
{

	Super::PostLogin(NewPlayer);

	Players.Add(NewPlayer);

	RespawnPlayer(NewPlayer);

}