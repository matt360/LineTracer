#pragma once
#include "GameFramework/Character.h"
#include "LineTracerCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class ALineTracerCharacter : public ACharacter
{
	#pragma region UE4 template code EXPAND
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
	#pragma endregion

	// Gun muzzle's offset from the characters location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	// Projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ALineTracerProjectile> ProjectileClass;

	// Sound to play each time we fire 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	// AnimMontage to play each time we fire 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UMaterialInterface* Material_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UMaterialInterface* Material_2;

protected:
	// Fires a projectile. 
	void OnFire();

	// Handles moving forward/backward 
	void MoveForward(float Val);

	// Handles stafing movement, left and right 
	void MoveRight(float Val);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

	// player can fire - true
	bool CanFire;

	// Handles the delay between shots
	FTimerHandle FireDelayTimerHandle;

	// Resets the players ability to fire
	void ResetFire();

public:
	// Returns Mesh1P subobject *
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	// Returns FirstPersonCameraComponent subobject *
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

