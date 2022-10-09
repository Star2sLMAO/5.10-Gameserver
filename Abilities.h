#pragma once

#include "Utils.h"
#include "SDK/SDK.hpp"
#include "Globals.h"
#include "UE4.h"
#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

template<class TArrayType>
class TArray_
{

public:
	FORCEINLINE int Num() const
	{
		return Count;
	}
	FORCEINLINE int Max() const
	{
		return MaxElements;
	}
	FORCEINLINE int Slack() const
	{
		return MaxElements - Count;
	}
	FORCEINLINE void Reserve(const int NumElements)
	{
		Data = Slack() >= NumElements ? Data : (TArrayType*)FMemory_Realloc(Data, (MaxElements = Count + NumElements) * sizeof(TArrayType), 0);
	}
	FORCEINLINE void Reset(int MinSizeAfterReset = 0)
	{
		MaxElements = MinSizeAfterReset;
		Count = 0;

		FreeMemory(Data);
		Data = MallocMemory(MinSizeAfterReset * sizeof(TArrayType), alignof(TArrayType));
	}
	FORCEINLINE bool RemoveSingle(const int Index)
	{
		if (Index < Count)
		{
			if (Index != Count - 1)
				Data[Index] = Data[Count - 1];

			--Count;

			return true;
		}
		return false;
	}
	FORCEINLINE void RemoveAt(int Index, int Lenght = 1)
	{
		for (; Lenght != 0; --Lenght)
		{
			if (!RemoveSingle(Index++))
				break;
		}
	}
	FORCEINLINE void AddSingle(TArrayType InData)
	{
		Reserve(1);
		Data[Count] = InData;
		++Count;
	}
	FORCEINLINE void Add(int n, TArrayType InData...)
	{
		va_list MyList;
		va_start(MyList, n);

		for (int i = 0; i < n; i++)
		{
			AddSingle(va_arg(MyList, TArrayType));
		}
		va_end(MyList);
	};
	FORCEINLINE void FreeArray()
	{
		FreeMemory(Data);
		Data = nullptr;
		Count = 0;
		MaxElements = 0;
	}

	FORCEINLINE explicit operator bool()
	{
		return MaxElements > 0 && Count > 0 && Data;
	}
	FORCEINLINE TArrayType& operator[](int i)
	{
		return Data[i];
	};

	FORCEINLINE const TArrayType& operator[](int i) const
	{
		return Data[i];
	};

	FORCEINLINE int MaxIndex()
	{
		return Count - 1;
	}

	class FBaseArrayIterator
	{
		int32_t Index;
		TArray<TArrayType>& ItArray;

	public:
		FBaseArrayIterator(TArray<TArrayType>& Array)
			: ItArray(Array), Index(Array.Count)
		{
		}
		FBaseArrayIterator(TArray<TArrayType>& Array, int32_t CurrentIndex)
			: ItArray(Array), Index(CurrentIndex)
		{
		}

		FORCEINLINE TArrayType& operator*()
		{
			return ItArray[Index];
		}
		FORCEINLINE TArrayType& operator->()
		{
			return ItArray[Index];
		}
		FORCEINLINE FBaseArrayIterator& operator++()
		{
			++Index;
			return *this;
		}
		FORCEINLINE bool operator==(const FBaseArrayIterator& other) const
		{
			return Index == other.Index;
		}
		FORCEINLINE bool operator!=(const FBaseArrayIterator& other) const
		{
			return Index != other.Index;
		}
	};

