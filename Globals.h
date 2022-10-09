#pragma once

#include "Utils.h"
#include "Base.h"

template <typename T_Fast>
static T_Fast* FindObjectFast(std::string name)
{
    auto Name = std::wstring(name.begin(), name.end()).c_str();

    static auto StaticFindObjectAddr = Util::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 54 41 56 41 57 48 8B EC 48 83 EC 60 80 3D ? ? ? ? ? 45 0F B6 F1");
    auto StaticFindObject = (T_Fast * (*)(UClass*, UObject * Package, const wchar_t* OrigInName, bool ExactClass))(StaticFindObjectAddr);
    return StaticFindObject(nullptr, nullptr, Name, false);
}

inline UFortEngine* GetEngine()
{
    static auto engine = UObject::FindObject<UFortEngine>("FortEngine_");
    return engine;
}

inline UWorld* GetWorld()
{
    return GetEngine()->GameViewport->World;
}

inline UKismetMathLibrary* GetMath()
{
    return (UKismetMathLibrary*)UKismetMathLibrary::StaticClass();
}

inline UGameplayStatics* GetGameplayStatics()
{
    return reinterpret_cast<UGameplayStatics*>(UGameplayStatics::StaticClass());
}

namespace Drivers
{
    static inline bool (*InitHost)(AOnlineBeaconHost*);
    static inline bool (*InitListen)(UIpNetDriver*, void*, FURL, bool, FString&);
    static inline void (*SetWorld)(UNetDriver*, UWorld*);
    static inline void (*ServerReplicateActors)(UReplicationGraph*);
    static inline void (*TickFlush)(UNetDriver*);
}
constexpr auto PI = 3.1415926535897932f;

