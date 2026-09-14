// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerOperator.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "WeaponBase.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DamageEvents.h"
#include "UrbanTacticalFPSGameMode.h"

// Sets default values
APlayerOperator::APlayerOperator()
{
    PrimaryActorTick.bCanEverTick = true;

    FirstPersonCamera =
        CreateDefaultSubobject<UCameraComponent>(
            TEXT("FirstPersonCamera")
        );

    FirstPersonCamera->SetupAttachment(
        GetCapsuleComponent()
    );

    FirstPersonCamera->SetRelativeLocation(
        FVector(0.f, 0.f, 60.f)
    );

    FirstPersonCamera->bUsePawnControlRotation = true;

    WeaponPivot =
        CreateDefaultSubobject<USceneComponent>(
            TEXT("WeaponPivot")
        );

    WeaponPivot->SetupAttachment(
        FirstPersonCamera
    );

    WeaponPivot->SetRelativeLocation(
        HipFireLocation
    );

    CurrentWeaponBaseLocation =
        HipFireLocation;

    GetCharacterMovement()->MaxWalkSpeed =
        WalkSpeed;

    GetCharacterMovement()->NavAgentProps.bCanCrouch =
        true;

    GetCharacterMovement()->BrakingDecelerationWalking =
        2048.f;

    GetCharacterMovement()->GroundFriction =
        8.f;

    bUseControllerRotationYaw = true;

    GetCharacterMovement()->bOrientRotationToMovement =
        false;

    GetMesh()->SetOwnerNoSee(true);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("PlayerOperator Active")
    );
}


// =========================
// INPUT
// =========================

void APlayerOperator::SetupPlayerInputComponent(
    UInputComponent* PlayerInputComponent
)
{
    Super::SetupPlayerInputComponent(
        PlayerInputComponent
    );

    PlayerInputComponent->BindAxis(
        "MoveForward",
        this,
        &APlayerOperator::MoveForward
    );

    PlayerInputComponent->BindAxis(
        "MoveRight",
        this,
        &APlayerOperator::MoveRight
    );

    PlayerInputComponent->BindAxis(
        "Turn",
        this,
        &APawn::AddControllerYawInput
    );

    PlayerInputComponent->BindAxis(
        "LookUp",
        this,
        &APawn::AddControllerPitchInput
    );

    PlayerInputComponent->BindAction(
        "Sprint",
        IE_Pressed,
        this,
        &APlayerOperator::StartSprint
    );

    PlayerInputComponent->BindAction(
        "Sprint",
        IE_Released,
        this,
        &APlayerOperator::StopSprint
    );

    PlayerInputComponent->BindAction(
        "Crouch",
        IE_Pressed,
        this,
        &APlayerOperator::StartCrouch
    );

    PlayerInputComponent->BindAction(
        "Crouch",
        IE_Released,
        this,
        &APlayerOperator::StopCrouch
    );

    PlayerInputComponent->BindAction(
        "LeanLeft",
        IE_Pressed,
        this,
        &APlayerOperator::LeanLeft
    );

    PlayerInputComponent->BindAction(
        "LeanLeft",
        IE_Released,
        this,
        &APlayerOperator::StopLean
    );

    PlayerInputComponent->BindAction(
        "LeanRight",
        IE_Pressed,
        this,
        &APlayerOperator::LeanRight
    );

    PlayerInputComponent->BindAction(
        "LeanRight",
        IE_Released,
        this,
        &APlayerOperator::StopLean
    );

    PlayerInputComponent->BindAction(
        "Fire",
        IE_Pressed,
        this,
        &APlayerOperator::StartFire
    );

    PlayerInputComponent->BindAction(
        "Fire",
        IE_Released,
        this,
        &APlayerOperator::StopFire
    );

    PlayerInputComponent->BindAction(
        "ADS",
        IE_Pressed,
        this,
        &APlayerOperator::StartADS
    );

    PlayerInputComponent->BindAction(
        "ADS",
        IE_Released,
        this,
        &APlayerOperator::StopADS
    );

    PlayerInputComponent->BindAction(
        "Reload",
        IE_Pressed,
        this,
        &APlayerOperator::ReloadWeapon
    );

    PlayerInputComponent->BindAction(
        "DebugDamage",
        IE_Pressed,
        this,
        &APlayerOperator::DebugTakeDamage
    );

    PlayerInputComponent->BindAction(
        "WeaponRifle",
        IE_Pressed,
        this,
        &APlayerOperator::EquipRifle
    );

    PlayerInputComponent->BindAction(
        "WeaponPistol",
        IE_Pressed,
        this,
        &APlayerOperator::EquipPistol
    );

    PlayerInputComponent->BindAction(
        "WeaponShotgun",
        IE_Pressed,
        this,
        &APlayerOperator::EquipShotgun
    );
}


