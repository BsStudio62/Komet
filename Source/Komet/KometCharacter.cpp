// Copyright Epic Games, Inc. All Rights Reserved.

#include "KometCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "SGrenade.h"
#include "SWeapon.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AKometCharacter


AKometCharacter::AKometCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	bUseControllerRotationYaw = true;

	bIsLaunch = false;
	SocketWeapon = "weapon_r";

	GodMod = false;
}

void AKometCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (HasAuthority())
	{
		
		if (!WeaponClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponClass Missing"));
			return;
		}

		// Spawn Weapon
		FActorSpawnParameters Params;
		Params.Owner = this;
		Weapon = GetWorld()->SpawnActor<ASWeapon>(WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketWeapon);

		// Event Dynamique Damage
		this->OnTakeAnyDamage.AddDynamic(this, &AKometCharacter::HandleTakeAnyDamage);
		this->OnTakeRadialDamage.AddDynamic(this, &AKometCharacter::HandleTakeRadialDamage);


	}

}

void AKometCharacter::SetColor(FLinearColor ColorValue)
{
	Color = ColorValue;

	UMaterialInstanceDynamic* Mat = GetMesh()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterials()[0]);

	Mat->SetVectorParameterValue("Tint", Color);

	UMaterialInstanceDynamic* Mat02 = GetMesh()->CreateDynamicMaterialInstance(1, GetMesh()->GetMaterials()[1]);

	Mat02->SetVectorParameterValue("Tint", Color);

}

void AKometCharacter::OnRep_Color()
{
	UMaterialInstanceDynamic* Mat = GetMesh()->CreateDynamicMaterialInstance(0, GetMesh()->GetMaterials()[0]);

	Mat->SetVectorParameterValue("Tint", Color);

	UMaterialInstanceDynamic* Mat02 = GetMesh()->CreateDynamicMaterialInstance(1, GetMesh()->GetMaterials()[1]);

	Mat02->SetVectorParameterValue("Tint", Color);
	
}


void AKometCharacter::WeaponFire()
{
	if (Weapon)
	{
		Weapon->Fire();
	}
	
}

void AKometCharacter::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (GodMod) return;

	TArray<AActor*> Actors;
	GetAttachedActors(Actors);

	if (Actors[0])
	{
		Actors[0]->Destroy();
	}

	Destroy();
}

void AKometCharacter::HandleTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, const FHitResult& HitInfo, AController* InstigatedBy, AActor* DamageCauser)
{
	if (GodMod) return;

	TArray<AActor*> Actors;
	GetAttachedActors(Actors);

	if (Actors[0])
	{
		Actors[0]->Destroy();
	}

	Destroy();

}

//////////////////////////////////////////////////////////////////////////
// Input

void AKometCharacter::ReniGrenade()
{

	bIsLaunch = false;
}

void AKometCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AKometCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AKometCharacter::Look);

		// Grenade
		EnhancedInputComponent->BindAction(GrenadeAction, ETriggerEvent::Started, this, &AKometCharacter::LaunchGrenade);

		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AKometCharacter::WeaponFire);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AKometCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AKometCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AKometCharacter::LaunchGrenade()
{

	if (!HasAuthority())
	{
		Server_LaunchGrenade();
	}
	else {

		if (!bIsLaunch)
		{
			if (!GrenadeClass)
			{
				UE_LOG(LogTemp, Warning, TEXT("GrenadeClass Missing"));
				return;
			}

			bIsLaunch = true;

			const FVector Location = GetMesh()->GetSocketLocation("weapon_r");
			const FRotator Rotation = CameraBoom->GetTargetRotation();

			ASGrenade* Grenade = GetWorld()->SpawnActor<ASGrenade>(GrenadeClass,Location,Rotation);

			const float TimeExplosion = Grenade->GetIntervalExplosion();

			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AKometCharacter::ReniGrenade, TimeExplosion, false);
			
		}

	}
}

void AKometCharacter::Server_LaunchGrenade_Implementation()
{
	LaunchGrenade();
}

void AKometCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AKometCharacter, Weapon);
	DOREPLIFETIME(AKometCharacter, Color);
	DOREPLIFETIME(AKometCharacter, Interaction);
	DOREPLIFETIME(AKometCharacter, GodMod);

}