static inline void sinCos(float* ScalarSin, float* ScalarCos, float Value)
{
	float quotient = (0.31830988618f * 0.5f) * Value;
	if (Value >= 0.0f)
	{
		quotient = (float)((int)(quotient + 0.5f));
	}
	else
	{
		quotient = (float)((int)(quotient - 0.5f));
	}
	float y = Value - (2.0f * PI) * quotient;

	float sign;
	if (y > 1.57079632679f)
	{
		y = PI - y;
		sign = -1.0f;
	}
	else if (y < -1.57079632679f)
	{
		y = -PI - y;
		sign = -1.0f;
	}
	else
	{
		sign = +1.0f;
	}

	float y2 = y * y;

	*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

	float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
	*ScalarCos = sign * p;
}
static auto RotToQuat(FRotator Rotator)
{
	const float DEG_TO_RAD = PI / (180.f);
	const float DIVIDE_BY_2 = DEG_TO_RAD / 2.f;
	float SP, SY, SR;
	float CP, CY, CR;

	sinCos(&SP, &CP, Rotator.Pitch * DIVIDE_BY_2);
	sinCos(&SY, &CY, Rotator.Yaw * DIVIDE_BY_2);
	sinCos(&SR, &CR, Rotator.Roll * DIVIDE_BY_2);

	FQuat RotationQuat;
	RotationQuat.X = CR * SP * SY - SR * CP * CY;
	RotationQuat.Y = -CR * SP * CY - SR * CP * SY;
	RotationQuat.Z = CR * CP * SY - SR * SP * CY;
	RotationQuat.W = CR * CP * CY + SR * SP * SY;

	return RotationQuat;
}
static AActor* SpawnActorTrans(UClass* StaticClass, FTransform SpawnTransform, AActor* Owner = nullptr, ESpawnActorCollisionHandlingMethod Flags = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
{
	AActor* FirstActor = GetGameplayStatics()->BeginDeferredActorSpawnFromClass(GetWorld(), StaticClass, SpawnTransform, Flags, Owner);

	if (FirstActor)
	{
		AActor* FinalActor = GetGameplayStatics()->FinishSpawningActor(FirstActor, SpawnTransform);

		if (FinalActor)
		{
			return FinalActor;
		}
	}

	return nullptr;
}
inline AActor* SpawnActorByClass(UClass* ActorClass, FVector Location = { 0.0f, 0.0f, 0.0f }, FRotator Rotation = { 0, 0, 0 }, AActor* Owner = nullptr, ESpawnActorCollisionHandlingMethod Flags = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
{
	FTransform SpawnTransform;

	SpawnTransform.Translation = Location;
	SpawnTransform.Scale3D = FVector{ 1, 1, 1 };
	SpawnTransform.Rotation = RotToQuat(Rotation);

	return SpawnActorTrans(ActorClass, SpawnTransform, Owner, Flags);
}

namespace Globals
{
	inline bool bHasBusStarted = false;

	inline UFortEngine* FortEngine;
	inline UWorld* World;
	inline UClass* PawnClass;

	std::vector<UFortWeaponItemDefinition*> CommonWeapons;
	std::vector<UFortWeaponItemDefinition*> UnCommonWeapons;
	std::vector<UFortWeaponItemDefinition*> RareWeapons;
	std::vector<UFortWeaponItemDefinition*> EpicWeapons;
	std::vector<UFortWeaponItemDefinition*> LegendaryWeapons;

	inline void Init()
	{
		auto BaseAddr = (uintptr_t)GetModuleHandle(0);

		PawnClass = UObject::FindClass("BlueprintGeneratedClass PlayerPawn_Athena.PlayerPawn_Athena_C");
		FortEngine = UObject::FindObject<UFortEngine>("FortEngine_");
		World = FortEngine->GameViewport->World;
	}
}

template <typename T = UObject>
inline T* NewObject(UObject* Outer, UClass* ObjectClass = T::StaticClass())
{
	return reinterpret_cast<T*>(UGameplayStatics::SpawnObject(ObjectClass, Outer));
}


namespace Spawners
{
	template <typename T>
	inline T* GetDefaultObject()
	{
		return T::StaticClass()->CreateDefaultObject<T>();
	}

	template <typename T>
	inline T* SpawnActor(FVector Loc = {}, FRotator Rot = {}, UClass* ActorClass = T::StaticClass())
	{
		FQuat Quat;
		FTransform Transform;

		auto DEG_TO_RAD = 3.14159 / 180;
		auto DIVIDE_BY_2 = DEG_TO_RAD / 2;

		auto SP = sin(Rot.Pitch * DIVIDE_BY_2);
		auto CP = cos(Rot.Pitch * DIVIDE_BY_2);

		auto SY = sin(Rot.Yaw * DIVIDE_BY_2);
		auto CY = cos(Rot.Yaw * DIVIDE_BY_2);

		auto SR = sin(Rot.Roll * DIVIDE_BY_2);
		auto CR = cos(Rot.Roll * DIVIDE_BY_2);

		Quat.X = CR * SP * SY - SR * CP * CY;
		Quat.Y = -CR * SP * CY - SR * CP * SY;
		Quat.Z = CR * CP * SY - SR * SP * CY;
		Quat.W = CR * CP * CY + SR * SP * SY;

		Transform.Rotation = Quat;
		Transform.Scale3D = FVector{ 1,1,1 };

		Transform.Translation = Loc;

		auto Actor = GetDefaultObject<UGameplayStatics>()->BeginSpawningActorFromClass(GetWorld(), ActorClass, Transform, false, nullptr);
		GetDefaultObject<UGameplayStatics>()->FinishSpawningActor(Actor, Transform);
		return (T*)Actor;
	}
	static void SpawnPickup32(UFortItemDefinition* ItemDef, int Count, FVector Location, FRotator Rotation = {}, bool bTossedFromContainer = false, int LoadedAmmo = 0)
	{
		if (ItemDef)
		{
			auto NewPickup = SpawnActor<AFortPickupAthena>(Location, Rotation);
			NewPickup->PrimaryPickupItemEntry.ItemDefinition = ItemDef;
			NewPickup->PrimaryPickupItemEntry.Count = Count;
			NewPickup->PrimaryPickupItemEntry.LoadedAmmo = LoadedAmmo;
			NewPickup->OnRep_PrimaryPickupItemEntry();

			NewPickup->TossPickup(Location, nullptr, 999, true);

			NewPickup->bTossedFromContainer = bTossedFromContainer;
			NewPickup->OnRep_TossedFromContainer();

			NewPickup->SetReplicateMovement(true);
			NewPickup->OnRep_ReplicateMovement();
			NewPickup->MovementComponent = (UProjectileMovementComponent*)GetDefaultObject<UGameplayStatics>()->SpawnObject(UProjectileMovementComponent::StaticClass(), NewPickup);
		}
	}
	inline AFortPickup* SummonPickup(UFortItemDefinition* ItemDef, int Count, FVector Location, EFortPickupSourceTypeFlag InPickupSourceTypeFlags = EFortPickupSourceTypeFlag::Tossed)
	{
		auto FortPickup = SpawnActor<AFortPickup>(Location);
		if (!FortPickup) return 0;

		FortPickup->bReplicates = true;

		FortPickup->PrimaryPickupItemEntry.Count = Count;
		FortPickup->PrimaryPickupItemEntry.ItemDefinition = ItemDef;

		FortPickup->OnRep_PrimaryPickupItemEntry();
		FortPickup->TossPickup(Location, 0, 6, true);
		//FortPickup->TossPickup(Location, Pawn, 6, true);

		return FortPickup;
	}

	static AFortPickup* SummonPickup2(AFortPlayerPawn* Pawn, auto ItemDef, int Count, FVector Location)
	{
		auto FortPickup = SpawnActor<AFortPickup>(Location);
		if (!FortPickup) return 0;

		FortPickup->bReplicates = true; // should be autmoatic but eh

		FortPickup->PrimaryPickupItemEntry.Count = Count;
		FortPickup->PrimaryPickupItemEntry.ItemDefinition = ItemDef;

		FortPickup->OnRep_PrimaryPickupItemEntry();
		FortPickup->TossPickup(Location, Pawn, 6, true);

		return FortPickup;

	}
}
namespace Spawners2
{
	inline AActor* SpawnActor(UClass* StaticClass, FVector Location = { 0.0f, 0.0f, 0.0f }, AActor* Owner = nullptr, FQuat Rotation = { 0, 0, 0 }, ESpawnActorCollisionHandlingMethod Flags = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
	{
		FTransform SpawnTransform;

		SpawnTransform.Translation = Location;
		SpawnTransform.Scale3D = FVector{ 1, 1, 1 };
		SpawnTransform.Rotation = Rotation;

		AActor* FirstActor = ((UGameplayStatics*)UGameplayStatics::StaticClass())->BeginDeferredActorSpawnFromClass(GetWorld(), StaticClass, SpawnTransform, Flags, Owner);

		if (FirstActor)
		{
			AActor* FinalActor = ((UGameplayStatics*)UGameplayStatics::StaticClass())->FinishSpawningActor(FirstActor, SpawnTransform);

			if (FinalActor)
				return FinalActor;
		}

		return nullptr;
	}
	template <typename ActorType>
	inline ActorType* SpawnActor(FVector Location = { 0.0f, 0.0f, 0.0f }, AActor* Owner = nullptr, FQuat Rotation = { 0, 0, 0 }, ESpawnActorCollisionHandlingMethod Flags = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn)
	{
		return (ActorType*)SpawnActor(ActorType::StaticClass(), Location, Owner, Rotation, Flags);
	}
}

