#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

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
    if (bIsDead)
    {
        return 0.0f;
    }
    CurrentHealth -= DamageAmount;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Enemy Health: %.0f"),
        CurrentHealth
    );

    if (HitReactionMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

        const float CurrentTime = GetWorld()->GetTimeSeconds();

        if (AnimInstance &&
            !AnimInstance->Montage_IsPlaying(HitReactionMontage) &&
            CurrentTime - LastHitReactionTime >= HitReactionCooldown)
        {
            PlayAnimMontage(HitReactionMontage);
            LastHitReactionTime = CurrentTime;
        }
    }

    if (CurrentHealth <= 0.f)
    {
        Die();
    }

    return DamageAmount;
}

void AEnemyBase::Die()
{
    if (bIsDead)
    {
        return;
    }

    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("ENEMY DEAD"));

    StopAttacking();

    // Stop any currently playing combat animations
    if (FireMontage)
    {
        StopAnimMontage(FireMontage);
    }

    if (HitReactionMontage)
    {
        StopAnimMontage(HitReactionMontage);
    }

    // Play death animation
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