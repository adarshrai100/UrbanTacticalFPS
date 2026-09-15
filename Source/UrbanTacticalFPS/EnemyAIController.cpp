#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

void AEnemyAIController::BeginPlay()
{
    Super::BeginPlay();

    PrimaryActorTick.bCanEverTick = true;
}

void AEnemyAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    ACharacter* PlayerCharacter =
        UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    AEnemyBase* Enemy =
        Cast<AEnemyBase>(GetPawn());

    if (!PlayerCharacter || !Enemy || Enemy->bIsDead)
    {
        return;
    }

    const float Distance =
        FVector::Distance(
            Enemy->GetActorLocation(),
            PlayerCharacter->GetActorLocation()
        );

    // Player is outside the enemy's detection range.
    if (Distance > Enemy->DetectionRange)
    {
        StopMovement();
        ClearFocus(EAIFocusPriority::Gameplay);
        Enemy->StopAttacking();

        return;
    }

    UNavigationSystemV1* NavSystem =
        UNavigationSystemV1::GetCurrent(GetWorld());

    if (!NavSystem)
    {
        return;
    }

    const FVector PlayerLocation =
        PlayerCharacter->GetActorLocation();

    FNavLocation PlayerNavLocation;

    const bool bPlayerOnNavMesh =
        NavSystem->ProjectPointToNavigation(
            PlayerLocation,
            PlayerNavLocation,
            FVector(500.f, 500.f, 1000.f)
        );

    if (!bPlayerOnNavMesh)
    {
        Enemy->StopAttacking();

        return;
    }

    // Move toward the player while outside attack range.
    if (Distance > Enemy->AttackRange)
    {
        SetFocus(PlayerCharacter);

        // Only request a new move when there isn't already
        // an active path-following request.
        if (!GetPathFollowingComponent()->HasValidPath())
        {
            MoveToLocation(
                PlayerNavLocation.Location,
                MoveAcceptanceDistance,
                true,
                true,
                true,
                true,
                nullptr,
                true
            );
        }

        Enemy->StopAttacking();
    }
    else
    {
        // Inside attack range.
        StopMovement();

        SetFocus(PlayerCharacter);

        Enemy->StartAttacking();
    }
}