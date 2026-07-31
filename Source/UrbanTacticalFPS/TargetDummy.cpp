// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetDummy.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATargetDummy::ATargetDummy()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void ATargetDummy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATargetDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ATargetDummy::TakeDamage(
    float DamageAmount,
    const FDamageEvent& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    Health -= DamageAmount;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Target Hit! Health: %.0f"),
        Health
    );

    if (Health <= 0.f)
    {
        UE_LOG(LogTemp, Warning, TEXT("TARGET DEAD"));

        Destroy();
    }

    return DamageAmount;
}

