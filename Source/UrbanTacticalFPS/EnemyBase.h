#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackRange = 150.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackStartRange = 250.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackDamage = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    float AttackInterval = 1.0f;

    virtual float TakeDamage(
        float DamageAmount,
        const FDamageEvent& DamageEvent,
        AController* EventInstigator,
        AActor* DamageCauser
    ) override;

    void Die();

    FTimerHandle AttackTimerHandle;
    void StartAttacking();
    void StopAttacking();
    void AttackPlayer();
};