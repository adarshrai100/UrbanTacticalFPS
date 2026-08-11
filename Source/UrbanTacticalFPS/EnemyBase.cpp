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
    bIsDead = true;
    UE_LOG(LogTemp, Warning, TEXT("ENEMY DEAD"));
    StopAttacking();

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
        UE_LOG(LogTemp, Warning, TEXT("ENEMY FIRING"));
    }
}

void AEnemyBase::StartAttacking()
{
    if (!bIsAttacking)
    {
        bIsAttacking = true;

        if (FireMontage)
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("FireMontage: %s | Length: %f"),
                *FireMontage->GetName(),
                FireMontage->GetPlayLength()
            );

            float MontageLength = PlayAnimMontage(FireMontage);

            UE_LOG(
                LogTemp,
                Warning,
                TEXT("PlayAnimMontage Result: %f"),
                MontageLength
            );
        }
        UE_LOG(LogTemp, Warning, TEXT("ENEMY START ATTACKING"));
        AttackPlayer();
    }

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
    UE_LOG(LogTemp, Warning, TEXT("ENEMY STOP ATTACKING"));

    bIsAttacking = false;

    GetWorldTimerManager().ClearTimer(
        AttackTimerHandle
    );

    if (FireMontage)
    {
        StopAnimMontage(FireMontage);
    }
}

void AEnemyBase::DestroyEnemy()
{
    Destroy();
}