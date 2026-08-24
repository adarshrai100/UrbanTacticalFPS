#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class URBANTACTICALFPS_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    float MoveAcceptanceDistance = 50.f;
};