	FORCEINLINE FBaseArrayIterator begin()
	{
		return FBaseArrayIterator(*this, 0);
	}
	FORCEINLINE FBaseArrayIterator begin() const
	{
		return FBaseArrayIterator(*this, 0);
	}
	FORCEINLINE FBaseArrayIterator end()
	{
		return FBaseArrayIterator(*this);
	}
	FORCEINLINE FBaseArrayIterator end() const
	{
		return FBaseArrayIterator(*this);
	}

private:
	TArrayType* Data;
	int32_t Count;
	int32_t MaxElements;
};
namespace Abilities
{
	bool (*InternalTryActivateAbility)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle, FPredictionKey, UGameplayAbility**, void*, FGameplayEventData*);
	FGameplayAbilitySpecHandle* (*GiveAbility_)(UAbilitySystemComponent*, FGameplayAbilitySpecHandle*, FGameplayAbilitySpec);

	static void Init()
	{
		auto BaseAddr = (uintptr_t)GetModuleHandle(0);
		InternalTryActivateAbility = decltype(InternalTryActivateAbility)(Util::FindPattern("4C 89 4C 24 ? 4C 89 44 24 ? 89 54 24 10 55 53 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 45 33 FF"));
		GiveAbility_ = decltype(GiveAbility_)(Util::FindPattern("48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 56 48 83 EC 20 83 B9 ? ? ? ? ? 49 8B E8 4C 8B F2 48 8B F9 7E 56"));
	}
	static void GiveAbility(AFortPlayerPawn* InPawn, UClass* AbilityClass)
	{
		auto AbilitySystemComponent = InPawn->AbilitySystemComponent;

		if (!AbilitySystemComponent)
			return;

		auto GenerateNewSpec = [&]() -> FGameplayAbilitySpec
		{
			FGameplayAbilitySpecHandle Handle{ rand() };

			FGameplayAbilitySpec Spec{ -1, -1, -1, Handle, AbilityClass->CreateDefaultObject<UGameplayAbility>(), 1, -1, nullptr, 0, false, false, false };

			return Spec;
		};

		auto Spec = GenerateNewSpec();

		for (int i = 0; i < AbilitySystemComponent->ActivatableAbilities.Items.Num(); i++)
		{
			auto& CurrentSpec = AbilitySystemComponent->ActivatableAbilities.Items[i];

			if (CurrentSpec.Ability == Spec.Ability)
				return;
		}

		auto Handle = GiveAbility_(AbilitySystemComponent, &Spec.Handle, Spec);
	}
	static void Update(AFortPlayerController* Controller, int Dirty = 0, bool bRemovedItem = false)
	{
		Controller->WorldInventory->bRequiresLocalUpdate = true;
		Controller->WorldInventory->HandleInventoryLocalUpdate();
		Controller->WorldInventory->ForceNetUpdate();
		Controller->HandleWorldInventoryLocalUpdate();

		Controller->QuickBars->OnRep_PrimaryQuickBar();
		Controller->QuickBars->OnRep_SecondaryQuickBar();
		Controller->QuickBars->ForceNetUpdate();
		Controller->ForceUpdateQuickbar(EFortQuickBars::Primary);
		Controller->ForceUpdateQuickbar(EFortQuickBars::Secondary);

		if (bRemovedItem)
			Controller->WorldInventory->Inventory.MarkArrayDirty();

		if (Dirty != 0 && Controller->WorldInventory->Inventory.ReplicatedEntries.Num() >= Dirty)
			Controller->WorldInventory->Inventory.MarkItemDirty(Controller->WorldInventory->Inventory.ReplicatedEntries[Dirty]);
	}


	static void GiveAbilities(AFortPlayerPawn* InPawn)
	{
		static auto AbilitySet = FindObjectFast<UFortAbilitySet>("/Game/Abilities/Player/Generic/Traits/DefaultPlayer/GAS_DefaultPlayer.GAS_DefaultPlayer");

		for (int i = 0; i < AbilitySet->GameplayAbilities.Num(); i++)
		{
			auto Ability = AbilitySet->GameplayAbilities[i];

			GiveAbility(InPawn, Ability);
		}

		static std::vector<UClass*> CustomAbilitySet
		{
			FindObjectFast<UClass>("/Game/Abilities/Weapons/Ranged/GA_Ranged_GenericDamage.GA_Ranged_GenericDamage_C"),
		};

		for (auto Ability : CustomAbilitySet)
		{
			GiveAbility(InPawn, Ability);
		}
	}

	static UFortWorldItem* CreateItem(UFortItemDefinition* ItemDef, int Count = 1)
	{
		return (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(Count, 1);
	}
	static UFortItem* CreateItem2(AFortPlayerController* TargetController, UFortItemDefinition* ItemDefinition, int count)
	{
		auto TempItemInstance = ItemDefinition->CreateTemporaryItemInstanceBP(count, 1);

		if (TempItemInstance)
		{
			TempItemInstance->SetOwningControllerForTemporaryItem(TargetController);
		}

		return TempItemInstance;
	}

	inline UFortWorldItem* GetInstanceFromGuid(AController* PC, const FGuid& ToFindGuid)
	{
		auto& ItemInstances = ((AFortPlayerController*)PC)->WorldInventory->Inventory.ItemInstances;
		for (int j = 0; j < ItemInstances.Num(); j++)
		{
			auto ItemInstance = ItemInstances[j];

			if (!ItemInstance)
				continue;

			auto Guid = ItemInstance->ItemEntry.ItemGuid;

			if (ToFindGuid == Guid)
			{
				return ItemInstance;
			}
		}

		return nullptr;
	}

	inline auto& GetItemInstances(AFortPlayerController* PC)
	{
		return PC->WorldInventory->Inventory.ItemInstances;
	}
	static void UpdateInventory(AFortPlayerController* InPC)
	{
		InPC->WorldInventory->Inventory.MarkArrayDirty();
		InPC->HandleWorldInventoryLocalUpdate();
		InPC->WorldInventory->HandleInventoryLocalUpdate();
		InPC->WorldInventory->bRequiresLocalUpdate = true;
		InPC->QuickBars->OnRep_PrimaryQuickBar();
		InPC->QuickBars->OnRep_SecondaryQuickBar();
		InPC->WorldInventory->ForceNetUpdate();
		InPC->QuickBars->ForceNetUpdate();
	}
	UFortWorldItem* AddItem43(AFortPlayerController* PlayerController, UFortWorldItem* Item)
	{
		PlayerController->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
		PlayerController->WorldInventory->Inventory.ItemInstances.Add(Item);
		return Item;
	}

	UFortWorldItem* AddItem54(AFortPlayerController* PlayerController, UFortItemDefinition* ItemDef, int Count)
	{
		if (ItemDef == nullptr || PlayerController == nullptr)
			return nullptr;

		auto Inventory = PlayerController->WorldInventory->Inventory;

		UFortWorldItem* ItemInstance = nullptr;
		for (int i = 0; i < Inventory.ItemInstances.Num(); i++)
		{
			auto ItemInstance = Inventory.ItemInstances[i];

			if (ItemInstance->GetItemDefinitionBP() == ItemDef)
			{
				if (ItemDef->MaxStackSize > (Count + ItemInstance->ItemEntry.Count))
				{
					ItemInstance->ItemEntry.Count += Count;
					Inventory.MarkItemDirty(ItemInstance->ItemEntry);

					for (int i = 0; i < Inventory.ReplicatedEntries.Num(); i++)
					{
						auto Entry = &Inventory.ReplicatedEntries[i];

						if (Entry->ItemDefinition == ItemDef)
						{
							Entry->Count += Count;
							Inventory.MarkItemDirty(*Entry);
							return ItemInstance;
						}
					}
				}
				else {
					int AmountToAdd = 0;

					for (int j = 1; j < Count; j++)
					{
						if ((ItemInstance->ItemEntry.Count + 1) == ItemDef->MaxStackSize)
						{
							AmountToAdd = j - 1;
						}
					}

					ItemInstance->ItemEntry.Count += AmountToAdd;
					Inventory.MarkItemDirty(ItemInstance->ItemEntry);

					for (int j = 0; j < Inventory.ReplicatedEntries.Num(); j++)
					{
						auto Entry = &Inventory.ReplicatedEntries[j];

						if (Entry->ItemDefinition == ItemDef)
						{
							Entry->Count += AmountToAdd;
							Inventory.MarkItemDirty(*Entry);
						}
					}

					int AmountToDrop = Count - AmountToAdd;
					Spawners::SummonPickup((UFortWorldItemDefinition*)ItemDef, AmountToDrop, PlayerController->Pawn->K2_GetActorLocation());
					return ItemInstance;
				}
			}
		}

		auto NewItem = CreateItem(ItemDef, Count);
		AddItem43(PlayerController, NewItem);
		return NewItem;
	}

	inline FFortItemEntry AddItem2(AFortPlayerControllerAthena* Controller, UFortWorldItem* SlotInstance, int Slot, EFortQuickBars Bars = EFortQuickBars::Primary, int Count = 1)
	{
		if (!Controller) return {};
		if (!SlotInstance) return {};

		auto QuickBars = Controller->QuickBars;
		if (!QuickBars) return {};

		auto WorldInventory = Controller->WorldInventory;
		if (!WorldInventory) return {};

		auto& Inventory = WorldInventory->Inventory;

		auto& ItemEntry = SlotInstance->ItemEntry;

		ItemEntry.Count = Count;

		SlotInstance->SetOwningControllerForTemporaryItem(Controller);
		SlotInstance->OwnerInventory = WorldInventory;

		Inventory.ItemInstances.Add((UFortWorldItem*)SlotInstance);
		Inventory.ReplicatedEntries.Add(ItemEntry);

		//Update(Controller, Inventory.ReplicatedEntries.Add(ItemEntry));

		return ItemEntry;
	}
	UFortWorldItem* AddItem(AFortPlayerController* PlayerController, UFortWorldItem* Item, int Slot, EFortQuickBars Bars = EFortQuickBars::Primary, int Count = 0)
	{
		if (!PlayerController) return {};
		if (!Item) return {};

		if (Slot < 0)
			return {};

		if (Bars == EFortQuickBars::Primary && Slot >= 6)
			return {};

		PlayerController->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
		PlayerController->WorldInventory->Inventory.ItemInstances.Add(Item);
		return Item;
	}
	UFortWorldItem* AddItem23(AFortPlayerController* PlayerController, UFortWorldItem* Item)
	{
		PlayerController->WorldInventory->Inventory.ReplicatedEntries.Add(Item->ItemEntry);
		PlayerController->WorldInventory->Inventory.ItemInstances.Add(Item);
		return Item;
	}

	static void AddItem23332(AFortPlayerControllerAthena* PlayerController, UFortItemDefinition* ItemDef, EFortQuickBars QuickBar, int Slot, int Count, bool bAddToQuickbar = true)
	{
		if (!ItemDef || !PlayerController)
			return;

		auto ItemInstance = (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(1, 0);
		if (ItemInstance)
		{
			ItemInstance->ItemEntry.Count = Count;
			ItemInstance->SetOwningControllerForTemporaryItem(PlayerController);

			PlayerController->WorldInventory->Inventory.ItemInstances.Add(ItemInstance);
			PlayerController->WorldInventory->Inventory.ReplicatedEntries.Add(ItemInstance->ItemEntry);

			if (bAddToQuickbar)
				PlayerController->QuickBars->ServerAddItemInternal(ItemInstance->GetItemGuid(), QuickBar, Slot);
		}
	}
	static void AddItemToInventory(UFortItemDefinition* ItemDefinition, EFortQuickBars Quickbar, AFortPlayerControllerAthena* TargetController, int count, int Slot)
	{

		auto Inventory = TargetController->WorldInventory;

		auto QuickBars = TargetController->QuickBars;

		if (Quickbar == EFortQuickBars::Secondary)
		{
			UFortWorldItem* ItemInstance = nullptr;

			for (int i = 0; i < Inventory->Inventory.ItemInstances.Num(); i++)
			{
				if (Inventory->Inventory.ItemInstances[i]->GetItemDefinitionBP() == ItemDefinition)
					ItemInstance = Inventory->Inventory.ItemInstances[i];
			}

			if (ItemInstance)
			{
				auto NewCount = ItemInstance->ItemEntry.Count + count;
				for (int i = 0; i < Inventory->Inventory.ReplicatedEntries.Num(); i++)
				{
					if (Inventory->Inventory.ReplicatedEntries[i].ItemDefinition == ItemDefinition)
					{
						Inventory->Inventory.ReplicatedEntries[i].Count = NewCount;
						Inventory->Inventory.ReplicatedEntries[i].ReplicationKey++;
					}
				}
				ItemInstance->ItemEntry.Count = NewCount;
				ItemInstance->ItemEntry.ReplicationKey++;
				return;
			}
		}


		auto ItemInstance = (UFortWorldItem*)CreateItem2(TargetController, ItemDefinition, count);
		ItemInstance->OwnerInventory = Inventory;
		ItemInstance->bTemporaryItemOwningController = true;

		if (Inventory && QuickBars)
		{
			auto ReplicatedEntries = (TArray_<FFortItemEntry>*) & Inventory->Inventory.ReplicatedEntries;
			auto ItemInstances = (TArray_<UFortWorldItem*>*) & Inventory->Inventory.ItemInstances;

			ReplicatedEntries->AddSingle(ItemInstance->ItemEntry);
			ItemInstances->AddSingle(ItemInstance);



			TargetController->AddItemToQuickBars(ItemDefinition, Quickbar, Slot);
			TargetController->QuickBars->ServerAddItemInternal(ItemInstance->GetItemGuid(), Quickbar, Slot);

		}

		UpdateInventory(TargetController);
	}
	static auto AddItemToSlot(AFortPlayerControllerAthena* Controller, UFortWorldItemDefinition* Definition, int Slot, EFortQuickBars Bars = EFortQuickBars::Primary, int Count = 1, int* Idx = nullptr)
	{
		auto ret = FFortItemEntry();

		if (Definition->IsA(UFortWeaponItemDefinition::StaticClass()))
		{
			Count = 1;
		}

		if (Slot < 0)
		{
			Slot = 1;
		}

		if (Bars == EFortQuickBars::Primary && Slot >= 6)
		{
			Slot = 5;
		}

		if (auto TempItemInstance = static_cast<UFortWorldItem*>(Definition->CreateTemporaryItemInstanceBP(Count, 1)))
		{
			TempItemInstance->SetOwningControllerForTemporaryItem(Controller);

			TempItemInstance->ItemEntry.Count = Count;
			TempItemInstance->OwnerInventory = Controller->WorldInventory;

			auto& ItemEntry = TempItemInstance->ItemEntry;

			auto _Idx = Controller->WorldInventory->Inventory.ReplicatedEntries.Add(ItemEntry);

			if (Idx)
				*Idx = _Idx;

			Controller->WorldInventory->Inventory.ItemInstances.Add(TempItemInstance);
			Controller->QuickBars->ServerAddItemInternal(ItemEntry.ItemGuid, Bars, Slot);

			Abilities::Update(Controller, _Idx);

			ret = ItemEntry;
		}

		return ret;
	}

	template <typename Class>
	static FFortItemEntry FindItemInInventory(AFortPlayerControllerAthena* PC, bool& bFound)
	{
		auto& ItemInstances = GetItemInstances(PC);

		for (int i = 0; i < ItemInstances.Num(); i++)
		{
			auto ItemInstance = ItemInstances[i];

			if (!ItemInstance)
				continue;

			auto Def = ItemInstance->ItemEntry.ItemDefinition;

			if (Def && Def->IsA(Class::StaticClass()))
			{
				//std::cout << i << " Pickaxe Index\n";
				bFound = true;
				return ItemInstance->ItemEntry;
			}
		}

		bFound = false;
		return FFortItemEntry();
	}
	inline void IncreaseItemCount(AFortPlayerControllerAthena* PC, UFortItemDefinition* Item, int Amount)
	{
		auto Inventory = PC->WorldInventory;

		for (int i = 0; i < Inventory->Inventory.ReplicatedEntries.Num(); i++)
		{
			auto ItemDefinition = Inventory->Inventory.ReplicatedEntries[i].ItemDefinition;

			if (Item == ItemDefinition)
			{
				Inventory->Inventory.ReplicatedEntries[i].Count += Amount;
				Inventory->Inventory.ReplicatedEntries[i].ReplicationKey++;

				Update(PC);
				return;
			}
		}
	}
	inline FFortItemEntry AddNewItem(AFortPlayerControllerAthena* Controller, UFortItemDefinition* ItemDefinition, int Slot, EFortQuickBars Bars = EFortQuickBars::Primary, int Count = 1)
	{
		if (!Controller) return {};
		if (!ItemDefinition) return {};

		if (Slot < 0)
			return {};

		if (Bars == EFortQuickBars::Primary && Slot >= 6)
			return {};

		if (auto TempItemInstance = (UFortWorldItem*)ItemDefinition->CreateTemporaryItemInstanceBP(Count, 1))
		{
			return AddItem2(Controller, TempItemInstance, Slot, Bars, Count);
		}
	}
	static bool RemoveItemFromSlot(AFortPlayerControllerAthena* Controller, int Slot, EFortQuickBars Quickbars = EFortQuickBars::Primary, int Amount = -1) // -1 for all items in the slot
	{
		auto& PrimarySlots = Controller->QuickBars->PrimaryQuickBar.Slots;
		auto& SecondarySlots = Controller->QuickBars->SecondaryQuickBar.Slots;

		bool bPrimaryQuickBar = (Quickbars == EFortQuickBars::Primary);

		if (Amount == -1)
		{
			if (bPrimaryQuickBar)
				Amount = PrimarySlots[Slot].Items.Num();
			else
				Amount = SecondarySlots[Slot].Items.Num();
		}

		bool bWasSuccessful = false;

		for (int i = 0; i < Amount; i++)
		{
			// todo add a check to make sure the slot has that amount of items
			auto& ToRemoveGuid = bPrimaryQuickBar ? PrimarySlots[Slot].Items[i] : SecondarySlots[Slot].Items[i];

			for (int j = 0; j < Controller->WorldInventory->Inventory.ItemInstances.Num(); j++)
			{
				auto ItemInstance = Controller->WorldInventory->Inventory.ItemInstances[j];

				if (!ItemInstance)
					continue;

				auto Guid = ItemInstance->ItemEntry.ItemGuid;

				if (ToRemoveGuid == Guid)
				{
					Controller->WorldInventory->Inventory.ItemInstances.RemoveAt(j);
					bWasSuccessful = true;
					// break;
				}
			}

			for (int x = 0; x < Controller->WorldInventory->Inventory.ReplicatedEntries.Num(); x++)
			{
				auto& ItemEntry = Controller->WorldInventory->Inventory.ReplicatedEntries[x];

				if (ItemEntry.ItemGuid == ToRemoveGuid)
				{
					Controller->WorldInventory->Inventory.ReplicatedEntries.RemoveAt(x);
					bWasSuccessful = true;

					// break;
				}
			}

			Controller->QuickBars->ServerRemoveItemInternal(ToRemoveGuid, false, true);
			ToRemoveGuid.Reset();
		}

		if (bWasSuccessful) // Make sure it acutally removed from the ItemInstances and ReplicatedEntries
		{
			Controller->QuickBars->EmptySlot(Quickbars, Slot);

			if (bPrimaryQuickBar)
				PrimarySlots[Slot].Items.FreeArray();
			else
				SecondarySlots[Slot].Items.FreeArray();

			// bPrimaryQuickBar ? PrimarySlots[Slot].Items.FreeArray() : SecondarySlots[Slot].Items.FreeArray();
		}

		Abilities::UpdateInventory(Controller);

		return bWasSuccessful;
	}

	inline bool RemoveItem(AFortPlayerControllerAthena* PlayerController, int Slot, int Amount = -1, EFortQuickBars Quickbars = EFortQuickBars::Primary)
	{
		if (!PlayerController) return false;

		auto QuickBarSlots = (Quickbars == EFortQuickBars::Primary) ? PlayerController->QuickBars->PrimaryQuickBar.Slots : PlayerController->QuickBars->SecondaryQuickBar.Slots;

		if (Amount == -1)
			Amount = QuickBarSlots[Slot].Items.Num();

		bool bWasSuccessful = false;

		for (int i = 0; i < Amount; i++)
		{
			auto& ToRemoveGuid = QuickBarSlots[Slot].Items[i];

			for (int j = 0; j < PlayerController->WorldInventory->Inventory.ItemInstances.Num(); j++)
			{
				auto ItemInstance = PlayerController->WorldInventory->Inventory.ItemInstances[j];

				if (!ItemInstance)
					continue;

				auto Guid = ItemInstance->ItemEntry.ItemGuid;

				if (ToRemoveGuid == Guid)
				{
					PlayerController->WorldInventory->Inventory.ItemInstances.RemoveAt(j);
					bWasSuccessful = true;
				}
			}

			for (int x = 0; x < PlayerController->WorldInventory->Inventory.ReplicatedEntries.Num(); x++)
			{
				auto& ItemEntry = PlayerController->WorldInventory->Inventory.ReplicatedEntries[x];

				if (ItemEntry.ItemGuid == ToRemoveGuid)
				{
					PlayerController->WorldInventory->Inventory.ReplicatedEntries.RemoveAt(x);
					bWasSuccessful = true;
				}
			}

			ToRemoveGuid.Reset();
		}

		if (bWasSuccessful)
		{

			QuickBarSlots[Slot].Items.FreeArray();
		}

		UpdateInventory(PlayerController);

		return bWasSuccessful;
	}
	inline AFortWeapon* EquipWeaponDefinition(AFortPlayerControllerAthena* PlayerController, UFortWeaponItemDefinition* Definition, FGuid Guid, bool bEquipWithMaxAmmo = false)
	{
		if (!PlayerController) return 0;
		if (!Definition) return 0;

		auto Pawn = (AFortPlayerPawnAthena*)PlayerController->Pawn;
		if (!Pawn) return 0;

		auto Instance = GetInstanceFromGuid(PlayerController, Guid);
		if (!Instance) return 0;

		auto WeaponClass = Definition->GetWeaponActorClass();
		if (!WeaponClass) return 0;

		AFortWeapon* Weapon;

		if (WeaponClass->GetFullName() == "BlueprintGeneratedClass TrapTool.TrapTool_C")
		{
			Weapon = (AFortWeapon*)Spawners2::SpawnActor(WeaponClass, {}, PlayerController);
			if (!Weapon) return 0;

			Weapon->bReplicates = true;
			Weapon->bOnlyRelevantToOwner = false;
			((AFortTrapTool*)Weapon)->ItemDefinition = Definition;
		}
		else
		{
			Weapon = Pawn->EquipWeaponDefinition(Definition, Guid);
			if (!Weapon) return 0;

			if (bEquipWithMaxAmmo)
				Weapon->AmmoCount = Weapon->GetBulletsPerClip();

			Instance->ItemEntry.LoadedAmmo = Weapon->AmmoCount;
			Weapon->OnRep_AmmoCount(Instance->ItemEntry.LoadedAmmo);
		}

		Weapon->WeaponData = Definition;
		Weapon->ItemEntryGuid = Guid;
		Weapon->Owner = Pawn;
		Weapon->SetOwner(Pawn);
		Weapon->OnRep_ReplicatedWeaponData();
		Weapon->ClientGivenTo(Pawn);
		Pawn->ClientInternalEquipWeapon(Weapon);
		Pawn->OnRep_CurrentWeapon();

		return Weapon;
	}
	static inline std::vector<std::string> WeaponItemDefinitionArray = {
	   "/Game/Athena/Items/Weapons/LTM/WID_Hook_Gun_Slide.WID_Hook_Gun_Slide",
	   "/Game/Athena/Items/Weapons/WID_Special_FiendHunter_Athena_VR_Ore_T03.WID_Special_FiendHunter_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Sniper_Suppressed_Scope_Athena_VR_Ore_T03.WID_Sniper_Suppressed_Scope_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Sniper_Suppressed_Scope_Athena_SR_Ore_T03.WID_Sniper_Suppressed_Scope_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Sniper_Standard_Scope_Athena_VR_Ore_T03.WID_Sniper_Standard_Scope_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Sniper_Standard_Scope_Athena_SR_Ore_T03.WID_Sniper_Standard_Scope_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Sniper_NoScope_Athena_UC_Ore_T03.WID_Sniper_NoScope_Athena_UC_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Sniper_NoScope_Athena_R_Ore_T03.WID_Sniper_NoScope_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Sniper_Heavy_Athena_VR_Ore_T03.WID_Sniper_Heavy_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Sniper_Heavy_Athena_SR_Ore_T03.WID_Sniper_Heavy_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_SR_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_VR_Ore_T03.WID_Shotgun_Standard_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_SR_Ore_T03.WID_Shotgun_Standard_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_C_Ore_T03.WID_Shotgun_Standard_Athena_C_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Shotgun_SlugFire_Athena_VR.WID_Shotgun_SlugFire_Athena_VR",
	   "/Game/Athena/Items/Weapons/WID_Shotgun_SlugFire_Athena_SR.WID_Shotgun_SlugFire_Athena_SR",
	   "/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_UC_Ore_T03.WID_Shotgun_SemiAuto_Athena_UC_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_R_Ore_T03.WID_Shotgun_SemiAuto_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Shotgun_BreakBarrel_Athena_VR_Ore_T03.WID_Shotgun_BreakBarrel_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Shotgun_BreakBarrel_Athena_SR_Ore_T03.WID_Shotgun_BreakBarrel_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_SixShooter_Athena_UC_Ore_T03.WID_Pistol_SixShooter_Athena_UC_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_SixShooter_Athena_R_Ore_T03.WID_Pistol_SixShooter_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_SixShooter_Athena_C_Ore_T02.WID_Pistol_SixShooter_Athena_C_Ore_T02",
	   "/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_VR_Ore_T03.WID_Pistol_SemiAuto_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_UC_Ore_T03.WID_Pistol_SemiAuto_Athena_UC_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_SR_Ore_T03.WID_Pistol_SemiAuto_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_R_Ore_T03.WID_Pistol_SemiAuto_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_C_Ore_T02.WID_Pistol_SemiAuto_Athena_C_Ore_T02",
	   "/Game/Athena/Items/Weapons/WID_Pistol_Scoped_Athena_VR_Ore_T03.WID_Pistol_Scoped_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_Scoped_Athena_SR_Ore_T03.WID_Pistol_Scoped_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_Scavenger_Athena_VR_Ore_T03.WID_Pistol_Scavenger_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_Scavenger_Athena_UC_Ore_T03.WID_Pistol_Scavenger_Athena_UC_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_Scavenger_Athena_R_Ore_T03.WID_Pistol_Scavenger_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_Revolver_SingleAction_Athena_VR.WID_Pistol_Revolver_SingleAction_Athena_VR",
	   "/Game/Athena/Items/Weapons/WID_Pistol_Revolver_SingleAction_Athena_UC.WID_Pistol_Revolver_SingleAction_Athena_UC",
	   "/Game/Athena/Items/Weapons/WID_Pistol_Revolver_SingleAction_Athena_R.WID_Pistol_Revolver_SingleAction_Athena_R",
	   "/Game/Athena/Items/Weapons/WID_Pistol_HandCannon_Athena_VR_Ore_T03.WID_Pistol_HandCannon_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_HandCannon_Athena_SR_Ore_T03.WID_Pistol_HandCannon_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavy_Athena_UC_Ore_T03.WID_Pistol_AutoHeavy_Athena_UC_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavy_Athena_R_Ore_T03.WID_Pistol_AutoHeavy_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavy_Athena_C_Ore_T02.WID_Pistol_AutoHeavy_Athena_C_Ore_T02",
	   "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_UC_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_UC_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03.WID_Pistol_AutoHeavySuppressed_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavySuppressed_Athena_C_Ore_T02.WID_Pistol_AutoHeavySuppressed_Athena_C_Ore_T02",
	   "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_VR_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_UC_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_UC_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_SR_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_C_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_C_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_VR_Ore_T03.WID_Launcher_Rocket_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_SR_Ore_T03.WID_Launcher_Rocket_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Launcher_Rocket_Athena_R_Ore_T03.WID_Launcher_Rocket_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Launcher_Military_Athena_VR_Ore_T03.WID_Launcher_Military_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Launcher_Military_Athena_SR_Ore_T03.WID_Launcher_Military_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Launcher_Grenade_Athena_VR_Ore_T03.WID_Launcher_Grenade_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Launcher_Grenade_Athena_SR_Ore_T03.WID_Launcher_Grenade_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Launcher_Grenade_Athena_R_Ore_T03.WID_Launcher_Grenade_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Hook_Gun_VR_Ore_T03.WID_Hook_Gun_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_DualPistol_SemiAuto_Athena_VR_Ore_T03.WID_DualPistol_SemiAuto_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_DualPistol_SemiAuto_Athena_SR_Ore_T03.WID_DualPistol_SemiAuto_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_Surgical_Thermal_Athena_VR_Ore_T03.WID_Assault_Surgical_Thermal_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_Surgical_Thermal_Athena_SR_Ore_T03.WID_Assault_Surgical_Thermal_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_Surgical_Athena_VR_Ore_T03.WID_Assault_Surgical_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_Surgical_Athena_R_Ore_T03.WID_Assault_Surgical_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_Suppressed_Athena_VR_Ore_T03.WID_Assault_Suppressed_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_Suppressed_Athena_SR_Ore_T03.WID_Assault_Suppressed_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_VR_Ore_T03.WID_Assault_SemiAuto_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_UC_Ore_T03.WID_Assault_SemiAuto_Athena_UC_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_SR_Ore_T03.WID_Assault_SemiAuto_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_R_Ore_T03.WID_Assault_SemiAuto_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_SemiAuto_Athena_C_Ore_T02.WID_Assault_SemiAuto_Athena_C_Ore_T02",
	   "/Game/Athena/Items/Weapons/WID_Assault_LMG_Athena_VR_Ore_T03.WID_Assault_LMG_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_LMG_Athena_SR_Ore_T03.WID_Assault_LMG_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_LMGSAW_Athena_VR_Ore_T03.WID_Assault_LMGSAW_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_LMGSAW_Athena_R_Ore_T03.WID_Assault_LMGSAW_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_Heavy_Athena_VR_Ore_T03.WID_Assault_Heavy_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_Heavy_Athena_SR_Ore_T03.WID_Assault_Heavy_Athena_SR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_Heavy_Athena_R_Ore_T03.WID_Assault_Heavy_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_UC_Ore_T03.WID_Assault_Auto_Athena_UC_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_C_Ore_T02.WID_Assault_Auto_Athena_C_Ore_T02",
	   "/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03",
	   "/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03"
	};
	inline UFortWeaponItemDefinition* GetRandomWeapon()
	{
		return FindObjectFast<UFortWeaponItemDefinition>(WeaponItemDefinitionArray[rand() % WeaponItemDefinitionArray.size()]);
	}

	static inline std::vector<std::string> ConsumableItemDefinitionArray = {
	 "/Game/Athena/Items/Consumables/TowerGrenade/Athena_TowerGrenade.Athena_TowerGrenade",
	 "/Game/Athena/Items/Consumables/TNT/Athena_TNT.Athena_TNT",
	 "/Game/Athena/Items/Consumables/ThrownConsumables/IceGrenade/Athena_IceGrenade.Athena_IceGrenade",
	 "/Game/Athena/Items/Consumables/SuperTowerGrenade/Levels/SpikeArena/Athena_SpikeArena_Grenade.Athena_SpikeArena_Grenade",
	 "/Game/Athena/Items/Consumables/SuperTowerGrenade/Levels/PortAFort_A/Athena_SuperTowerGrenade_A.Athena_SuperTowerGrenade_A",
	 "/Game/Athena/Items/Consumables/SuperTowerGrenade/Levels/GiftBox/Athena_GiftBox.Athena_GiftBox",
	 "/Game/Athena/Items/Consumables/StickyGrenade/Athena_StickyGrenade.Athena_StickyGrenade",
	 "/Game/Athena/Items/Consumables/SmokeGrenade/Athena_SmokeGrenade.Athena_SmokeGrenade",
	 "/Game/Athena/Items/Consumables/ShockwaveGrenade/Athena_ShockGrenade.Athena_ShockGrenade",
	 "/Game/Athena/Items/Consumables/KnockGrenade/Athena_KnockGrenade.Athena_KnockGrenade",
	 "/Game/Athena/Items/Consumables/Grenade/Athena_Grenade.Athena_Grenade",
	 "/Game/Athena/Items/Consumables/GasGrenade/Athena_GasGrenade.Athena_GasGrenade",
	 "/Game/Athena/Items/Consumables/FireworksMortar/Athena_FireworksMortar.Athena_FireworksMortar",
	 "/Game/Athena/Items/Consumables/DanceGrenade/Athena_DanceGrenade.Athena_DanceGrenade",
	 "/Game/Athena/Items/Consumables/C4/Athena_C4.Athena_C4",
	 "/Game/Athena/Items/Consumables/BoomBox/BoomBox_Athena.BoomBox_Athena",
	 "/Game/Athena/Items/Consumables/Medkit/Athena_Medkit.Athena_Medkit",
	 "/Game/Athena/Items/Consumables/Bush/Athena_Bush.Athena_Bush",
	 "/Game/Athena/Items/Consumables/Bandage/Athena_Bandage.Athena_Bandage",
	 "/Game/Athena/Items/Consumables/Balloons/Athena_Balloons_Consumable.Athena_Balloons_Consumable",
	 "/Game/Athena/Items/Consumables/Balloons/Athena_Balloons.Athena_Balloons"
	};

	std::vector<UFortItemDefinition*> WeaponItemDefinitionAddresses = {};

	enum WeaponItemDefinitionNames {
		AssaultRifle_Grey, AssaultRifle_Green, AssaultRifle_Blue, AssaultRifle_Purple, AssaultRifle_Gold,
		PumpShotgun_Green, PumpShotgun_Blue,
		TacticalShotgun_Grey, TacticalShotgun_Green, TacticalShotgun_Blue,
		HeavyShotgun_Purple, HeavyShotgun_Gold,
		BoltAction_Blue, BoltAction_Purple, BoltAction_Gold,
		SemiAutoSniper_Green, SemiAutoSniper_Blue,
		SMG_Grey, SMG_Green, SMG_Blue,
		TacticalSMG_Green, TacticalSMG_Blue, TacticalSMG_Purple,
		Minigun_Purple, Minigun_Gold,
		Pistol_Grey, Pistol_Green, Pistol_Blue,
		SilencedPistol_Blue, SilencedPistol_Purple,
		Revolver_Grey, Revolver_Green, Revolver_Blue,
		RocketLauncher_Blue, RocketLauncher_Purple, RocketLauncher_Gold
	};
	inline UFortWeaponItemDefinition* GetRandomConsumable()
	{
		return FindObjectFast<UFortWeaponItemDefinition>(ConsumableItemDefinitionArray[rand() % ConsumableItemDefinitionArray.size()]);
	}
	static inline std::vector<std::string> AmmoItemDefinitionArray = {
	"/Game/Athena/Items/Ammo/AmmoDataRockets.AmmoDataRockets",
	"/Game/Athena/Items/Ammo/AthenaAmmoDataShells.AthenaAmmoDataShells",
	"/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium",
	"/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight",
	"/Game/Athena/Items/Ammo/AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy"
	};


	enum AmmoItemDefinitionNames {
		Rockets,
		Shells,
		Medium,
		Light,
		Heavy
	};

	static inline std::vector<std::string> ResourcePoolNames = {
	"/Game/Items/ResourcePickups/WoodItemData.WoodItemData",
	"/Game/Items/ResourcePickups/StoneItemData.StoneItemData",
	"/Game/Items/ResourcePickups/MetalItemData.MetalItemData"
	};
	std::vector<UFortItemDefinition*> AmmoItemDefinitionAddresses = {};



	enum MaterialItemDefinitionNames {
		Wood,
		Stone,
		Metal
	};

	std::vector<std::string> MaterialItemDefinitionArray = {
		"/Game/Items/ResourcePickups/WoodItemData.WoodItemData",
		"/Game/Items/ResourcePickups/StoneItemData.StoneItemData",
		"/Game/Items/ResourcePickups/MetalItemData.MetalItemData"
	};

	std::vector<UFortItemDefinition*> MaterialItemDefinitionAddresses = {};



	enum TrapItemDefinitionNames {
		LaunchPad,
		Campfire,
		Spike,
		Bouncer
	};

	std::vector<std::string> TrapItemDefinitionArray = {

	};

	std::vector<UFortItemDefinition*> TrapItemDefinitionAddresses = {};

	enum ConsumableItemDefinitionNames {
		Shields,
		ShieldSmall,
		PurpleStuff,
		SuperMedkit,
		Grenade,
		KnockGrenade,
		SmokeGrenade,
		StickyGrenade,
		Bush,
		Medkit,
		DanceGrenade
	};

	UFortItemDefinition* Pickaxe;

	std::vector<UFortItemDefinition*> ConsumableItemDefinitionAddresses = {};

	UFortItemDefinition* GetWeapon(bool bRandom, WeaponItemDefinitionNames name = {})
	{
		return WeaponItemDefinitionAddresses[bRandom ? (rand() % WeaponItemDefinitionArray.size()) : name];
	}

	UFortItemDefinition* GetConsumable(bool bRandom, int* count, ConsumableItemDefinitionNames name = {})
	{
		auto Consumable = ConsumableItemDefinitionAddresses[bRandom ? (rand() % ConsumableItemDefinitionArray.size()) : name];
		auto ConsumableName = Consumable->GetPersistentName().c_str();

		if (wcsstr(ConsumableName, L"athena_shieldsmall"))
			*count = 3;
		else if (wcsstr(ConsumableName, L"athena_shields"))
			*count = 1;
		else if (wcsstr(ConsumableName, L"athena_purplestuff"))
			*count = 1;
		else if (wcsstr(ConsumableName, L"athena_supermedkit"))
			*count = 1;
		else if (wcsstr(ConsumableName, L"athena_grenade"))
			*count = 3;
		else if (wcsstr(ConsumableName, L"athena_knockgrenade"))
			*count = 3;
		else if (wcsstr(ConsumableName, L"athena_smokegrenade"))
			*count = 3;
		else if (wcsstr(ConsumableName, L"athena_stickygrenade"))
			*count = 3;
		else if (wcsstr(ConsumableName, L"athena_bush"))
			*count = 1;
		else if (wcsstr(ConsumableName, L"athena_medkit"))
			*count = 1;
		else if (wcsstr(ConsumableName, L"athena_dancegrenade"))
			*count = 1;

		return Consumable;
	}

	bool IsConsumable(UFortItemDefinition* Definition)
	{
		if (!Definition) return false;

		auto ConsumableName = Definition->GetPersistentName().c_str();

		if (wcsstr(ConsumableName, L"athena_shieldsmall"))
			return true;
		else if (wcsstr(ConsumableName, L"athena_shields"))
			return true;
		else if (wcsstr(ConsumableName, L"athena_purplestuff"))
			return true;
		else if (wcsstr(ConsumableName, L"athena_supermedkit"))
			return true;
		else if (wcsstr(ConsumableName, L"athena_grenade"))
			return true;
		else if (wcsstr(ConsumableName, L"athena_knockgrenade"))
			return true;
		else if (wcsstr(ConsumableName, L"athena_smokegrenade"))
			return true;
		else if (wcsstr(ConsumableName, L"athena_stickygrenade"))
			return true;
		else if (wcsstr(ConsumableName, L"athena_bush"))
			return true;
		else if (wcsstr(ConsumableName, L"athena_medkit"))
			return true;
		else if (wcsstr(ConsumableName, L"athena_dancegrenade"))
			return true;

		return false;
	}
	UFortItemDefinition* GetAmmo(bool bRandom, int* count, AmmoItemDefinitionNames name = {})
	{
		auto Ammo = AmmoItemDefinitionAddresses[bRandom ? (rand() % AmmoItemDefinitionArray.size()) : name];
		*count = ((UFortAmmoItemDefinition*)Ammo)->DropCount;
		return Ammo;
	}
	UFortItemDefinition* GetAmmoOfWeapon(UFortItemDefinition* Weapon, int* count)
	{
		if (Weapon)
		{
			auto Ammo = ((UFortWorldItemDefinition*)Weapon)->GetAmmoWorldItemDefinition_BP();
			*count = ((UFortAmmoItemDefinition*)Ammo)->DropCount;
			return Ammo;
		}

		return GetAmmo(false, count, AmmoItemDefinitionNames::Medium);
	}


	inline UFortAmmoItemDefinition* GetRandomAmmo()
	{
		return FindObjectFast<UFortAmmoItemDefinition>(AmmoItemDefinitionArray[rand() % AmmoItemDefinitionArray.size()]);
	}
	inline UFortResourceItemDefinition* GetRandomMat()
	{
		return FindObjectFast<UFortResourceItemDefinition>(ResourcePoolNames[rand() % ResourcePoolNames.size()]);
	}
	static UFortWorldItem* CreateItem2(UFortItemDefinition* ItemDef, int Count = 1)
	{
		return (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(Count, 1);
	}
	static void Setup(AFortPlayerController* InPC)
	{
		auto QuickBars = Spawners::SpawnActor<AFortQuickBarsAthena>();
		QuickBars->SetOwner(InPC);
		InPC->QuickBars = QuickBars;
		InPC->OnRep_QuickBar();

		static auto PickaxeDef = FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Assassin.WID_Harvest_Pickaxe_Assassin");
		static auto WallDef = FindObjectFast<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Wall.BuildingItemData_Wall");
		static auto FloorDef = FindObjectFast<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Floor.BuildingItemData_Floor");
		static auto StairDef = FindObjectFast<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_Stair_W.BuildingItemData_Stair_W");
		static auto RoofDef = FindObjectFast<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/BuildingItemData_RoofS.BuildingItemData_RoofS");
		static auto EditToolDef = FindObjectFast<UFortItemDefinition>("/Game/Items/Weapons/BuildingTools/EditTool.EditTool");
		static auto PumpDef = FindObjectFast<UFortItemDefinition>("/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_C_Ore_T03.WID_Shotgun_Standard_Athena_C_Ore_T03");

		auto EditTool = Abilities::CreateItem2(EditToolDef);
		InPC->WorldInventory->Inventory.ItemInstances.Add(EditTool);
		InPC->WorldInventory->Inventory.ReplicatedEntries.Add(EditTool->ItemEntry);

		auto PickaxeItem = Abilities::CreateItem2(PickaxeDef);
		InPC->WorldInventory->Inventory.ItemInstances.Add(PickaxeItem);
		InPC->WorldInventory->Inventory.ReplicatedEntries.Add(PickaxeItem->ItemEntry);
		QuickBars->ServerAddItemInternal(PickaxeItem->GetItemGuid(), EFortQuickBars::Primary, 0);

		auto WallItem = Abilities::CreateItem2(WallDef);
		InPC->WorldInventory->Inventory.ItemInstances.Add(WallItem);
		InPC->WorldInventory->Inventory.ReplicatedEntries.Add(WallItem->ItemEntry);
		QuickBars->ServerAddItemInternal(WallItem->GetItemGuid(), EFortQuickBars::Secondary, 0);

		auto PumpItem = Abilities::CreateItem2(PumpDef);
		InPC->WorldInventory->Inventory.ItemInstances.Add(PumpItem);
		InPC->WorldInventory->Inventory.ReplicatedEntries.Add(PumpItem->ItemEntry);
		QuickBars->ServerAddItemInternal(PumpItem->GetItemGuid(), EFortQuickBars::Primary, 1);


		auto FloorItem = Abilities::CreateItem2(FloorDef);
		InPC->WorldInventory->Inventory.ItemInstances.Add(FloorItem);
		InPC->WorldInventory->Inventory.ReplicatedEntries.Add(FloorItem->ItemEntry);
		QuickBars->ServerAddItemInternal(FloorItem->GetItemGuid(), EFortQuickBars::Secondary, 1);

		auto StairItem = Abilities::CreateItem2(StairDef);
		InPC->WorldInventory->Inventory.ItemInstances.Add(StairItem);
		InPC->WorldInventory->Inventory.ReplicatedEntries.Add(StairItem->ItemEntry);
		QuickBars->ServerAddItemInternal(StairItem->GetItemGuid(), EFortQuickBars::Secondary, 2);

		auto RoofItem = Abilities::CreateItem2(RoofDef);
		InPC->WorldInventory->Inventory.ItemInstances.Add(RoofItem);
		InPC->WorldInventory->Inventory.ReplicatedEntries.Add(RoofItem->ItemEntry);
		QuickBars->ServerAddItemInternal(RoofItem->GetItemGuid(), EFortQuickBars::Secondary, 3);

		UpdateInventory(InPC);
	}
	void Initialize(UObject* Object, UFunction* Function, void* Parameters, std::string ObjectName, std::string FunctionName)
	{
		auto FuncName = Function->GetName();

		if (FuncName == "ServerAttemptAircraftJump")
		{
			auto params = (AFortPlayerControllerAthena_ServerAttemptAircraftJump_Params*)(Parameters);

			auto PC = (AFortPlayerControllerAthena*)Object;

			if (PC->Pawn)
				return oProcessEvent(Object, Function, Parameters);

			auto Pawn = Spawners::SpawnActor<AFortPlayerPawnAthena>(((AFortGameStateAthena*)GetWorld()->GameState)->GetAircraft(0)->K2_GetActorLocation(), {}, Globals::PawnClass);
			PC->Possess(Pawn);

			PC->SetControlRotation(params->ClientRotation);

			Pawn->bCanBeDamaged = true;
		}
	}

}