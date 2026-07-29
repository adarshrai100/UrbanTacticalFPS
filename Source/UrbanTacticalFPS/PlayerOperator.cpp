// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerOperator.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WeaponBase.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"

// Sets default values
APlayerOperator::APlayerOperator()
{
    PrimaryActorTick.bCanEverTick = true;

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
    FirstPersonCamera->bUsePawnControlRotation = true;

    WeaponPivot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponPivot"));
    WeaponPivot->SetupAttachment(FirstPersonCamera);
    WeaponPivot->SetRelativeLocation(FVector(40.f, 20.f, -35.f));
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

    PlayerInputComponent->BindAction("ADS", IE_Pressed, this, &APlayerOperator::StartADS);
    PlayerInputComponent->BindAction("ADS", IE_Released, this, &APlayerOperator::StopADS);

    PlayerInputComponent->BindAction(
        "Reload",
        IE_Pressed,
        this,
        &APlayerOperator::ReloadWeapon
    );
}

void APlayerOperator::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("=== PlayerOperator Compiled Successfully ==="));

    if (WeaponClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponClass is VALID"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("WeaponClass is NULL"));
    }

    if (GetWorld())
    {
        if (WeaponClass)
        {
            EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);

            if (EquippedWeapon)
            {
                EquippedWeapon->SetOwner(this);

                EquippedWeapon->AttachToComponent(
                    WeaponPivot,
                    FAttachmentTransformRules::SnapToTargetNotIncludingScale
                );
            }
        }
    }
    if (CrosshairClass)
    {
        CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairClass);

        if (CrosshairWidget)
        {
            CrosshairWidget->AddToViewport();
        }
    }
    FirstPersonCamera->SetFieldOfView(HipFOV);
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

    // Smooth Lean
    CurrentLean = FMath::FInterpTo(
        CurrentLean,
        TargetLean,
        DeltaTime,
        LeanSpeed
    );

    CurrentYawOffset = FMath::FInterpTo(
        CurrentYawOffset,
        TargetYawOffset,
        DeltaTime,
        25.f
    );

    // Lean Position
    FVector CameraLocation(0.f, CurrentLean, 60.f);
    FirstPersonCamera->SetRelativeLocation(CameraLocation);

    // Smooth ADS weapon movement
    FVector TargetWeaponLocation = bIsADS ? ADSLocation : HipFireLocation;

    FVector NewWeaponLocation = FMath::VInterpTo(
        WeaponPivot->GetRelativeLocation(),
        TargetWeaponLocation,
        DeltaTime,
        ADSInterpolationSpeed
    );

    WeaponPivot->SetRelativeLocation(NewWeaponLocation);

    // Lean Rotation
    float Roll = (CurrentLean / LeanOffset) * LeanAngle;
    FirstPersonCamera->SetRelativeRotation(
        FRotator(0.f, 0.f, Roll)
    );

    // Smooth Recoil
    CurrentRecoilOffset = FMath::FInterpTo(
        CurrentRecoilOffset,
        TargetRecoilOffset,
        DeltaTime,
        25.f
    );

    float DeltaPitch = CurrentRecoilOffset - PreviousRecoilOffset;
    float DeltaYaw = CurrentYawOffset - PreviousYawOffset;

    AddControllerPitchInput(-DeltaPitch);
    AddControllerYawInput(DeltaYaw);

    PreviousRecoilOffset = CurrentRecoilOffset;
    PreviousYawOffset = CurrentYawOffset;

    // Recover recoil
    TargetRecoilOffset = FMath::FInterpTo(
        TargetRecoilOffset,
        0.f,
        DeltaTime,
        EquippedWeapon ? EquippedWeapon->GetRecoilRecoverySpeed() : 10.f
    );

    TargetYawOffset = FMath::FInterpTo(
        TargetYawOffset,
        0.f,
        DeltaTime,
        EquippedWeapon ? EquippedWeapon->GetRecoilRecoverySpeed() : 10.f
    );

    // Smooth ADS camera zoom
    float TargetFOV = bIsADS ? ADSFOV : HipFOV;

    float NewFOV = FMath::FInterpTo(
        FirstPersonCamera->FieldOfView,
        TargetFOV,
        DeltaTime,
        FOVInterpolationSpeed
    );

    FirstPersonCamera->SetFieldOfView(NewFOV);
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

void APlayerOperator::AddRecoil()
{
    if (!EquippedWeapon)
    {
        return;
    }

    float RecoilMultiplier = bIsADS ? ADSRecoilMultiplier : 1.0f;

    // Vertical recoil
    TargetRecoilOffset += EquippedWeapon->GetVerticalRecoil() * RecoilMultiplier;
    TargetRecoilOffset = FMath::Clamp(TargetRecoilOffset, 0.f, 15.f);

    // Horizontal recoil
    TargetYawOffset += FMath::FRandRange(
        -EquippedWeapon->GetHorizontalRecoil(),
        EquippedWeapon->GetHorizontalRecoil()
    ) * RecoilMultiplier;

    TargetYawOffset = FMath::Clamp(
        TargetYawOffset,
        -4.f,
        4.f
    );
}

void APlayerOperator::StartADS()
{
    bIsADS = true;

    if (CrosshairWidget)
    {
        CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}

void APlayerOperator::StopADS()
{
    bIsADS = false;

    if (CrosshairWidget)
    {
        CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
    }
}

void APlayerOperator::ReloadWeapon()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->Reload();
    }
}

