// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KometGameMode.generated.h"

class AKometCharacter;

UCLASS(minimalapi)
class AKometGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	TArray<APlayerController*> Players;

	FTimerHandle TimerHandle;

	UPROPERTY(EditDefaultsOnly)
	float RestartTimeInterval;

	void RestartDeadPlayer();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AKometCharacter> CharacterClass;

	UPROPERTY(EditDefaultsOnly)
	TArray<FLinearColor> Colors;

	void RespawnPlayer(APlayerController* PC);

public:
	AKometGameMode();

	virtual void StartPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	
};



