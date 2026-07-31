#include "EnemyBase.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
}

void AEnemyBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

float AEnemyBase::TakeDamage(
    float DamageAmount,
    const FDamageEvent& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    CurrentHealth -= DamageAmount;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Enemy Health: %.0f"),
        CurrentHealth
    );

    if (CurrentHealth <= 0.f)
    {
        Die();
    }

    return DamageAmount;
}

void AEnemyBase::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("ENEMY DEAD"));

    Destroy();
}