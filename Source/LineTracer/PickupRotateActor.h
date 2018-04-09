// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "PickupRotateActor.generated.h"

UCLASS()
class LINETRACER_API APickupRotateActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupRotateActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MyMesh;

	UPROPERTY(EditAnywhere)
	USceneComponent* HoldingComp;

	UFUNCTOIN()
	void RotateActor();

	UFUNCTION()
	void Pickup();

	bool bHolding;
	bool bGravity;


	FRotator ControlRotation;
	ACharacter* MyCharacter;
	UCameraComponent* PlayerCamera;
	// Track player's forward vector
	FVector ForwardVector;

	
	
};
