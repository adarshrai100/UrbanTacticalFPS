// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerOperator.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaponBase.h"
#include "Engine/World.h"

// Sets default values
APlayerOperator::APlayerOperator()
{
    PrimaryActorTick.bCanEverTick = true;

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
    FirstPersonCamera->bUsePawnControlRotation = true;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
    GetCharacterMovement()->GroundFriction = 8.f;
    bUseControllerRotationYaw = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    GetMesh()->SetOwnerNoSee(true);

    UE_LOG(LogTemp, Warning, TEXT("PlayerOperator Active"));
}


// Called to bind functionality to input
void APlayerOperator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerOperator::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerOperator::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerOperator::StartSprint);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerOperator::StopSprint);

    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerOperator::StartCrouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayerOperator::StopCrouch);

    PlayerInputComponent->BindAction("LeanLeft", IE_Pressed, this, &APlayerOperator::LeanLeft);
    PlayerInputComponent->BindAction("LeanLeft", IE_Released, this, &APlayerOperator::StopLean);

    PlayerInputComponent->BindAction("LeanRight", IE_Pressed, this, &APlayerOperator::LeanRight);
    PlayerInputComponent->BindAction("LeanRight", IE_Released, this, &APlayerOperator::StopLean);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerOperator::StartFire);
    PlayerInputComponent->BindAction("Fire", IE_Released, this, &APlayerOperator::StopFire);
}

void APlayerOperator::BeginPlay()
{
    Super::BeginPlay();

    if (GetWorld())
    {
        EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>();
    }
}

void APlayerOperator::MoveForward(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void APlayerOperator::MoveRight(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void APlayerOperator::StartSprint()
{
    GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
    UE_LOG(LogTemp, Warning, TEXT("Sprint Started"));
}

void APlayerOperator::StopSprint()
{
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    UE_LOG(LogTemp, Warning, TEXT("Sprint Stopped"));
}

void APlayerOperator::StartCrouch()
{
    Crouch();
}

void APlayerOperator::StopCrouch()
{
    UnCrouch();
}

void APlayerOperator::LeanLeft()
{
    TargetLean = -LeanOffset;
}

void APlayerOperator::LeanRight()
{
    TargetLean = LeanOffset;
}

void APlayerOperator::StopLean()
{
    TargetLean = 0.f;
}

void APlayerOperator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CurrentLean = FMath::FInterpTo(CurrentLean, TargetLean, DeltaTime, LeanSpeed);

    FVector CameraLocation = FVector(0.f, CurrentLean, 60.f);
    FirstPersonCamera->SetRelativeLocation(CameraLocation);

    float Roll = (CurrentLean / LeanOffset) * LeanAngle;
    FirstPersonCamera->SetRelativeRotation(FRotator(0.f, 0.f, Roll));
}

void APlayerOperator::StartFire()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->StartFire();
    }
}

void APlayerOperator::StopFire()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->StopFire();
    }
}

