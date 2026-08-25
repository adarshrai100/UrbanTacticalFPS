#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UrbanTacticalFPSGameMode.generated.h"

UCLASS(minimalapi)
class AUrbanTacticalFPSGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AUrbanTacticalFPSGameMode();

    virtual void BeginPlay() override;

    void EnemyDied();
    void RefreshObjective();

    int32 GetRemainingEnemies() const;

private:

    int32 RemainingEnemies = 0;

    void InitializeEnemyCount();

    bool bMissionComplete = false;
};