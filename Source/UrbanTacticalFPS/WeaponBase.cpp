#include "WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerOperator.h"
#include "Components/AudioComponent.h"

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

void AWeaponBase::BeginPlay()
{
    Super::BeginPlay();

    CurrentAmmo = MagazineSize;
}

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

    // =========================
    // MUZZLE FLASH
    // =========================

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

    // =========================
    // FIRE SOUND
    // =========================

    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            FireSound,
            MuzzleLocation,
            0.35f
        );
    }

    // =========================
    // CAMERA AIM
    // =========================

    FVector CameraStart = CameraLocation;

    FVector CameraEnd =
        CameraStart +
        (CameraRotation.Vector() * Range);

    FHitResult CameraHit;

    bool bCameraHit =
        GetWorld()->LineTraceSingleByChannel(
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

    // =========================
    // SHOTGUN
    // =========================

    if (bIsShotgun)
    {
        FVector BaseDirection =
            (TargetPoint - MuzzleLocation).GetSafeNormal();

        for (int32 PelletIndex = 0;
            PelletIndex < PelletCount;
            PelletIndex++)
        {
            FRotator PelletRotation =
                BaseDirection.Rotation();

            PelletRotation.Yaw +=
                FMath::FRandRange(
                    -SpreadAngle,
                    SpreadAngle
                );

            PelletRotation.Pitch +=
                FMath::FRandRange(
                    -SpreadAngle,
                    SpreadAngle
                );

            FVector PelletDirection =
                PelletRotation.Vector();

            FVector TraceStart =
                MuzzleLocation;

            FVector TraceEnd =
                TraceStart +
                (PelletDirection * Range);

            FHitResult Hit;

            FCollisionQueryParams QueryParams;

            QueryParams.bReturnPhysicalMaterial = true;

            bool bHit =
                GetWorld()->LineTraceSingleByChannel(
                    Hit,
                    TraceStart,
                    TraceEnd,
                    ECC_Visibility,
                    QueryParams
                );

            if (bHit)
            {
                AActor* HitActor =
                    Hit.GetActor();

                if (HitActor)
                {
                    UGameplayStatics::ApplyDamage(
                        HitActor,
                        Damage,
                        Controller,
                        this,
                        UDamageType::StaticClass()
                    );
                }

                UNiagaraSystem* SelectedImpactEffect =
                    BulletImpactEffect;

                if (Hit.PhysMaterial.IsValid())
                {
                    EPhysicalSurface SurfaceType =
                        UPhysicalMaterial::DetermineSurfaceType(
                            Hit.PhysMaterial.Get()
                        );

                    switch (SurfaceType)
                    {
                    case SurfaceType1:
                        SelectedImpactEffect =
                            ConcreteImpactEffect;
                        break;

                    case SurfaceType2:
                        SelectedImpactEffect =
                            MetalImpactEffect;
                        break;

                    case SurfaceType3:
                        SelectedImpactEffect =
                            WoodImpactEffect;
                        break;

                    default:
                        break;
                    }
                }

                if (SelectedImpactEffect)
                {
                    FRotator ImpactRotation =
                        Hit.ImpactNormal.Rotation();

                    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                        GetWorld(),
                        SelectedImpactEffect,
                        Hit.ImpactPoint,
                        ImpactRotation
                    );
                }

                if (BulletImpactSound)
                {
                    UGameplayStatics::PlaySoundAtLocation(
                        this,
                        BulletImpactSound,
                        Hit.ImpactPoint,
                        BulletImpactSoundVolume
                    );
                }
            }
        }
    }
    else
    {
        // =========================
        // RIFLE / PISTOL
        // =========================

        FVector ShotDirection =
            (TargetPoint - MuzzleLocation).GetSafeNormal();

        FVector TraceStart =
            MuzzleLocation;

        FVector TraceEnd =
            TraceStart +
            (ShotDirection * Range);

        FHitResult Hit;

        FCollisionQueryParams QueryParams;

        QueryParams.bReturnPhysicalMaterial = true;

        bool bHit =
            GetWorld()->LineTraceSingleByChannel(
                Hit,
                TraceStart,
                TraceEnd,
                ECC_Visibility,
                QueryParams
            );

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Trace Hit: %s"),
            bHit ? TEXT("TRUE") : TEXT("FALSE")
        );

        if (bHit)
        {
            AActor* HitActor =
                Hit.GetActor();

            if (HitActor)
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT("Hit Actor: %s"),
                    *HitActor->GetName()
                );

                UGameplayStatics::ApplyDamage(
                    HitActor,
                    Damage,
                    Controller,
                    this,
                    UDamageType::StaticClass()
                );
            }

            UNiagaraSystem* SelectedImpactEffect =
                BulletImpactEffect;

            if (Hit.PhysMaterial.IsValid())
            {
                EPhysicalSurface SurfaceType =
                    UPhysicalMaterial::DetermineSurfaceType(
                        Hit.PhysMaterial.Get()
                    );

                switch (SurfaceType)
                {
                case SurfaceType1:
                    SelectedImpactEffect =
                        ConcreteImpactEffect;
                    break;

                case SurfaceType2:
                    SelectedImpactEffect =
                        MetalImpactEffect;
                    break;

                case SurfaceType3:
                    SelectedImpactEffect =
                        WoodImpactEffect;
                    break;

                default:
                    break;
                }
            }

            if (SelectedImpactEffect)
            {
                FRotator ImpactRotation =
                    Hit.ImpactNormal.Rotation();

                UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                    GetWorld(),
                    SelectedImpactEffect,
                    Hit.ImpactPoint,
                    ImpactRotation
                );
            }

            if (BulletImpactSound)
            {
                UGameplayStatics::PlaySoundAtLocation(
                    this,
                    BulletImpactSound,
                    Hit.ImpactPoint,
                    BulletImpactSoundVolume
                );
            }
        }
    }

    // =========================
    // RECOIL + FIRE WEAPON KICK
    // =========================

    APlayerOperator* Player =
        Cast<APlayerOperator>(GetOwner());

    if (Player)
    {
        Player->AddRecoil();
        Player->AddFireWeaponKick();
    }

    // =========================
    // ONE AMMO PER SHOT
    // =========================

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
    if (bIsFiring)
    {
        return;
    }

    if (bIsReloading)
    {
        return;
    }

    bIsFiring = true;

    // =========================
    // SEMI-AUTOMATIC
    // =========================

    if (!bIsAutomatic)
    {
        Fire();
        return;
    }

    // =========================
    // AUTOMATIC
    // =========================

    float FireDelay =
        60.f / FireRate;

    GetWorldTimerManager().SetTimer(
        FireTimer,
        this,
        &AWeaponBase::Fire,
        FireDelay,
        true
    );
}

