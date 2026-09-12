#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraSystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include "WeaponBase.generated.h"

class USoundBase;

UCLASS()
class URBANTACTICALFPS_API AWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AWeaponBase();

	void Fire();
	void StartFire();
	void StopFire();

	FTimerHandle FireTimer;

	float GetVerticalRecoil() const { return VerticalRecoil; }
	float GetHorizontalRecoil() const { return HorizontalRecoil; }
	float GetRecoilRecoverySpeed() const { return RecoilRecoverySpeed; }

	bool bIsFiring = false;

	void Reload();
	void FinishReload();

	int32 GetCurrentAmmo() const;
	int32 GetReserveAmmo() const;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// =========================
	// WEAPON
	// =========================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float FireRate = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float Range = 10000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	bool bIsAutomatic = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USceneComponent* WeaponRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USceneComponent* MuzzlePoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Shotgun")
	int32 PelletCount = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Shotgun")
	float SpreadAngle = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Shotgun")
	bool bIsShotgun = false;

	// =========================
	// EFFECTS
	// =========================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	UNiagaraSystem* MuzzleFlash;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Impact")
	UNiagaraSystem* BulletImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Impact")
	TObjectPtr<USoundBase> BulletImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Impact")
	float BulletImpactSoundVolume = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Impact")
	UNiagaraSystem* ConcreteImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Impact")
	UNiagaraSystem* MetalImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects|Impact")
	UNiagaraSystem* WoodImpactEffect;

	// =========================
	// RECOIL
	// =========================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Recoil")
	float VerticalRecoil = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Recoil")
	float HorizontalRecoil = 0.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Recoil")
	float RecoilRecoverySpeed = 10.0f;

	// =========================
	// AMMO
	// =========================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 MagazineSize = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 CurrentAmmo = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 ReserveAmmo = 120;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ammo")
	float ReloadTime = 2.2f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	bool bIsReloading = false;

	FTimerHandle ReloadTimer;

	// =========================
	// ADS
	// =========================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|ADS")
	bool bCanADS = true;

	// =========================
	// AUDIO
	// =========================

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> FireSound;
};