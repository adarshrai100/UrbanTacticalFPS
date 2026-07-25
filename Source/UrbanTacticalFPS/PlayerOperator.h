#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SceneComponent.h"
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


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    USceneComponent* WeaponPivot;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<AWeaponBase> WeaponClass;

    UPROPERTY()
    AWeaponBase* EquippedWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> CrosshairClass;

    UUserWidget* CrosshairWidget;
};