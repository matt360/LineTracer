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
	class USkeletalMeshComponent* m_Mesh1P;

	// Gun mesh: 1st person view (seen only by self) 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* m_Gun;

	// Location on gun mesh where projectiles should spawn. 
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* m_MuzzleLocation;

	// First person camera 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* m_CameraComponent;

public:
	ALineTracerCharacter();

protected:
	virtual void BeginPlay();

public:
	// Base turn rate, in deg/sec. Other scaling may affect final turn rate. 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float m_BaseTurnRate;

	// Base look up/down rate, in deg/sec. Other scaling may affect final rate. 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float m_BaseLookUpRate;

	// Projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ALineTracerProjectile> m_ProjectileClass;

	// AnimMontage to play each time we fire 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* m_FireAnimation;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Gun muzzle's offset from the characters location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector m_GunOffsetFromMuzzle;

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
	// player can fire - true
	bool m_CanFire;

	// Handles the delay between shots
	FTimerHandle m_FireDelayTimerHandle;

	// Fires a projectile. 
	void OnFire();

	// Resets the players ability to fire
	void ResetGun();

	// gravity gun
protected:
	UPROPERTY(EditAnywhere)
	USceneComponent* m_HeldObjectLocation;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* m_HeldObject;
	
	// Glass material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UMaterialInterface* m_Material_1;

	// Yellow material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UMaterialInterface* m_Material_2;

	void GravityGun();

public:
	// Returns m_Mesh1P subobject *
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return m_Mesh1P; }
	// Returns m_CameraComponent subobject *
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return m_CameraComponent; }
};

