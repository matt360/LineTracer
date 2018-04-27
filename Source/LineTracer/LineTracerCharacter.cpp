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
	// set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	m_BaseTurnRate = 45.f;
	m_BaseLookUpRate = 45.f;

	// create a CameraComponent	
	m_CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	m_CameraComponent->SetupAttachment(GetCapsuleComponent());
	m_CameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	m_CameraComponent->bUsePawnControlRotation = true;

	// create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	m_Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	m_Mesh1P->SetOnlyOwnerSee(true);
	m_Mesh1P->SetupAttachment(m_CameraComponent);
	m_Mesh1P->bCastDynamicShadow = false;
	m_Mesh1P->CastShadow = false;
	m_Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	m_Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// create a gun mesh component
	m_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	m_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	m_Gun->bCastDynamicShadow = false;
	m_Gun->CastShadow = false;
	m_Gun->SetupAttachment(RootComponent);

	m_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	m_MuzzleLocation->SetupAttachment(m_Gun);
	m_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// default offset from the character location for projectiles to spawn
	m_GunOffsetFromMuzzle = FVector(100.0f, 0.0f, 10.0f);

	// timer
	m_CanFire = true;
	
	m_HeldObjectLocation = CreateDefaultSubobject<USceneComponent>(TEXT("HeldObjectLocation"));
	// attach the held object location to the muzzle 
	m_HeldObjectLocation->SetupAttachment(m_MuzzleLocation);
	// set the relative location (to the player) of the held object location
	m_HeldObjectLocation->RelativeLocation.Y = 25.0f;
	m_HeldObjectLocation->RelativeLocation.Z = 150.0f;
	m_HeldObjectLocation->RelativeRotation.Roll = 0.0f;
	m_HeldObjectLocation->RelativeRotation.Pitch = 0.0f;
	m_HeldObjectLocation->RelativeRotation.Yaw = 0.0f;
}

void ALineTracerCharacter::BeginPlay()
{
	// call the base class  
	Super::BeginPlay();

	// attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	m_Gun->AttachToComponent(m_Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
}

// called every frame
void ALineTracerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_HeldObject)
		m_HeldObject->SetWorldLocationAndRotation(m_HeldObjectLocation->GetComponentLocation(), m_HeldObjectLocation->GetComponentRotation());
}

void ALineTracerCharacter::GravityGun()
{
	// get the forawrd vector from where the player is looking
	FVector CameraForward = FVector(m_CameraComponent->GetForwardVector());
	m_HeldObject->SetMaterial(0, m_Material_2);
	// take the camera's vector and mulitply by a foctor of 100000 and the mass of the held object 
	m_HeldObject->AddForce(CameraForward * 100000 * m_HeldObject->GetMass());
	// after releasing the held obejct, set the m_HeldObject pointer to NULL, to prepare it for the next object
	m_HeldObject = nullptr;
}

void ALineTracerCharacter::OnFire()
{
	if (m_CanFire)
	{
		// set m_CanFire to false. Later set to true in the ResetGun() function
		m_CanFire = false;

		// set to true to loop
		GetWorld()->GetTimerManager().SetTimer(m_FireDelayTimerHandle, this, &ALineTracerCharacter::ResetGun, 0.2f, false); 

		// see if the ray trace hit anything
		FHitResult Hit;

		// grab the control rotation from the actor character
		FRotator SpawnRotation = GetControlRotation();

		// function from the starter content - point in frot of the gun - MuzzleOffset is in camera space, so transform it to world space before
		// offsetting from the character location to find the final muzzle position
		FVector StartLocation = ((m_MuzzleLocation != nullptr) ? m_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(m_GunOffsetFromMuzzle);

		// end the ray trace 2000 units from the start
		float LineLength = 2000;

		// start location plus camera forward vector multiplied by the trace line lenght
		FVector EndLocation = StartLocation + (m_CameraComponent->GetForwardVector() * LineLength);

		// variable to handle collision events in the ray trace
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
				// cast the hit actor's mesh to m_HeldObject
				m_HeldObject = Cast<UStaticMeshComponent>(Hit.GetActor()->GetRootComponent());
				// set material to glass
				m_HeldObject->SetMaterial(0, m_Material_1);
			}

		}
		
		// try and play a firing animation if specified
		if (m_FireAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = m_Mesh1P->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(m_FireAnimation, 1.f);
			}
		}
		
	}
}

void ALineTracerCharacter::ResetGun()
{
	m_CanFire = true;
	GetWorldTimerManager().ClearTimer(m_FireDelayTimerHandle);
}


void ALineTracerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// left mouse button press
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ALineTracerCharacter::OnFire);
	// right mouse button press
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
	AddControllerYawInput(Rate * m_BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALineTracerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * m_BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

