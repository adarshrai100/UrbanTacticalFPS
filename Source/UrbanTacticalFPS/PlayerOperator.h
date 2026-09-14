#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SceneComponent.h"
#include "FPSHUDWidget.h"
#include "PlayerOperator.generated.h"

class UCameraComponent;
class USceneComponent;
class AWeaponBase;
class UUserWidget;

UCLASS()
class URBANTACTICALFPS_API APlayerOperator : public ACharacter
{
    GENERATED_BODY()

public:

    APlayerOperator();

    void AddRecoil();
    void AddFireWeaponKick();

    void ReloadWeapon();
    void UpdateAmmoUI();
    void SetMissionCompleteState();

    AWeaponBase* GetEquippedWeapon() const;
    UFPSHUDWidget* GetHUDWidget() const;

    virtual float TakeDamage(
        float DamageAmount,
        const FDamageEvent& DamageEvent,
        AController* EventInstigator,
        AActor* DamageCauser
    ) override;

protected:

    virtual void SetupPlayerInputComponent(
        class UInputComponent* PlayerInputComponent
    ) override;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void MoveForward(float Value);
    void MoveRight(float Value);

    void StartSprint();
    void StopSprint();

    void StartCrouch();
    void StopCrouch();

    float WalkSpeed = 200.f;
    float SprintSpeed = 650.f;

    float LeanOffset = 100.f;
    float CurrentLean = 0.f;
    float TargetLean = 0.f;
    float LeanSpeed = 8.f;
    float LeanAngle = 10.f;

    void LeanLeft();
    void LeanRight();
    void StopLean();

    void StartFire();
    void StopFire();

    void StartADS();
    void StopADS();

    void DebugTakeDamage();
    void Die();

    // =========================
    // WEAPON SWITCHING
    // =========================

    void EquipRifle();
    void EquipPistol();
    void EquipShotgun();
    void SwitchWeapon(AWeaponBase* NewWeapon);

    bool bIsSwitchingWeapon = false;

    AWeaponBase* PendingWeapon = nullptr;

    float WeaponSwitchTime = 0.f;

    UPROPERTY(EditAnywhere, Category = "Weapon|Switch")
    float WeaponSwitchDuration = 0.25f;

    UPROPERTY(EditAnywhere, Category = "Weapon|Switch")
    FVector WeaponSwitchOffset = FVector(0.f, 0.f, -35.f);

    // =========================
    // CAMERA
    // =========================

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FirstPersonCamera;

    // =========================
    // WEAPON
    // =========================

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    USceneComponent* WeaponPivot;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<AWeaponBase> WeaponClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<AWeaponBase> PistolClass;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<AWeaponBase> ShotgunClass;

    UPROPERTY()
    AWeaponBase* EquippedWeapon;

    UPROPERTY()
    AWeaponBase* RifleWeapon;

    UPROPERTY()
    AWeaponBase* PistolWeapon;

    UPROPERTY()
    AWeaponBase* ShotgunWeapon;

    // =========================
    // UI
    // =========================

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UFPSHUDWidget> HUDClass;

    UPROPERTY()
    TObjectPtr<UFPSHUDWidget> HUDWidget;

    // =========================
    // RECOIL
    // =========================

    float CurrentRecoilOffset = 0.f;
    float TargetRecoilOffset = 0.f;
    float PreviousRecoilOffset = 0.f;

    float CurrentYawOffset = 0.f;
    float TargetYawOffset = 0.f;
    float PreviousYawOffset = 0.f;

    // =========================
    // ADS
    // =========================

    UPROPERTY(EditAnywhere, Category = "Weapon|ADS")
    FVector HipFireLocation = FVector(25.f, 20.f, -40.f);

    UPROPERTY(EditAnywhere, Category = "Weapon|ADS")
    FVector ADSLocation = FVector(10.f, 5.f, -20.f);

    UPROPERTY(EditAnywhere, Category = "Weapon|ADS")
    float ADSInterpolationSpeed = 12.f;

    UPROPERTY(EditAnywhere, Category = "Weapon|ADS")
    float HipFOV = 90.f;

    UPROPERTY(EditAnywhere, Category = "Weapon|ADS")
    float ADSFOV = 70.f;

    UPROPERTY(EditAnywhere, Category = "Weapon|ADS")
    float FOVInterpolationSpeed = 10.f;

    UPROPERTY(EditAnywhere, Category = "Weapon|ADS")
    float ADSRecoilMultiplier = 0.5f;

    bool bIsADS = false;

    // =========================
    // HEALTH
    // =========================

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    float CurrentHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    bool bIsDead = false;

    virtual void PossessedBy(AController* NewController) override;

    // =========================
    // WEAPON SWAY
    // =========================

    UPROPERTY(EditAnywhere, Category = "Weapon|Sway")
    float SwayAmount = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Weapon|Sway")
    float SwaySpeed = 6.0f;

    UPROPERTY(EditAnywhere, Category = "Weapon|Sway")
    float MaxSway = 3.0f;

    float PreviousLookYaw = 0.f;
    float PreviousLookPitch = 0.f;

    FVector CurrentSwayOffset = FVector::ZeroVector;

    // =========================
    // FIRE WEAPON KICK
    // =========================

    UPROPERTY(EditAnywhere, Category = "Weapon|Fire")
    float FireWeaponKickAmount = 3.0f;

    UPROPERTY(EditAnywhere, Category = "Weapon|Fire")
    float FireWeaponKickSpeed = 18.0f;

    float CurrentFireWeaponKick = 0.f;
    float TargetFireWeaponKick = 0.f;

    // =========================
    // MOVEMENT WEAPON BOB
    // =========================

    UPROPERTY(EditAnywhere, Category = "Weapon|Movement Bob")
    float BobAmount = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Weapon|Movement Bob")
    float BobSpeed = 8.0f;

    UPROPERTY(EditAnywhere, Category = "Weapon|Movement Bob")
    float SprintBobAmount = 3.5f;

    UPROPERTY(EditAnywhere, Category = "Weapon|Movement Bob")
    float SprintBobSpeed = 11.0f;

    float BobTime = 0.f;

    FVector CurrentBobOffset = FVector::ZeroVector;

    // =========================
    // SPRINT WEAPON POSITION
    // =========================

    UPROPERTY(EditAnywhere, Category = "Weapon|Sprint")
    FVector SprintWeaponOffset = FVector(10.f, 0.f, -8.f);

    UPROPERTY(EditAnywhere, Category = "Weapon|Sprint")
    float SprintWeaponInterpolationSpeed = 8.0f;

    // =========================
    // IDLE WEAPON MOTION
    // =========================

    UPROPERTY(EditAnywhere, Category = "Weapon|Idle")
    float IdleMotionAmount = 0.8f;

    UPROPERTY(EditAnywhere, Category = "Weapon|Idle")
    float IdleMotionSpeed = 1.5f;

    float IdleMotionTime = 0.f;

    FVector CurrentIdleOffset = FVector::ZeroVector;

    // =========================
    // BASE WEAPON POSITION
    // =========================

    FVector CurrentWeaponBaseLocation = FVector(40.f, 20.f, -35.f);
};