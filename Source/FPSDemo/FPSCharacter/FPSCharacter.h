// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputLibrary.h"
#include "Camera/CameraComponent.h"
#include "FPSCharacter.generated.h"


UCLASS()
class FPSDEMO_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()
    
public:
	// Sets default values for this character's properties
	AFPSCharacter();

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* FPSCameraComponent;
	UPROPERTY(VisibleAnywhere, Category = "FPSMesh")
	USkeletalMeshComponent* FPSMesh;
	
private:
	UPROPERTY(EditAnywhere, Category = "Shoot")
	float FireRate;
	UPROPERTY(EditAnywhere, Category = "Shoot")
	float LastFireTime;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<class AProjetileActor> ProjectileClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input") 
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* ShootAction;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void StartJump(const FInputActionValue& Value);
	void StopJump(const FInputActionValue& Value);

	void Shoot(const FInputActionValue& Value);
};
