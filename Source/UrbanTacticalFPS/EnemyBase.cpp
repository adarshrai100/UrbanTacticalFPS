#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

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

    if (HitReactionMontage)
    {
        PlayAnimMontage(HitReactionMontage);
    }

    if (CurrentHealth <= 0.f)
    {
        Die();
    }

    return DamageAmount;
}

void AEnemyBase::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("ENEMY DEAD"));
    StopAttacking();
    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage);
    }

    SetActorEnableCollision(false);

    GetCharacterMovement()->DisableMovement();

    GetWorldTimerManager().SetTimer(
        DeathTimerHandle,
        this,
        &AEnemyBase::DestroyEnemy,
        3.0f,
        false
    );
}

void AEnemyBase::AttackPlayer()
{
    AActor* PlayerActor =
        UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    if (PlayerActor)
    {
        UGameplayStatics::ApplyDamage(
            PlayerActor,
            AttackDamage,
            GetController(),
            this,
            UDamageType::StaticClass()
        );
    }
}

void AEnemyBase::StartAttacking()
{
    if (!GetWorldTimerManager().IsTimerActive(AttackTimerHandle))
    {
        GetWorldTimerManager().SetTimer(
            AttackTimerHandle,
            this,
            &AEnemyBase::AttackPlayer,
            AttackInterval,
            true
        );
    }
}

void AEnemyBase::StopAttacking()
{
    GetWorldTimerManager().ClearTimer(
        AttackTimerHandle
    );
}

void AEnemyBase::DestroyEnemy()
{
    Destroy();
}