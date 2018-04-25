#include "LineTracer.h"
#include "LineTracerCharacter.h"
#include "LineTracerProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

ALineTracerCharacter::ALineTracerCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	#pragma region UE4 template code EXAPND
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	#pragma endregion

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// timer
	CanFire = true;

	Gravity = false;

	HoldingComp = CreateDefaultSubobject<USceneComponent>(TEXT("HoldingComponent"));
	HoldingComp->RelativeLocation.Y = 100.0f;
	HoldingComp->SetupAttachment(FP_MuzzleLocation);
}

void ALineTracerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
}

// Called every frame
void ALineTracerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MeshRootComp)
		MeshRootComp->SetWorldLocationAndRotation(HoldingComp->GetComponentLocation(), HoldingComp->GetComponentRotation());
		//MeshRootComp->SetWorldLocation(GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation() + 100.f);
		//MeshRootComp->SetRotation(GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorRotation());
}

void ALineTracerCharacter::GravityGun()
{
	// get the forawrd vector from where the player is looking
	FVector CameraForward = FVector(FirstPersonCameraComponent->GetForwardVector());
	MeshRootComp->AddForce(CameraForward * 100000 * MeshRootComp->GetMass());
	MeshRootComp = nullptr;
}

void ALineTracerCharacter::OnFire()
{
	if (CanFire)
	{
		// set CanFire to false. Later set to true in the ResetFire() function
		CanFire = false;

		// set to true to loop
		GetWorld()->GetTimerManager().SetTimer(FireDelayTimerHandle, this, &ALineTracerCharacter::ResetFire, 0.2f, false); 

		// see if the line trace hit anything
		FHitResult Hit;

		// end the line trace 2000 units from the start
		float LineLength = 2000;

		// grab the control rotation from the actor character
		FRotator SpawnRotation = GetControlRotation();

		// function from the starter content - point in frot of the gun - MuzzleOffset is in camera space, so transform it to world space before
		// offsetting from the character location to find the final muzzle position
		FVector StartLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		// start location plus camera forward vector multiplied by the trace line lenght
		FVector EndLocation = StartLocation + (FirstPersonCameraComponent->GetForwardVector() * LineLength);

		// variable to handle collision events in the line trace
		FCollisionQueryParams CollisionParameters;
		// set Hit - if the hit was successfull FHitResult will return the hit actor
		GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_PhysicsBody, CollisionParameters);

		// draw the trace line
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1, 0, 1.f);

		// if Hit returned a hit actor
		if (Hit.GetActor())
		{
			// check if the hit actor root component is movable
			if (Hit.GetActor()->IsRootComponentMovable()) {
				// cast the hit actor's mesh to MeshRootComp
				MeshRootComp = Cast<UStaticMeshComponent>(Hit.GetActor()->GetRootComponent());

				if (MeshRootComp->GetMaterial(0)->GetName() == "FirstPersonProjectileMaterial")
					MeshRootComp->SetMaterial(0, Material_2);
				else
					MeshRootComp->SetMaterial(0, Material_1);
			}

		}
	
		#pragma region UE4 template code
		// try and play a firing animation if specified
		if (FireAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
		#pragma endregion
	}
}

void ALineTracerCharacter::ResetFire()
{
	CanFire = true;
	GetWorldTimerManager().ClearTimer(FireDelayTimerHandle);
}

#pragma region UE4 template code
void ALineTracerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ALineTracerCharacter::OnFire);
	PlayerInputComponent->BindAction("FireRelease", IE_Pressed, this, &ALineTracerCharacter::GravityGun);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALineTracerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALineTracerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ALineTracerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ALineTracerCharacter::LookUpAtRate);
}

void ALineTracerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ALineTracerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ALineTracerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALineTracerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
#pragma endregion
