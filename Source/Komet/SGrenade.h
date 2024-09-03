// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGrenade.generated.h"

UCLASS()
class KOMET_API ASGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASGrenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Mesh Grenade
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	UStaticMeshComponent* MeshGrenade;

	// Movement Component
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	// Variable Debug class
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	bool bIsDebug;

	// Temps entre Explosion
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	float ExplosionInterval;

	// Base Damage
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float BaseDamage;

	// Minimun Damage
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float MinimunDamage;

	// Radius Interne
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float InnerRadiusDamage;

	// Radius Externe
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float OuterRadiusDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float FallOffDamage;

	// Effect Explosion Particule
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	UParticleSystem* ExplosionEffect;

	// Effect Sound
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	USoundBase* SoundEffect;

	void Explode();

	UFUNCTION(NetMulticast, reliable)
	void NetMulticast_PlayEffect();

public:

	FORCEINLINE float GetIntervalExplosion() const { return ExplosionInterval; }
};