// =========================
// BEGIN PLAY
// =========================

void APlayerOperator::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("=== PlayerOperator Compiled Successfully ===")
    );

    CurrentWeaponBaseLocation =
        HipFireLocation;

    if (GetWorld())
    {
        if (WeaponClass)
        {
            RifleWeapon =
                GetWorld()->SpawnActor<AWeaponBase>(
                    WeaponClass
                );

            if (RifleWeapon)
            {
                RifleWeapon->SetOwner(this);

                RifleWeapon->AttachToComponent(
                    WeaponPivot,
                    FAttachmentTransformRules::SnapToTargetNotIncludingScale
                );

                EquippedWeapon =
                    RifleWeapon;
            }
        }

        if (PistolClass)
        {
            PistolWeapon =
                GetWorld()->SpawnActor<AWeaponBase>(
                    PistolClass
                );

            if (PistolWeapon)
            {
                PistolWeapon->SetOwner(this);

                PistolWeapon->AttachToComponent(
                    WeaponPivot,
                    FAttachmentTransformRules::SnapToTargetNotIncludingScale
                );

                PistolWeapon->SetActorHiddenInGame(
                    true
                );
            }
        }

        if (ShotgunClass)
        {
            ShotgunWeapon =
                GetWorld()->SpawnActor<AWeaponBase>(
                    ShotgunClass
                );

            if (ShotgunWeapon)
            {
                ShotgunWeapon->SetOwner(this);

                ShotgunWeapon->AttachToComponent(
                    WeaponPivot,
                    FAttachmentTransformRules::SnapToTargetNotIncludingScale
                );

                ShotgunWeapon->SetActorHiddenInGame(
                    true
                );
            }
        }
    }

    if (HUDClass)
    {
        HUDWidget =
            CreateWidget<UFPSHUDWidget>(
                GetWorld(),
                HUDClass,
                TEXT("HUD")
            );

        if (HUDWidget)
        {
            HUDWidget->AddToViewport();

            HUDWidget->HideGameOver();

            UpdateAmmoUI();

            if (AUrbanTacticalFPSGameMode* GameMode =
                GetWorld()->GetAuthGameMode<AUrbanTacticalFPSGameMode>())
            {
                GameMode->RefreshObjective();
            }
        }
    }

    FirstPersonCamera->SetFieldOfView(
        HipFOV
    );

    CurrentHealth =
        MaxHealth;

    bIsDead =
        false;

    if (APlayerController* PlayerController =
        Cast<APlayerController>(GetController()))
    {
        PlayerController->bShowMouseCursor =
            false;

        FInputModeGameOnly InputMode;

        PlayerController->SetInputMode(
            InputMode
        );
    }
}


// =========================
// MOVEMENT
// =========================

void APlayerOperator::MoveForward(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator Rotation =
            Controller->GetControlRotation();

        const FRotator YawRotation(
            0.f,
            Rotation.Yaw,
            0.f
        );

        const FVector Direction =
            FRotationMatrix(
                YawRotation
            ).GetUnitAxis(
                EAxis::X
            );

        AddMovementInput(
            Direction,
            Value
        );
    }
}

void APlayerOperator::MoveRight(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator Rotation =
            Controller->GetControlRotation();

        const FRotator YawRotation(
            0.f,
            Rotation.Yaw,
            0.f
        );

        const FVector Direction =
            FRotationMatrix(
                YawRotation
            ).GetUnitAxis(
                EAxis::Y
            );

        AddMovementInput(
            Direction,
            Value
        );
    }
}


// =========================
// SPRINT
// =========================

void APlayerOperator::StartSprint()
{
    GetCharacterMovement()->MaxWalkSpeed =
        SprintSpeed;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Sprint Started")
    );
}

void APlayerOperator::StopSprint()
{
    GetCharacterMovement()->MaxWalkSpeed =
        WalkSpeed;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Sprint Stopped")
    );
}


