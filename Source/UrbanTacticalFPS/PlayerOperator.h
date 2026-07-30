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
    void ReloadWeapon();
    void UpdateAmmoUI();


    AWeaponBase* GetEquippedWeapon() const;

protected:

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
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




    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    USceneComponent* WeaponPivot;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<AWeaponBase> WeaponClass;

    UPROPERTY()
    AWeaponBase* EquippedWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UFPSHUDWidget> HUDClass;

    UPROPERTY()
    TObjectPtr<UFPSHUDWidget> HUDWidget;




    float CurrentRecoilOffset = 0.f;
    float TargetRecoilOffset = 0.f;
    float PreviousRecoilOffset = 0.f;   
    float CurrentYawOffset = 0.f;
    float TargetYawOffset = 0.f;
    float PreviousYawOffset = 0.f;

    // ADS
    UPROPERTY(EditAnywhere, Category = "Weapon|ADS")
    FVector HipFireLocation = FVector(40.f, 20.f, -35.f);

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



};