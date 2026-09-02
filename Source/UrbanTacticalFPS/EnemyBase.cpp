#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "UrbanTacticalFPSGameMode.h"
#include "Components/SceneComponent.h"
#include "Components/PointLightComponent.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

AEnemyBase::AEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;

    MuzzleFlashLight = CreateDefaultSubobject<UPointLightComponent>(
        TEXT("MuzzleFlashLight")
    );

    MuzzleFlashLight->SetupAttachment(RootComponent);

    MuzzleFlashLight->SetIntensity(0.0f);

    MuzzleFlashLight->SetLightColor(
        FColor(255, 180, 80)
    );

    MuzzleFlashLight->SetAttenuationRadius(250.0f);
}

void AEnemyBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    EnemyMuzzlePoint = FindComponentByClass<USceneComponent>();
    TArray<USceneComponent*> SceneComponents;
    GetComponents<USceneComponent>(SceneComponents);

    for (USceneComponent* Component : SceneComponents)
    {
        if (Component && Component->GetName() == TEXT("MuzzlePoint"))
        {
            EnemyMuzzlePoint = Component;
            break;
        }
    }

    if (EnemyMuzzlePoint)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("MUZZLE POINT FOUND: %s"),
            *EnemyMuzzlePoint->GetName()
        );
    }
    else
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("MUZZLE POINT NOT FOUND")
        );
    }

    if (EnemyMuzzlePoint && MuzzleFlashLight)
    {
        MuzzleFlashLight->AttachToComponent(
            EnemyMuzzlePoint,
            FAttachmentTransformRules::SnapToTargetNotIncludingScale
        );
    }
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

    AUrbanTacticalFPSGameMode* GameMode =
        GetWorld()->GetAuthGameMode<AUrbanTacticalFPSGameMode>();

    if (GameMode)
    {
        GameMode->EnemyDied();
    }

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

    if (!PlayerActor)
    {
        return;
    }

    FVector StartLocation =
        GetActorLocation() + FVector(0.f, 0.f, 60.f);

    FVector EndLocation =
        PlayerActor->GetActorLocation() + FVector(0.f, 0.f, 50.f);

    FHitResult HitResult;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    TArray<AActor*> Enemies;

    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),
        AEnemyBase::StaticClass(),
        Enemies
    );

    for (AActor* EnemyActor : Enemies)
    {
        QueryParams.AddIgnoredActor(EnemyActor);
    }

    const bool bHit =
        GetWorld()->LineTraceSingleByChannel(
            HitResult,
            StartLocation,
            EndLocation,
            ECC_Visibility,
            QueryParams
        );

    const bool bHasLineOfSight =
        !bHit || HitResult.GetActor() == PlayerActor;

    if (bHasLineOfSight)
    {
        UGameplayStatics::ApplyDamage(
            PlayerActor,
            AttackDamage,
            GetController(),
            this,
            UDamageType::StaticClass()
        );

        ShowMuzzleFlash();

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("ENEMY FIRING - LINE OF SIGHT")
        );
    }
    else
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT(
                "ENEMY CANNOT FIRE - OBSTRUCTED | Hit: %s"
            ),
            HitResult.GetActor()
            ? *HitResult.GetActor()->GetName()
            : TEXT("Unknown")
        );
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
    //UE_LOG(LogTemp, Warning, TEXT("ENEMY STOP ATTACKING"));

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

void AEnemyBase::ShowMuzzleFlash()
{
    if (MuzzleFlashLight)
    {
        MuzzleFlashLight->SetIntensity(5000.0f);
    }

    if (MuzzleFlashEffect && EnemyMuzzlePoint)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            MuzzleFlashEffect,
            EnemyMuzzlePoint->GetComponentLocation(),
            EnemyMuzzlePoint->GetComponentRotation()
        );
    }

    GetWorld()->GetTimerManager().SetTimer(
        MuzzleFlashTimer,
        this,
        &AEnemyBase::HideMuzzleFlash,
        0.05f,
        false
    );
}

void AEnemyBase::HideMuzzleFlash()
{
    if (MuzzleFlashLight)
    {
        MuzzleFlashLight->SetIntensity(0.0f);
    }
}