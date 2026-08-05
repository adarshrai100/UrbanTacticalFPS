#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"
#include "GameFramework/Character.h"

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

    if (!PlayerCharacter || !Enemy)
    {
        return;
    }

    float Distance =
        FVector::Distance(
            Enemy->GetActorLocation(),
            PlayerCharacter->GetActorLocation()
        );


    MoveToActor(
        PlayerCharacter,
        Enemy->AttackRange
    );

    if (Distance <= Enemy->AttackStartRange)
    {
        Enemy->StartAttacking();
    }
    else
    {
        Enemy->StopAttacking();
    }
}