void AWeaponBase::StopFire()
{
    bIsFiring = false;

    GetWorldTimerManager().ClearTimer(
        FireTimer
    );
}

void AWeaponBase::Reload()
{
    if (bIsReloading)
    {
        return;
    }

    if (CurrentAmmo == MagazineSize)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Magazine already full")
        );

        return;
    }

    if (ReserveAmmo <= 0)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("No reserve ammo")
        );

        return;
    }

    bIsReloading = true;

    StopFire();

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Reloading...")
    );

    // =========================
    // RELOAD SOUND
    // =========================

    if (ReloadAudioComponent)
    {
        ReloadAudioComponent->Stop();
        ReloadAudioComponent = nullptr;
    }

    if (ReloadSound)
    {
        ReloadAudioComponent =
            UGameplayStatics::SpawnSoundAttached(
                ReloadSound,
                RootComponent,
                NAME_None,
                FVector::ZeroVector,
                EAttachLocation::KeepRelativeOffset,
                false,
                0.5f
            );
    }

    // =========================
    // SHELL-BY-SHELL RELOAD
    // =========================

    if (bShellByShellReload)
    {
        GetWorldTimerManager().SetTimer(
            ReloadTimer,
            this,
            &AWeaponBase::FinishReload,
            ShellInsertTime,
            true
        );

        return;
    }

    // =========================
    // MAGAZINE RELOAD
    // =========================

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
    // =========================
    // SHELL-BY-SHELL RELOAD
    // =========================

    if (bShellByShellReload)
    {
        if (CurrentAmmo >= MagazineSize ||
            ReserveAmmo <= 0)
        {
            GetWorldTimerManager().ClearTimer(
                ReloadTimer
            );

            bIsReloading = false;

            // Stop reload sound only when
            // the entire shotgun reload is complete.
            if (ReloadAudioComponent)
            {
                ReloadAudioComponent->Stop();
                ReloadAudioComponent = nullptr;
            }

            if (APlayerOperator* Player =
                Cast<APlayerOperator>(GetOwner()))
            {
                Player->UpdateAmmoUI();
            }

            UE_LOG(
                LogTemp,
                Warning,
                TEXT("Shotgun Reload Complete: %d / %d"),
                CurrentAmmo,
                ReserveAmmo
            );

            return;
        }

        CurrentAmmo++;
        ReserveAmmo--;

        if (APlayerOperator* Player =
            Cast<APlayerOperator>(GetOwner()))
        {
            Player->UpdateAmmoUI();
        }

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Shell Loaded: %d / %d"),
            CurrentAmmo,
            ReserveAmmo
        );

        if (CurrentAmmo >= MagazineSize ||
            ReserveAmmo <= 0)
        {
            GetWorldTimerManager().ClearTimer(
                ReloadTimer
            );

            bIsReloading = false;

            // Stop reload sound when the
            // final shell has been inserted.
            if (ReloadAudioComponent)
            {
                ReloadAudioComponent->Stop();
                ReloadAudioComponent = nullptr;
            }

            UE_LOG(
                LogTemp,
                Warning,
                TEXT("Shotgun Reload Complete: %d / %d"),
                CurrentAmmo,
                ReserveAmmo
            );
        }

        return;
    }

    // =========================
    // NORMAL MAGAZINE RELOAD
    // =========================

    // Normal reload reaches FinishReload()
    // only once, so stop the sound here.
    if (ReloadAudioComponent)
    {
        ReloadAudioComponent->Stop();
        ReloadAudioComponent = nullptr;
    }

    int32 AmmoNeeded =
        MagazineSize - CurrentAmmo;

    int32 AmmoToLoad =
        FMath::Min(
            AmmoNeeded,
            ReserveAmmo
        );

    CurrentAmmo += AmmoToLoad;
    ReserveAmmo -= AmmoToLoad;

    if (APlayerOperator* Player =
        Cast<APlayerOperator>(GetOwner()))
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