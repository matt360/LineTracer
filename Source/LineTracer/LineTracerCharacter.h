#pragma once
#include "GameFramework/Character.h"
#include "LineTracerCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class ALineTracerCharacter : public ACharacter
{
	GENERATED_BODY()

	// Pawn mesh: 1st person view (arms; seen only by self) 
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	// Gun mesh: 1st person view (seen only by self) 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	// Location on gun mesh where projectiles should spawn. 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	// First person camera 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

public:
	ALineTracerCharacter();

protected:
	virtual void BeginPlay();

public:
	// Base turn rate, in deg/sec. Other scaling may affect final turn rate. 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	// Base look up/down rate, in deg/sec. Other scaling may affect final rate. 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	// Projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ALineTracerProjectile> ProjectileClass;

	// Sound to play each time we fire 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Gun muzzle's offset from the characters location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffsetFromMuzzle;

	// AnimMontage to play each time we fire 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

protected:
	// Handles moving forward/backward 
	void MoveForward(float Val);

	// Handles stafing movement, left and right 
	void MoveRight(float Val);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

protected:
	// Fires a projectile. 
	void OnFire();

	// player can fire - true
	bool CanFire;

	// Handles the delay between shots
	FTimerHandle FireDelayTimerHandle;

	// Resets the players ability to fire
	void ResetFire();

	// gravity gun
	void GravityGun();

	UPROPERTY(EditAnywhere)
	USceneComponent* HeldObjectLocation;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* HeldObject;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UMaterialInterface* Material_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UMaterialInterface* Material_2;

public:
	// Returns Mesh1P subobject *
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	// Returns FirstPersonCameraComponent subobject *
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
};

