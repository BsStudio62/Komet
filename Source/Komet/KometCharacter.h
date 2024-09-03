// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "KometCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class ASGrenade;
class ASWeapon;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AKometCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Grenade Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GrenadeAction;

	/** Interaction Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractionAction;

	/** Interaction Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

protected:

#pragma region Grenade

	// Grenade Class // 
	UPROPERTY(EditDefaultsOnly, Category = "Grenade")
	TSubclassOf<ASGrenade> GrenadeClass;

	// Couldown Grenade // 
	bool bIsLaunch;

#pragma endregion

#pragma region Weapon

	UPROPERTY(Replicated)
	ASWeapon* Weapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ASWeapon> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName SocketWeapon;

	void WeaponFire();

#pragma endregion 

#pragma region Damage

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void HandleTakeRadialDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, FVector Origin, const FHitResult& HitInfo, AController* InstigatedBy, AActor* DamageCauser);

#pragma endregion 

	UPROPERTY(ReplicatedUsing = OnRep_Color, BlueprintReadOnly, Category = "Character")
	FLinearColor Color;

	UFUNCTION()
	void OnRep_Color();

	UPROPERTY(Replicated, BlueprintReadWrite)
	AActor* Interaction;

	UPROPERTY(Replicated, BlueprintReadWrite)
	bool GodMod;

public:

	AKometCharacter();
	
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// Launch Grenade
	void LaunchGrenade();

	UFUNCTION(Server,Reliable)
	void Server_LaunchGrenade();

	// Enable Grenade Launch
	void ReniGrenade();

	
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	void SetColor(FLinearColor ColorValue);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetInteraction(AActor* InteractionValue)
	{
		Interaction = InteractionValue;
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetGodMod(bool GodModValue)
	{
		GodMod = GodModValue;
	}

};