// =========================
// CROUCH
// =========================

void APlayerOperator::StartCrouch()
{
    Crouch();
}

void APlayerOperator::StopCrouch()
{
    UnCrouch();
}


// =========================
// LEAN
// =========================

void APlayerOperator::LeanLeft()
{
    TargetLean =
        -LeanOffset;
}

void APlayerOperator::LeanRight()
{
    TargetLean =
        LeanOffset;
}

void APlayerOperator::StopLean()
{
    TargetLean =
        0.f;
}


// =========================
// TICK
// =========================

void APlayerOperator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // =========================
    // SMOOTH LEAN
    // =========================

    CurrentLean =
        FMath::FInterpTo(
            CurrentLean,
            TargetLean,
            DeltaTime,
            LeanSpeed
        );

    CurrentYawOffset =
        FMath::FInterpTo(
            CurrentYawOffset,
            TargetYawOffset,
            DeltaTime,
            25.f
        );

    // =========================
    // CAMERA LEAN
    // =========================

    FVector CameraLocation(
        0.f,
        CurrentLean,
        60.f
    );

    FirstPersonCamera->SetRelativeLocation(
        CameraLocation
    );

    // =========================
    // MOVEMENT STATE
    // =========================

    FVector Velocity =
        GetVelocity();

    Velocity.Z =
        0.f;

    float MovementSpeed =
        Velocity.Size();

    bool bIsMoving =
        MovementSpeed > 10.f;

    bool bIsSprinting =
        MovementSpeed > WalkSpeed + 50.f;

    bool bIsGrounded =
        GetCharacterMovement()->IsMovingOnGround();

    // =========================
    // BASE WEAPON POSITION
    // =========================

    FVector TargetBaseLocation =
        bIsADS
        ? ADSLocation
        : HipFireLocation;

    if (!bIsADS && bIsSprinting)
    {
        TargetBaseLocation +=
            SprintWeaponOffset;
    }

    CurrentWeaponBaseLocation =
        FMath::VInterpTo(
            CurrentWeaponBaseLocation,
            TargetBaseLocation,
            DeltaTime,
            ADSInterpolationSpeed
        );

    // =========================
    // WEAPON SWAY
    // =========================

    float CurrentLookYaw =
        GetControlRotation().Yaw;

    float CurrentLookPitch =
        GetControlRotation().Pitch;

    float LookYawDelta =
        FMath::FindDeltaAngleDegrees(
            PreviousLookYaw,
            CurrentLookYaw
        );

    float LookPitchDelta =
        FMath::FindDeltaAngleDegrees(
            PreviousLookPitch,
            CurrentLookPitch
        );

    PreviousLookYaw =
        CurrentLookYaw;

    PreviousLookPitch =
        CurrentLookPitch;

    float SwayX =
        FMath::Clamp(
            LookYawDelta * SwayAmount,
            -MaxSway,
            MaxSway
        );

    float SwayZ =
        FMath::Clamp(
            -LookPitchDelta * SwayAmount,
            -MaxSway,
            MaxSway
        );

    FVector TargetSwayOffset(
        SwayX,
        0.f,
        SwayZ
    );

    CurrentSwayOffset =
        FMath::VInterpTo(
            CurrentSwayOffset,
            TargetSwayOffset,
            DeltaTime,
            SwaySpeed
        );

    // =========================
    // MOVEMENT WEAPON BOB
    // =========================

    FVector TargetBobOffset =
        FVector::ZeroVector;

    if (bIsMoving && bIsGrounded)
    {
        float CurrentBobSpeed =
            bIsSprinting
            ? SprintBobSpeed
            : BobSpeed;

        float CurrentBobAmount =
            bIsSprinting
            ? SprintBobAmount
            : BobAmount;

        BobTime +=
            DeltaTime *
            CurrentBobSpeed;

        float BobVertical =
            FMath::Sin(BobTime) *
            CurrentBobAmount;

        float BobHorizontal =
            FMath::Cos(
                BobTime * 0.5f
            ) *
            CurrentBobAmount *
            0.5f;

        TargetBobOffset =
            FVector(
                0.f,
                BobHorizontal,
                BobVertical
            );
    }
    else
    {
        BobTime =
            0.f;
    }

    CurrentBobOffset =
        FMath::VInterpTo(
            CurrentBobOffset,
            TargetBobOffset,
            DeltaTime,
            10.f
        );

    // =========================
    // IDLE WEAPON MOTION
    // =========================

    FVector TargetIdleOffset =
        FVector::ZeroVector;

    if (!bIsMoving &&
        !bIsSprinting &&
        !bIsADS &&
        bIsGrounded &&
        !bIsSwitchingWeapon)
    {
        IdleMotionTime +=
            DeltaTime *
            IdleMotionSpeed;

        float IdleVertical =
            FMath::Sin(
                IdleMotionTime
            ) *
            IdleMotionAmount;

        float IdleHorizontal =
            FMath::Cos(
                IdleMotionTime * 0.5f
            ) *
            IdleMotionAmount *
            0.5f;

        TargetIdleOffset =
            FVector(
                0.f,
                IdleHorizontal,
                IdleVertical
            );
    }
    else
    {
        IdleMotionTime =
            0.f;
    }

    CurrentIdleOffset =
        FMath::VInterpTo(
            CurrentIdleOffset,
            TargetIdleOffset,
            DeltaTime,
            6.f
        );

    // =========================
    // FIRE WEAPON KICK
    // =========================

    CurrentFireWeaponKick =
        FMath::FInterpTo(
            CurrentFireWeaponKick,
            TargetFireWeaponKick,
            DeltaTime,
            FireWeaponKickSpeed
        );

    TargetFireWeaponKick =
        FMath::FInterpTo(
            TargetFireWeaponKick,
            0.f,
            DeltaTime,
            FireWeaponKickSpeed
        );

    FVector FireKickOffset(
        -CurrentFireWeaponKick,
        0.f,
        0.f
    );

    // =========================
    // WEAPON SWITCHING
    // =========================

    FVector SwitchOffset =
        FVector::ZeroVector;

    if (bIsSwitchingWeapon &&
        PendingWeapon)
    {
        WeaponSwitchTime +=
            DeltaTime;

        float SwitchProgress =
            FMath::Clamp(
                WeaponSwitchTime /
                WeaponSwitchDuration,
                0.f,
                1.f
            );

        float SwitchAmount = 0.f;

        if (SwitchProgress < 0.5f)
        {
            // Lower weapon
            SwitchAmount =
                SwitchProgress / 0.5f;
        }
        else
        {
            // Raise weapon
            SwitchAmount =
                1.f -
                ((SwitchProgress - 0.5f) / 0.5f);

            // Perform actual swap once
            if (EquippedWeapon != PendingWeapon)
            {
                if (EquippedWeapon)
                {
                    EquippedWeapon->SetActorHiddenInGame(
                        true
                    );
                }

                EquippedWeapon =
                    PendingWeapon;

                EquippedWeapon->SetActorHiddenInGame(
                    false
                );

                UpdateAmmoUI();
            }
        }

        SwitchOffset =
            WeaponSwitchOffset *
            SwitchAmount;

        if (SwitchProgress >= 1.f)
        {
            bIsSwitchingWeapon =
                false;

            PendingWeapon =
                nullptr;

            WeaponSwitchTime =
                0.f;
        }
    }

    // =========================
    // FINAL WEAPON POSITION
    // =========================

    FVector FinalWeaponLocation =
        CurrentWeaponBaseLocation;

    FinalWeaponLocation +=
        CurrentSwayOffset;

    FinalWeaponLocation +=
        CurrentBobOffset;

    FinalWeaponLocation +=
        CurrentIdleOffset;

    FinalWeaponLocation +=
        FireKickOffset;

    FinalWeaponLocation +=
        SwitchOffset;

    WeaponPivot->SetRelativeLocation(
        FinalWeaponLocation
    );

    // =========================
    // LEAN ROTATION
    // =========================

    float Roll =
        (CurrentLean / LeanOffset) *
        LeanAngle;

    FirstPersonCamera->SetRelativeRotation(
        FRotator(
            0.f,
            0.f,
            Roll
        )
    );

    // =========================
    // SMOOTH RECOIL
    // =========================

    CurrentRecoilOffset =
        FMath::FInterpTo(
            CurrentRecoilOffset,
            TargetRecoilOffset,
            DeltaTime,
            25.f
        );

    float DeltaPitch =
        CurrentRecoilOffset -
        PreviousRecoilOffset;

    float DeltaYaw =
        CurrentYawOffset -
        PreviousYawOffset;

    AddControllerPitchInput(
        -DeltaPitch
    );

    AddControllerYawInput(
        DeltaYaw
    );

    PreviousRecoilOffset =
        CurrentRecoilOffset;

    PreviousYawOffset =
        CurrentYawOffset;

    // =========================
    // RECOVER RECOIL
    // =========================

    TargetRecoilOffset =
        FMath::FInterpTo(
            TargetRecoilOffset,
            0.f,
            DeltaTime,
            EquippedWeapon
            ? EquippedWeapon->GetRecoilRecoverySpeed()
            : 10.f
        );

    TargetYawOffset =
        FMath::FInterpTo(
            TargetYawOffset,
            0.f,
            DeltaTime,
            EquippedWeapon
            ? EquippedWeapon->GetRecoilRecoverySpeed()
            : 10.f
        );

    // =========================
    // SMOOTH ADS CAMERA ZOOM
    // =========================

    float TargetFOV =
        bIsADS
        ? ADSFOV
        : HipFOV;

    float NewFOV =
        FMath::FInterpTo(
            FirstPersonCamera->FieldOfView,
            TargetFOV,
            DeltaTime,
            FOVInterpolationSpeed
        );

    FirstPersonCamera->SetFieldOfView(
        NewFOV
    );
}


