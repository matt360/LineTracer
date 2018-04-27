// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RotateActor.generated.h"

UCLASS()
class LINETRACER_API ARotateActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARotateActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = Movement)
	float m_Angle;

	UPROPERTY(EditAnywhere, Category = Movement)
	bool m_MoveRight;

	UPROPERTY(EditAnywhere, Category = Movement)
	bool m_MoveLeft;

	UPROPERTY(EditAnywhere, Category = Movement)
	bool m_GoUp;

	UPROPERTY(EditAnywhere, Category = Movement)
	bool m_GoDown;
	
	UPROPERTY(EditAnywhere, Category = Movement)
	FVector m_Dimensions;

	UPROPERTY(EditAnywhere, Category = Movement)
	FVector m_Axis;

	UPROPERTY(EditAnywhere, Category = Movement)
	float m_Multiplier;

};
