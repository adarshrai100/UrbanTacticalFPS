#include "UrbanTacticalFPSGameMode.h"
#include "PlayerOperator.h"
#include "EnemyBase.h"
#include "FPSHUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AUrbanTacticalFPSGameMode::AUrbanTacticalFPSGameMode()
{
    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
        TEXT("/Game/Blueprints/BP_PlayerOperator")
    );

    if (PlayerPawnBPClass.Class)
    {
        DefaultPawnClass = PlayerPawnBPClass.Class;
    }
}

void AUrbanTacticalFPSGameMode::BeginPlay()
{
    Super::BeginPlay();

    InitializeEnemyCount();
}

void AUrbanTacticalFPSGameMode::InitializeEnemyCount()
{
    TArray<AActor*> EnemyActors;

    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(),
        AEnemyBase::StaticClass(),
        EnemyActors
    );

    RemainingEnemies = 0;

    for (AActor* Actor : EnemyActors)
    {
        AEnemyBase* Enemy = Cast<AEnemyBase>(Actor);

        if (Enemy && !Enemy->bIsDead)
        {
            RemainingEnemies++;
        }
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("MISSION START - HOSTILES: %d"),
        RemainingEnemies
    );

    RefreshObjective();
}

void AUrbanTacticalFPSGameMode::EnemyDied()
{
    if (bMissionComplete)
    {
        return;
    }

    RemainingEnemies = FMath::Max(0, RemainingEnemies - 1);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("HOSTILE ELIMINATED - REMAINING: %d"),
        RemainingEnemies
    );

    RefreshObjective();

    if (RemainingEnemies <= 0)
    {
        bMissionComplete = true;

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("=== MISSION COMPLETE ===")
        );

        APlayerOperator* Player =
            Cast<APlayerOperator>(
                UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)
            );

        if (Player)
        {
            Player->SetMissionCompleteState();
        }
    }
}

int32 AUrbanTacticalFPSGameMode::GetRemainingEnemies() const
{
    return RemainingEnemies;
}

void AUrbanTacticalFPSGameMode::RefreshObjective()
{
    APlayerOperator* Player =
        Cast<APlayerOperator>(
            UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)
        );

    if (!Player)
    {
        return;
    }

    UFPSHUDWidget* HUD = Player->GetHUDWidget();

    if (!HUD)
    {
        return;
    }

    if (RemainingEnemies > 0)
    {
        HUD->SetObjectiveText(
            FText::FromString(
                FString::Printf(
                    TEXT("ELIMINATE ALL HOSTILES\nHOSTILES REMAINING: %d"),
                    RemainingEnemies
                )
            )
        );
    }
    else
    {
        HUD->ShowMissionComplete();
    }
}