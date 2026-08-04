#include "EnemyAIController.h"
#include "Kismet/GameplayStatics.h"
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

    if (PlayerCharacter)
    {
        MoveToActor(
            PlayerCharacter,
            150.f
        );
    }

    if (PlayerCharacter)
    {
        APawn* ControlledPawn = GetPawn();

        if (ControlledPawn)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("Speed: %f"),
                ControlledPawn->GetVelocity().Size());
        }

        MoveToActor(PlayerCharacter, 150.f);
    }
}