// =========================
// FIRE
// =========================

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


// =========================
// RECOIL
// =========================

void APlayerOperator::AddRecoil()
{
    if (!EquippedWeapon)
    {
        return;
    }

    float RecoilMultiplier =
        bIsADS
        ? ADSRecoilMultiplier
        : 1.0f;

    TargetRecoilOffset +=
        EquippedWeapon->GetVerticalRecoil() *
        RecoilMultiplier;

    TargetRecoilOffset =
        FMath::Clamp(
            TargetRecoilOffset,
            0.f,
            15.f
        );

    TargetYawOffset +=
        FMath::FRandRange(
            -EquippedWeapon->GetHorizontalRecoil(),
            EquippedWeapon->GetHorizontalRecoil()
        ) *
        RecoilMultiplier;

    TargetYawOffset =
        FMath::Clamp(
            TargetYawOffset,
            -4.f,
            4.f
        );
}


// =========================
// FIRE WEAPON KICK
// =========================

void APlayerOperator::AddFireWeaponKick()
{
    float KickMultiplier =
        bIsADS
        ? 0.5f
        : 1.0f;

    TargetFireWeaponKick +=
        FireWeaponKickAmount *
        KickMultiplier;

    TargetFireWeaponKick =
        FMath::Clamp(
            TargetFireWeaponKick,
            0.f,
            FireWeaponKickAmount * 2.f
        );
}


