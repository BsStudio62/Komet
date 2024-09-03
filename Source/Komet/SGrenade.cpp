// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASGrenade::ASGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	MeshGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshGrenade"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	MeshGrenade->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BaseDamage = 100.0f;
	MinimunDamage = 10.0f;
	InnerRadiusDamage = 200.0f;
	OuterRadiusDamage = 650.0f;
	FallOffDamage = 1.0f;

	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->InitialSpeed = 1000.0f;
	ProjectileMovementComponent->MaxSpeed = 1000.0f;

	bIsDebug = false;

	bReplicates = true;
	SetReplicateMovement(true);


}

// Called when the game starts or when spawned
void ASGrenade::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle,this, &ASGrenade::Explode,ExplosionInterval, false);

	}

	
}

void ASGrenade::Explode()
{
	FVector Location = GetActorLocation();
	TArray<AActor*> IgnoredActor;

	if (UGameplayStatics::ApplyRadialDamageWithFalloff(this, BaseDamage, MinimunDamage, Location, InnerRadiusDamage, OuterRadiusDamage, FallOffDamage,UDamageType::StaticClass(),IgnoredActor))
	{
		NetMulticast_PlayEffect();

	}

	if (bIsDebug)
	{
		DrawDebugSphere(GetWorld(), Location, InnerRadiusDamage, 12, FColor::Red, false,5.0f);
		DrawDebugSphere(GetWorld(), Location, OuterRadiusDamage, 12, FColor::Green, false,5.0f);
	}

	Destroy();
}


void ASGrenade::NetMulticast_PlayEffect_Implementation()
{
	FVector Location = GetActorLocation();

	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, Location);

		if (SoundEffect)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SoundEffect, Location);
		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("Sound Missing"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Effect Particule Missing"));
	}

	


}



