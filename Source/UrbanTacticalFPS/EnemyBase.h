#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

class USceneComponent;
class UPointLightComponent;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class URBANTACTICALFPS_API AEnemyBase : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyBase();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    float CurrentHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    bool bIsDead = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackRange = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackStartRange = 250.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackDamage = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackInterval = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    bool bIsAttacking = false;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> HitReactionMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> DeathMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    TObjectPtr<UAnimMontage> FireMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    float HitReactionCooldown = 0.5f;

    float LastHitReactionTime = -100.f;

    virtual float TakeDamage(
        float DamageAmount,
        const FDamageEvent& DamageEvent,
        AController* EventInstigator,
        AActor* DamageCauser
    ) override;

    void Die();
    void DestroyEnemy();

    FTimerHandle AttackTimerHandle;
    FTimerHandle DeathTimerHandle;
    FTimerHandle MuzzleFlashTimer;
    void StartAttacking();
    void StopAttacking();
    void AttackPlayer();

    void ShowMuzzleFlash();
    void HideMuzzleFlash();



    UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USceneComponent> EnemyMuzzlePoint;

    UPROPERTY()
    TObjectPtr<UPointLightComponent> MuzzleFlashLight;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    TObjectPtr<UNiagaraSystem> MuzzleFlashEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundBase> EnemyFireSound;
};