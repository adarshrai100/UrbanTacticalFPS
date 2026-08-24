#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"
#include "GameFramework/Character.h"
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

    if (Distance > Enemy->AttackRange)
    {
        ClearFocus(EAIFocusPriority::Gameplay);

        const EPathFollowingRequestResult::Type MoveResult =
            MoveToActor(
                PlayerCharacter,
                50.f
            );

        if (MoveResult == EPathFollowingRequestResult::Failed)
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("ENEMY MOVE FAILED | Distance: %.1f"),
                Distance
            );
        }

        Enemy->StopAttacking();
    }
    else
    {
        StopMovement();

        SetFocus(PlayerCharacter);

        Enemy->StartAttacking();
    }
}