// =========================
// ADS
// =========================

void APlayerOperator::StartADS()
{
    if (!EquippedWeapon)
    {
        return;
    }

    if (!EquippedWeapon->bCanADS)
    {
        return;
    }

    if (bIsSwitchingWeapon)
    {
        return;
    }

    bIsADS =
        true;

    if (HUDWidget)
    {
        HUDWidget->HideCrosshair();
    }
}

void APlayerOperator::StopADS()
{
    bIsADS =
        false;

    if (HUDWidget)
    {
        HUDWidget->ShowCrosshair();
    }
}


// =========================
// RELOAD
// =========================

void APlayerOperator::ReloadWeapon()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->Reload();
    }
}


// =========================
// WEAPON / HUD
// =========================

AWeaponBase* APlayerOperator::GetEquippedWeapon() const
{
    return EquippedWeapon;
}

void APlayerOperator::UpdateAmmoUI()
{
    if (HUDWidget &&
        EquippedWeapon)
    {
        HUDWidget->SetAmmo(
            EquippedWeapon->GetCurrentAmmo(),
            EquippedWeapon->GetReserveAmmo()
        );
    }
}


// =========================
// DAMAGE
// =========================

float APlayerOperator::TakeDamage(
    float DamageAmount,
    const FDamageEvent& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    if (bIsDead)
    {
        return 0.0f;
    }

    CurrentHealth -=
        DamageAmount;

    if (HUDWidget)
    {
        HUDWidget->ShowDamageEffect();
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Player Health: %.0f / %.0f"),
        CurrentHealth,
        MaxHealth
    );

    if (CurrentHealth <= 0.0f)
    {
        CurrentHealth =
            0.0f;

        Die();
    }

    return DamageAmount;
}

