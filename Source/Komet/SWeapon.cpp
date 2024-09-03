// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASWeapon::ASWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	Distance = 10000.0f;
	bIsDebug = false;

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bReplicates = true;

}

// Called when the game starts or when spawned
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASWeapon::Server_Fire_Implementation()
{
	Fire();
}

void ASWeapon::NetMulticast_PlayEffect_Implementation()
{
	FVector LocationEffect = WeaponMesh->GetSocketLocation("Muzzle");

	if (SoundEffect)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SoundEffect, LocationEffect);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Sound Missing"));
	}
}

void ASWeapon::Fire()
{
	if (!HasAuthority())
	{
		Server_Fire();
	}
	else 
	{
		NetMulticast_PlayEffect();

		AActor* MyOwner = GetOwner();

		FVector TraceStart;
		FRotator Rotation;

		MyOwner->GetActorEyesViewPoint(TraceStart,Rotation);

		

		FVector ShotDirection = Rotation.Vector();

		FVector TraceEnd = TraceStart + (ShotDirection * Distance);

		FCollisionQueryParams ParamsQuery;
		ParamsQuery.AddIgnoredActor(MyOwner);
		ParamsQuery.AddIgnoredActor(this);

		FHitResult Hit;

		if (GetWorld()->LineTraceSingleByChannel(Hit,TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, ParamsQuery))
		{
			APawn* Pawn = Cast<APawn>(Hit.GetActor());

			if (Pawn )
			{
				UGameplayStatics::ApplyDamage(Pawn, 100.0f, MyOwner->GetInstigatorController(), MyOwner, UDamageType::StaticClass());
			}

			if (bIsDebug)
			{
				DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 1.0f, 0, 1.0f);
			}
		}
	}
}



