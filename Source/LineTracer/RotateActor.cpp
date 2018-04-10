// Fill out your copyright notice in the Description page of Project Settings.

#include "LineTracer.h"
#include "RotateActor.h"


// Sets default values
ARotateActor::ARotateActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 300 - number of units away from the player in the X axis
	Dimensions = FVector(300, 0, 0);
	// what direction is the actor to be moved about
	//AxisVector = FVector(0, 1, 1); 
	Multiplier = 50.0f;
}

// Called when the game starts or when spawned
void ARotateActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARotateActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// rotate actor around the player
	FVector NewLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();

	// degree around the point the actor should move to
	// DeltaTime times Multiplier for smooth movement
	AngleAxis += DeltaTime * Multiplier;

	// when the actor made a full circle around the player - reset AngleAxis
	if (AngleAxis >= 360.0f) { AngleAxis = 0; } 

	// Set RotateValue from Dimensions vector
	// this will return the number of units to move the actor to the new location
	FVector RotateValue = Dimensions.RotateAngleAxis(AngleAxis, AxisVector);
	NewLocation.X += RotateValue.X;
	NewLocation.Y += RotateValue.Y;
	NewLocation.Z += RotateValue.Z;

	// 
	FRotator NewRotation = FRotator(0, AngleAxis, 0);

	FQuat QuatRotation = FQuat(NewRotation);

	SetActorLocationAndRotation(NewLocation, QuatRotation, false, 0, ETeleportType::None);
}