void APlayerOperator::DebugTakeDamage()
{
    TakeDamage(
        25.0f,
        FDamageEvent(),
        nullptr,
        nullptr
    );
}


// =========================
// DEATH
// =========================

void APlayerOperator::Die()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead =
        true;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("PLAYER DEAD")
    );

    if (HUDWidget)
    {
        HUDWidget->ShowGameOver();
    }

    DisableInput(nullptr);

    if (APlayerController* PlayerController =
        Cast<APlayerController>(GetController()))
    {
        PlayerController->bShowMouseCursor =
            true;

        FInputModeUIOnly InputMode;

        InputMode.SetWidgetToFocus(
            HUDWidget->TakeWidget()
        );

        InputMode.SetLockMouseToViewportBehavior(
            EMouseLockMode::DoNotLock
        );

        PlayerController->SetInputMode(
            InputMode
        );
    }

    if (EquippedWeapon)
    {
        EquippedWeapon->StopFire();
    }

    GetCharacterMovement()->DisableMovement();
}


// =========================
// POSSESSION
// =========================

void APlayerOperator::PossessedBy(
    AController* NewController
)
{
    Super::PossessedBy(
        NewController
    );

    if (APlayerController* PlayerController =
        Cast<APlayerController>(NewController))
    {
        EnableInput(
            PlayerController
        );

        PlayerController->SetIgnoreMoveInput(
            false
        );

        PlayerController->SetIgnoreLookInput(
            false
        );

        GetCharacterMovement()->SetMovementMode(
            MOVE_Walking
        );

        PlayerController->bShowMouseCursor =
            false;

        FInputModeGameOnly InputMode;

        PlayerController->SetInputMode(
            InputMode
        );

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("PLAYER POSSESSED - INPUT AND MOVEMENT RESTORED")
        );
    }
}


// =========================
// HUD
// =========================

UFPSHUDWidget* APlayerOperator::GetHUDWidget() const
{
    return HUDWidget;
}


// =========================
// MISSION COMPLETE
// =========================

void APlayerOperator::SetMissionCompleteState()
{
    if (bIsDead)
    {
        return;
    }

    DisableInput(nullptr);

    if (EquippedWeapon)
    {
        EquippedWeapon->StopFire();
    }

    if (APlayerController* PlayerController =
        Cast<APlayerController>(GetController()))
    {
        PlayerController->bShowMouseCursor =
            true;

        FInputModeUIOnly InputMode;

        if (HUDWidget)
        {
            InputMode.SetWidgetToFocus(
                HUDWidget->TakeWidget()
            );
        }

        InputMode.SetLockMouseToViewportBehavior(
            EMouseLockMode::DoNotLock
        );

        PlayerController->SetInputMode(
            InputMode
        );
    }

    GetCharacterMovement()->DisableMovement();

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("MISSION COMPLETE - PLAYER CONTROL LOCKED")
    );
}


// =========================
// WEAPON EQUIP
// =========================

void APlayerOperator::EquipRifle()
{
    if (RifleWeapon)
    {
        SwitchWeapon(
            RifleWeapon
        );
    }
}

void APlayerOperator::EquipPistol()
{
    if (PistolWeapon)
    {
        SwitchWeapon(
            PistolWeapon
        );
    }
}

void APlayerOperator::EquipShotgun()
{
    if (ShotgunWeapon)
    {
        SwitchWeapon(
            ShotgunWeapon
        );
    }
}


// =========================
// WEAPON SWITCH
// =========================

void APlayerOperator::SwitchWeapon(
    AWeaponBase* NewWeapon
)
{
    if (!NewWeapon ||
        NewWeapon == EquippedWeapon)
    {
        return;
    }

    if (bIsSwitchingWeapon)
    {
        return;
    }

    if (EquippedWeapon)
    {
        EquippedWeapon->StopFire();
    }

    PendingWeapon =
        NewWeapon;

    bIsSwitchingWeapon =
        true;

    WeaponSwitchTime =
        0.f;

    bIsADS =
        false;

    if (HUDWidget)
    {
        HUDWidget->ShowCrosshair();
    }
}