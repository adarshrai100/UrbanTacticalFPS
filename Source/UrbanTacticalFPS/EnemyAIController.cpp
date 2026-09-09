#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"

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

    // --------------------------------------------------
    // NAVIGATION DIAGNOSTIC
    // --------------------------------------------------

    UNavigationSystemV1* NavSystem =
        UNavigationSystemV1::GetCurrent(GetWorld());

    if (NavSystem)
    {
        FNavLocation EnemyNavLocation;
        FNavLocation PlayerNavLocation;

        FVector EnemyFeetLocation = Enemy->GetActorLocation();
        EnemyFeetLocation.Z -= Enemy->GetSimpleCollisionHalfHeight();

        FVector PlayerFeetLocation = PlayerCharacter->GetActorLocation();
        PlayerFeetLocation.Z -= PlayerCharacter->GetSimpleCollisionHalfHeight();

        const bool bEnemyOnNavMesh =
            NavSystem->ProjectPointToNavigation(
                EnemyFeetLocation,
                EnemyNavLocation,
                FVector(100.f, 100.f, 300.f)
            );

        const bool bPlayerOnNavMesh =
            NavSystem->ProjectPointToNavigation(
                PlayerFeetLocation,
                PlayerNavLocation,
                FVector(100.f, 100.f, 300.f)
            );

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("NAV CHECK | Enemy: %s | Player: %s"),
            bEnemyOnNavMesh ? TEXT("YES") : TEXT("NO"),
            bPlayerOnNavMesh ? TEXT("YES") : TEXT("NO")
        );
    }
    else
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("NAV CHECK | Navigation System NOT FOUND")
        );
    }

    // --------------------------------------------------
    // DISTANCE DETECTION
    // --------------------------------------------------

    const float Distance =
        FVector::Distance(
            Enemy->GetActorLocation(),
            PlayerCharacter->GetActorLocation()
        );

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("ENEMY AI | Distance: %.0f | Detection: %.0f"),
        Distance,
        Enemy->DetectionRange
    );

    // --------------------------------------------------
    // OUTSIDE DETECTION RANGE
    // --------------------------------------------------

    if (Distance > Enemy->DetectionRange)
    {
        StopMovement();

        ClearFocus(EAIFocusPriority::Gameplay);

        Enemy->StopAttacking();

        return;
    }

    // --------------------------------------------------
    // PLAYER DETECTED — CHASE
    // --------------------------------------------------

    if (Distance > Enemy->AttackRange)
    {
        SetFocus(PlayerCharacter);

        EPathFollowingRequestResult::Type MoveResult =
            MoveToActor(
                PlayerCharacter,
                150.f
            );

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("MOVE RESULT: %d"),
            static_cast<int32>(MoveResult)
        );

        Enemy->StopAttacking();
    }
    else
    {
        // --------------------------------------------------
        // PLAYER IN ATTACK RANGE
        // --------------------------------------------------

        StopMovement();

        SetFocus(PlayerCharacter);

        Enemy->StartAttacking();
    }
}