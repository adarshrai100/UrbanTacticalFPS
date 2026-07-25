// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "TargetDummy.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
    RootComponent = WeaponRoot;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(WeaponRoot);


    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::Fire()
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

    if (!PlayerController) return;

    FVector CameraLocation;
    FRotator CameraRotation;

    PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector Start = CameraLocation;
    FVector End = Start + (CameraRotation.Vector() * Range);

    FHitResult Hit;

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        Start,
        End,
        ECC_Visibility
    );

    if (bHit)
    {
        AActor* HitActor = Hit.GetActor();

        if (HitActor)
        {
            UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());

            // Apply damage
            if (HitActor->ActorHasTag("Target"))
            {
                // Cast to TargetDummy
                ATargetDummy* Target = Cast<ATargetDummy>(HitActor);
                if (Target)
                {
                    Target->TakeDamage(Damage);
                }
            }
        }

        DrawDebugSphere(GetWorld(), Hit.Location, 12.f, 12, FColor::Red, false, 2.f);
    }

    DrawDebugLine(
        GetWorld(),
        Start,
        End,
        FColor::Green,
        false,
        1.f,
        0,
        1.f
    );
}

void AWeaponBase::StartFire()
{
    if (!bIsFiring)
    {
        bIsFiring = true;

        float FireDelay = 60.f / FireRate;

        GetWorldTimerManager().SetTimer(
            FireTimer,
            this,
            &AWeaponBase::Fire,
            FireDelay,
            true
        );
    }
}

void AWeaponBase::StopFire()
{
    bIsFiring = false;
    GetWorldTimerManager().ClearTimer(FireTimer);
}

