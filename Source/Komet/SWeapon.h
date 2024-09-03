// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

UCLASS()
class KOMET_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* WeaponMesh;

	// Distance de tir
	UPROPERTY(EditDefaultsOnly)
	float Distance;

	UFUNCTION(Server,Reliable)
	void Server_Fire();

	UPROPERTY(EditDefaultsOnly)
	bool bIsDebug;

	// Effect Sound
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* SoundEffect;

	UFUNCTION(NetMulticast,reliable)
	void NetMulticast_PlayEffect();

public:	

	void Fire();

};
