// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerOperator.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
    PrimaryActorTick.bCanEverTick = true;

    WeaponRoot = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));
    RootComponent = WeaponRoot;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    WeaponMesh->SetupAttachment(WeaponRoot);

    MuzzlePoint = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzlePoint"));
    MuzzlePoint->SetupAttachment(WeaponMesh);

    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentAmmo = MagazineSize;
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AWeaponBase::Fire()
{
    if (bIsReloading)
    {
        return;
    }

    APawn* OwnerPawn = Cast<APawn>(GetOwner());

    if (!OwnerPawn)
    {
        return;
    }

    AController* Controller = OwnerPawn->GetController();

    if (!Controller)
    {
        return;
    }
    if (CurrentAmmo <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Out of Ammo"));
        return;
    }

    FVector CameraLocation;
    FRotator CameraRotation;

    Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

    FVector MuzzleLocation = MuzzlePoint->GetComponentLocation();

    if (MuzzleFlash)
    {
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            MuzzleFlash,
            MuzzlePoint,
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset,
            true
        );
    }

    // First trace from camera to determine aim point
    FVector CameraStart = CameraLocation;
    FVector CameraEnd = CameraStart + (CameraRotation.Vector() * Range);

    FHitResult CameraHit;

    bool bCameraHit = GetWorld()->LineTraceSingleByChannel(
        CameraHit,
        CameraStart,
        CameraEnd,
        ECC_Visibility
    );

    FVector TargetPoint;

    if (bCameraHit)
    {
        TargetPoint = CameraHit.Location;
    }
    else
    {
        TargetPoint = CameraEnd;
    }

    // Fire from muzzle toward target point
    FVector ShotDirection = (TargetPoint - MuzzleLocation).GetSafeNormal();

    FVector TraceStart = MuzzleLocation;
    FVector TraceEnd = TraceStart + (ShotDirection * Range);

    FHitResult Hit;

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        TraceStart,
        TraceEnd,
        ECC_Visibility
    );
    UE_LOG(LogTemp, Warning, TEXT("Trace Hit: %s"),
        bHit ? TEXT("TRUE") : TEXT("FALSE"));

    if (bHit)
    {
        AActor* HitActor = Hit.GetActor();

        if (HitActor)
        {
            UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());

            UGameplayStatics::ApplyDamage(
                HitActor,
                Damage,
                Controller,
                this,
                UDamageType::StaticClass()
            );
        }

        DrawDebugSphere(
            GetWorld(),
            Hit.Location,
            12.f,
            12,
            FColor::Red,
            false,
            2.f
        );
    }

    DrawDebugLine(
        GetWorld(),
        TraceStart,
        TraceEnd,
        FColor::Green,
        false,
        10.f,
        0,
        3.f
    );

    APlayerOperator* Player = Cast<APlayerOperator>(GetOwner());

    if (Player)
    {
        Player->AddRecoil();
    }

    CurrentAmmo--;
    if (Player)
    {
        Player->UpdateAmmoUI();
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Ammo: %d / %d"),
        CurrentAmmo,
        ReserveAmmo
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

void AWeaponBase::Reload()
{
    if (bIsReloading)
    {
        return;
    }

    if (CurrentAmmo == MagazineSize)
    {
        UE_LOG(LogTemp, Warning, TEXT("Magazine already full"));
        return;
    }

    if (ReserveAmmo <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No reserve ammo"));
        return;
    }

    bIsReloading = true;
    StopFire();

    UE_LOG(LogTemp, Warning, TEXT("Reloading..."));

    GetWorldTimerManager().SetTimer(
        ReloadTimer,
        this,
        &AWeaponBase::FinishReload,
        ReloadTime,
        false
    );
}

void AWeaponBase::FinishReload()
{
    int32 AmmoNeeded = MagazineSize - CurrentAmmo;

    int32 AmmoToLoad = FMath::Min(
        AmmoNeeded,
        ReserveAmmo
    );

    CurrentAmmo += AmmoToLoad;
    ReserveAmmo -= AmmoToLoad;

    if (APlayerOperator* Player = Cast<APlayerOperator>(GetOwner()))
    {
        Player->UpdateAmmoUI();
    }

    bIsReloading = false;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Reload Complete: %d / %d"),
        CurrentAmmo,
        ReserveAmmo
    );
}

int32 AWeaponBase::GetCurrentAmmo() const
{
    return CurrentAmmo;
}

int32 AWeaponBase::GetReserveAmmo() const
{
    return ReserveAmmo;
}