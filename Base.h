#pragma once

#include "Utils.h"
#include "Globals.h"
#include "UE4.h"
#include "Abilities.h"
#include <random>
#include <map>

inline bool bListening = false;

inline int teamidx = 4;

bool GuidComp(FGuid guidA, FGuid guidB)
{
    if (guidA.A == guidB.A && guidA.B == guidB.B && guidA.C == guidB.C && guidA.D == guidB.D)
        return true;
    else
        return false;
}

std::vector<std::string> AthenaLootPool = {
        "/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_SR_Ore_T03.WID_Assault_AutoHigh_Athena_SR_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Assault_AutoHigh_Athena_VR_Ore_T03.WID_Assault_AutoHigh_Athena_VR_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_R_Ore_T03.WID_Assault_Auto_Athena_R_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_UC_Ore_T03.WID_Assault_Auto_Athena_UC_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Assault_Auto_Athena_C_Ore_T02.WID_Assault_Auto_Athena_C_Ore_T02",
        "/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_C_Ore_T03.WID_Shotgun_Standard_Athena_C_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_C_Ore_T03.WID_Shotgun_Standard_Athena_C_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Shotgun_Standard_Athena_UC_Ore_T03.WID_Shotgun_Standard_Athena_UC_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_UC_Ore_T03.WID_Shotgun_SemiAuto_Athena_UC_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_R_Ore_T03.WID_Shotgun_SemiAuto_Athena_R_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Shotgun_SemiAuto_Athena_VR_Ore_T03.WID_Shotgun_SemiAuto_Athena_VR_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Shotgun_SlugFire_Athena_VR.WID_Shotgun_SlugFire_Athena_VR",
        "/Game/Athena/Items/Weapons/WID_Shotgun_SlugFire_Athena_SR.WID_Shotgun_SlugFire_Athena_SR",
        "/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_R_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_VR_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Sniper_BoltAction_Scope_Athena_SR_Ore_T03.WID_Sniper_BoltAction_Scope_Athena_SR_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Sniper_Standard_Scope_Athena_VR_Ore_T03.WID_Sniper_Standard_Scope_Athena_VR_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Sniper_Standard_Scope_Athena_SR_Ore_T03.WID_Sniper_Standard_Scope_Athena_SR_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_C_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_C_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_UC_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_UC_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03.WID_Pistol_AutoHeavyPDW_Athena_R_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Pistol_Scavenger_Athena_UC_Ore_T03.WID_Pistol_Scavenger_Athena_UC_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Pistol_Scavenger_Athena_R_Ore_T03.WID_Pistol_Scavenger_Athena_R_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Pistol_Scavenger_Athena_VR_Ore_T03.WID_Pistol_Scavenger_Athena_VR_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Assault_LMG_Athena_VR_Ore_T03.WID_Assault_LMG_Athena_VR_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Assault_LMG_Athena_SR_Ore_T03.WID_Assault_LMG_Athena_SR_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_C_Ore_T02.WID_Pistol_SemiAuto_Athena_C_Ore_T02",
        "/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_UC_Ore_T03.WID_Pistol_SemiAuto_Athena_UC_Ore_T03",
        "/Game/Athena/Items/Weapons/WID_Pistol_SemiAuto_Athena_R_Ore_T03.WID_Pistol_SemiAuto_Athena_R_Ore_T03",
};

static UFortItemDefinition* GetRandomItemDefinition()
{
    while (true)
    {
        auto Idx = GetMath()->RandomInteger(AthenaLootPool.size());
        auto Item = AthenaLootPool[Idx];
        auto Def = FindObjectFast<UFortItemDefinition>(Item);
        if (!Def)
            continue;

        auto rng = std::default_random_engine{};
        std::shuffle(AthenaLootPool.begin(), AthenaLootPool.end(), rng);
        return Def;
    }
}

static AFortPickupAthena* SpawnPickup(FVector Location, UFortItemDefinition* ItemDef = GetRandomItemDefinition(), int Count = 1)
{
    if (Count == 1 && ItemDef->IsA(UFortAmmoItemDefinition::StaticClass()))
        Count = ((UFortAmmoItemDefinition*)(ItemDef))->DropCount;

    auto Pickup = Spawners::SpawnActor<AFortPickupAthena>(Location, {});

    Pickup->PrimaryPickupItemEntry.ItemDefinition = ItemDef;
    Pickup->PrimaryPickupItemEntry.Count = Count;

    Pickup->TossPickup(Location, nullptr, 6, true);

    return Pickup;
}


static inline void TickFlushHook(UNetDriver* NetDriver)
{
    if (NetDriver->ReplicationDriver)
    {
        Drivers::ServerReplicateActors((UReplicationGraph*)NetDriver->ReplicationDriver);
    }  if (GetAsyncKeyState(VK_F6) & 1)
    {
        auto GameState = (AFortGameStateAthena*)GetWorld()->GameState;
        GameState->bGameModeWillSkipAircraft = false;
        GameState->AircraftStartTime = 0;
        GameState->WarmupCountdownEndTime = 0;
        ((UKismetSystemLibrary*)UKismetSystemLibrary::StaticClass())->ExecuteConsoleCommand(GetWorld(), L"startaircraft", nullptr);
    }
    return Drivers::TickFlush(NetDriver);
}

inline FTransform GetPlayerStart(AFortPlayerControllerAthena* Controller)
{
    TArray<AActor*> OutActors;
    ((UGameplayStatics*)UGameplayStatics::StaticClass())->GetAllActorsOfClass(GetWorld(), AFortPlayerStartWarmup::StaticClass(), &OutActors);

    auto ActorsNum = OutActors.Num();

    auto SpawnTransform = FTransform();
    SpawnTransform.Scale3D = FVector(1, 1, 1);
    SpawnTransform.Rotation = FQuat();
    SpawnTransform.Translation = FVector{ -124461, -116273, 7000 };

    auto GamePhase = ((AFortGameStateAthena*)GetWorld()->GameState)->GamePhase;

    if (ActorsNum != 0 && (GamePhase == EAthenaGamePhase::Setup || GamePhase == EAthenaGamePhase::Warmup))
    {
        auto ActorToUseNum = rand() % ActorsNum;
        auto ActorToUse = OutActors[ActorToUseNum];

        while (!ActorToUse)
        {
            ActorToUseNum = rand() % ActorsNum;
            ActorToUse = OutActors[ActorToUseNum];
        }

        SpawnTransform.Translation = ActorToUse->K2_GetActorLocation();

        Controller->WarmupPlayerStart = (AFortPlayerStartWarmup*)ActorToUse;
    }

    OutActors.FreeArray();

    return SpawnTransform;
}

static inline void (*SetClientLoginState)(UNetConnection*, uint8_t);
static inline void SetClientLoginStateHook(UNetConnection* NetConnection, uint8_t State, FVector Location)
{
    if (State == 3)
    {

        auto PlayerController = (AFortPlayerControllerAthena*)(NetConnection->PlayerController);

        auto PlayerState = (AFortPlayerStateAthena*)PlayerController->PlayerState;

        auto Pawn = Pawn::InitializePawn(PlayerController, GetPlayerStart(PlayerController).Translation);

        PlayerController->bHasClientFinishedLoading = true;
        PlayerController->bHasServerFinishedLoading = true;
        PlayerController->bHasInitiallySpawned = true;
        PlayerController->OnRep_bHasServerFinishedLoading();

        PlayerState->bHasFinishedLoading = true;
        PlayerState->bHasStartedPlaying = true;
        PlayerState->OnRep_bHasStartedPlaying();

        static auto BodyPart = FindObjectFast<UCustomCharacterPart>("/Game/Characters/CharacterParts/Female/Medium/Bodies/F_Med_Soldier_01.F_Med_Soldier_01");
        static auto HeadPart = FindObjectFast<UCustomCharacterPart>("/Game/Characters/CharacterParts/Female/Medium/Heads/F_Med_Head1.F_Med_Head1");

        Pawn->ServerChoosePart(EFortCustomPartType::Head, HeadPart);
        Pawn->ServerChoosePart(EFortCustomPartType::Body, BodyPart);
        ((AFortPlayerState*)Pawn->PlayerState)->OnRep_CharacterParts();

        PlayerController->OverriddenBackpackSize = 5;

        Abilities::GiveAbilities(Pawn);
        Abilities::Setup(PlayerController);
        Abilities::UpdateInventory(PlayerController);

        PlayerState->TeamIndex = EFortTeam(teamidx);

        PlayerState->PlayerTeam->TeamMembers.Add(PlayerController);
        PlayerState->PlayerTeam->Team = EFortTeam(teamidx);

        PlayerState->SquadId = teamidx - 1;
        PlayerState->OnRep_PlayerTeam();
        PlayerState->OnRep_SquadId();

        teamidx++;

    }

    return SetClientLoginState(NetConnection, State);
}

bool bSetup = false;

void ProcessEventHook(UObject* Object, UFunction* Function, void* Parameters)
{
    auto ObjectName = Object->GetFullName();
    auto FunctionName = Function->GetFullName();
    auto FuncName = Function->GetName();
    auto FullFuncName = Function->GetFullName();

    if (FuncName == "ServerTryActivateAbility")
    {
        auto params = (UAbilitySystemComponent_ServerTryActivateAbility_Params*)(Parameters);

        UGameplayAbility* InstancedAbility = nullptr;
        if (!Abilities::InternalTryActivateAbility((UAbilitySystemComponent*)Object, params->AbilityToActivate, params->PredictionKey, &InstancedAbility, nullptr, nullptr))
        {
            ((UAbilitySystemComponent*)Object)->ClientActivateAbilityFailed(params->AbilityToActivate, params->PredictionKey.Base);
        }

        return;
    }

    if (FuncName == "ServerAbilityRPCBatch")
    {
        auto AbilityComp = (UAbilitySystemComponent*)Object;
        auto CurrentParams = (UAbilitySystemComponent_ServerAbilityRPCBatch_Params*)Parameters;

        UGameplayAbility* InstancedAbility = nullptr;
        if (!Abilities::InternalTryActivateAbility(AbilityComp, CurrentParams->BatchInfo.AbilitySpecHandle, CurrentParams->BatchInfo.PredictionKey, &InstancedAbility, nullptr, nullptr))
        {
            AbilityComp->ClientActivateAbilityFailed(CurrentParams->BatchInfo.AbilitySpecHandle, CurrentParams->BatchInfo.PredictionKey.Base);
        }
    }
    if (FuncName.contains("OnDamageServer"))
    {
        if (!Object->IsA(ABuildingSMActor::StaticClass()))
            return oProcessEvent(Object, Function, Parameters);

        auto BuildingActor = (ABuildingSMActor*)Object;
        auto Params = (ABuildingActor_OnDamageServer_Params*)Parameters;

        if (Params->InstigatedBy && Params->InstigatedBy->IsA(AFortPlayerController::StaticClass()) && !BuildingActor->bPlayerPlaced)
        {
            auto FortController = (AFortPlayerController*)Params->InstigatedBy;

            if (FortController->MyFortPawn->CurrentWeapon && FortController->MyFortPawn->CurrentWeapon->WeaponData == FindObjectFast<UFortWeaponMeleeItemDefinition>("/Game/Athena/Items/Weapons/WID_Harvest_Pickaxe_Assassin.WID_Harvest_Pickaxe_Assassin"))
                FortController->ClientReportDamagedResourceBuilding(BuildingActor, BuildingActor->ResourceType, GetMath()->RandomIntegerInRange(3, 6), false, true);
        }
    }
    if (FuncName.contains("ClientReportDamagedResourceBuilding"))
    {
        auto PC = (AFortPlayerControllerAthena*)Object;
        auto Params = (AFortPlayerController_ClientReportDamagedResourceBuilding_Params*)Parameters;

        if (PC && Params)
        {
            UFortResourceItemDefinition* ItemDef = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");

            if (Params->PotentialResourceType == EFortResourceType::Wood)
                ItemDef = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");

            if (Params->PotentialResourceType == EFortResourceType::Stone)
                ItemDef = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");

            if (Params->PotentialResourceType == EFortResourceType::Metal)
                ItemDef = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

            int Count = 0;

            auto WorldInventory = PC->WorldInventory;

            for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++)
            {
                auto ItemInstance = WorldInventory->Inventory.ItemInstances[i];

                if (ItemInstance->GetItemDefinitionBP() == ItemDef)
                {
                    WorldInventory->Inventory.ItemInstances.Remove(i);

                    for (int j = 0; j < WorldInventory->Inventory.ReplicatedEntries.Num(); j++)
                    {
                        auto Entry = WorldInventory->Inventory.ReplicatedEntries[j];

                        if (Entry.ItemDefinition == ItemDef)
                        {
                            WorldInventory->Inventory.ReplicatedEntries.Remove(j);
                            Count = Entry.Count;
                        }
                    }
                }
            }

            auto NewPickupWorldItem = (UFortWorldItem*)ItemDef->CreateTemporaryItemInstanceBP(Count + Params->PotentialResourceCount, 1);

            NewPickupWorldItem->bTemporaryItemOwningController = true;
            NewPickupWorldItem->SetOwningControllerForTemporaryItem(PC);

            WorldInventory->Inventory.ItemInstances.Add(NewPickupWorldItem);
            WorldInventory->Inventory.ReplicatedEntries.Add(NewPickupWorldItem->ItemEntry);

            Abilities::UpdateInventory(PC);
        }
    }
    if (FuncName == "ServerAttemptInventoryDrop")
    {
        auto params = (AFortPlayerController_ServerAttemptInventoryDrop_Params*)(Parameters);
        auto PlayerController = (AFortPlayerControllerAthena*)(Object);

        if (!PlayerController->MyFortPawn)
            return;

        if (PlayerController->IsInAircraft())
            return;

        if (PlayerController->MyFortPawn->IsSkydiving())
            return;

        auto WorldInventory = PlayerController->WorldInventory;
        auto QuickBars = PlayerController->QuickBars;

        for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++)
        {
            auto ItemInstance = WorldInventory->Inventory.ItemInstances[i];

            if (UKismetGuidLibrary::EqualEqual_GuidGuid(ItemInstance->GetItemGuid(), params->ItemGuid))
            {
                if (ItemInstance->ItemEntry.Count == ItemInstance->ItemEntry.Count)
                {
                    WorldInventory->Inventory.ItemInstances.Remove(i);

                    Spawners::SpawnPickup32((UFortWorldItemDefinition*)ItemInstance->GetItemDefinitionBP(), ItemInstance->ItemEntry.Count, PlayerController->MyFortPawn->K2_GetActorLocation());

                    for (int j = 0; j < WorldInventory->Inventory.ReplicatedEntries.Num(); j++)
                    {
                        auto Entry = WorldInventory->Inventory.ReplicatedEntries[j];

                        if (UKismetGuidLibrary::EqualEqual_GuidGuid(Entry.ItemGuid, params->ItemGuid))
                        {
                            WorldInventory->Inventory.ReplicatedEntries.Remove(j);
                        }
                    }
                }
                else {
                    ItemInstance->ItemEntry.Count -= ItemInstance->ItemEntry.Count;
                    WorldInventory->Inventory.MarkItemDirty(ItemInstance->ItemEntry);

                    Spawners::SpawnPickup32((UFortWorldItemDefinition*)ItemInstance->GetItemDefinitionBP(), ItemInstance->ItemEntry.Count, PlayerController->MyFortPawn->K2_GetActorLocation());

                    for (int j = 0; j < WorldInventory->Inventory.ReplicatedEntries.Num(); j++)
                    {
                        auto Entry = &WorldInventory->Inventory.ReplicatedEntries[j];

                        if (UKismetGuidLibrary::EqualEqual_GuidGuid(Entry->ItemGuid, params->ItemGuid))
                        {
                            Entry->Count -= ItemInstance->ItemEntry.Count;
                            WorldInventory->Inventory.MarkItemDirty(*Entry);
                        }
                    }
                }
            }
        }

        for (int i = 0; i < QuickBars->PrimaryQuickBar.Slots.Num(); i++)
        {
            auto Slot = QuickBars->PrimaryQuickBar.Slots[i];

            if (Slot.Items.IsValidIndex(0))
            {
                if (UKismetGuidLibrary::IsValid_Guid(Slot.Items[0]))
                {
                    if (UKismetGuidLibrary::EqualEqual_GuidGuid(Slot.Items[0], params->ItemGuid))
                    {
                        PlayerController->QuickBars->EmptySlot(EFortQuickBars::Primary, i);
                    }
                }
            }
        }

        Abilities::UpdateInventory(PlayerController);
    }

    if (FuncName == "ServerHandlePickup") {
        auto Pawn = (AFortPlayerPawnAthena*)Object;
        auto PlayerController = (AFortPlayerControllerAthena*)Pawn->Controller;
        auto Params = (AFortPlayerPawn_ServerHandlePickup_Params*)Parameters;

            auto Pickup = Params->Pickup;
            auto ItemDef = Pickup->PrimaryPickupItemEntry.ItemDefinition;
            auto ItemInstances = PlayerController->WorldInventory->Inventory.ItemInstances;
            auto QuickBars = PlayerController->QuickBars;
            auto QuickbarSlots = PlayerController->QuickBars->PrimaryQuickBar.Slots;
            auto WorldInventory = PlayerController->WorldInventory;
            auto NewItem = Abilities::AddItem54(PlayerController, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count);

            bool force_give_weapon = false;

            Pickup->K2_DestroyActor();

            if (!ItemDef->IsA(UFortWeaponItemDefinition::StaticClass()) || !ItemDef->IsA(UFortWeaponRangedItemDefinition::StaticClass()) ||!ItemDef->IsA(UFortConsumableItemDefinition::StaticClass()))
            {
                auto PrimaryQuickBarSlots = PlayerController->QuickBars->PrimaryQuickBar.Slots;

                for (int i = 1; i < PrimaryQuickBarSlots.Num(); i++)
                {
                    if (i == PrimaryQuickBarSlots.Num() - 1 && PrimaryQuickBarSlots[i].Items.Data) //Slots are full
                    {
                        auto FocusedSlot = QuickBars->PrimaryQuickBar.CurrentFocusedSlot;
                        if (!FocusedSlot) return; //Pickaxe 

                        i = FocusedSlot;

                        if (PrimaryQuickBarSlots.Num() < FocusedSlot) continue;
                        if (!PrimaryQuickBarSlots[FocusedSlot].Items.Num()) continue;

                        FGuid& FocusedGuid = PrimaryQuickBarSlots[FocusedSlot].Items[0];

                        for (int j = 0; j < ItemInstances.Num(); j++)
                        {
                            auto ItemInstance = ItemInstances[j];
                            if (!ItemInstance) continue;

                            if (ItemInstance->ItemEntry.ItemGuid == FocusedGuid)
                            {
                                auto Pickup = Spawners::SummonPickup(ItemInstance->ItemEntry.ItemDefinition, ItemInstance->ItemEntry.Count, Pawn->K2_GetActorLocation()); //Drop the weapon to replace
                                Pickup->Owner = PlayerController;

                                auto Weapon = Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)ItemInstance->ItemEntry.ItemDefinition, FocusedGuid);
                                if (Weapon)
                                    ItemInstance->ItemEntry.LoadedAmmo = Weapon->AmmoCount;

                                Pickup->PrimaryPickupItemEntry.LoadedAmmo = ItemInstance->ItemEntry.LoadedAmmo;

                                Abilities::RemoveItem(PlayerController, FocusedSlot);

                                force_give_weapon = true;

                                break;
                            }
                        }

                        continue;
                    }

                    if (!PrimaryQuickBarSlots[i].Items.Data || force_give_weapon)
                    {
                        auto entry = Abilities::AddNewItem(PlayerController, ItemDef, i, EFortQuickBars::Primary, Params->Pickup->PrimaryPickupItemEntry.Count);

                        auto Instance = Abilities::GetInstanceFromGuid(PlayerController, entry.ItemGuid);
                        if (!Instance) return;

                        if (!Params->Pickup->Owner)
                        {
                            auto Weapon = Abilities::EquipWeaponDefinition(PlayerController, (UFortWeaponItemDefinition*)Params->Pickup->PrimaryPickupItemEntry.ItemDefinition, entry.ItemGuid, true);
                            if (Weapon)
                                Params->Pickup->PrimaryPickupItemEntry.LoadedAmmo = Weapon->AmmoCount;
                        }

                        Params->Pickup->Owner = PlayerController;
                        Instance->ItemEntry.LoadedAmmo = Params->Pickup->PrimaryPickupItemEntry.LoadedAmmo;

                        Params->Pickup->PickupLocationData.PickupTarget = Pawn;
                        Params->Pickup->PickupLocationData.PickupTarget = Pawn;
                        Params->Pickup->PickupLocationData.FlyTime = 0.75f;
                        Params->Pickup->PickupLocationData.ItemOwner = Pawn;
                        Params->Pickup->OnRep_PickupLocationData();
                        Params->Pickup->bPickedUp = true;
                        Params->Pickup->OnRep_bPickedUp();

                        //Params->Pickup->K2_DestroyActor();

                        Abilities::UpdateInventory(PlayerController);
                        return;
                    }
                }
            
            Abilities::UpdateInventory(PlayerController);

                return oProcessEvent(Object, Function, Parameters);
            
           
        }
    }
    if (FuncName == "ServerHandlePickupWithSwap")
    {
        auto Pawn = (AFortPlayerPawnAthena*)Object;
        auto PlayerController = (AFortPlayerControllerAthena*)Pawn->Controller;
        if (!PlayerController || PlayerController->IsInAircraft()) return;

        auto Params = (AFortPlayerPawn_ServerHandlePickupWithSwap_Params*)Parameters;
        if (!Params || !Params->Pickup) return;
        if (Params->Pickup->IsActorBeingDestroyed() || Params->Pickup->bPickedUp) return;

        auto Pickup = Params->Pickup;
        auto ItemDef = Pickup->PrimaryPickupItemEntry.ItemDefinition;
        auto& ItemInstances = PlayerController->WorldInventory->Inventory.ItemInstances;

        auto QuickBars = PlayerController->QuickBars;
        if (!QuickBars) return;
        auto WorldInventory = PlayerController->WorldInventory;
        auto NewItem = Abilities::AddItem54(PlayerController, Pickup->PrimaryPickupItemEntry.ItemDefinition, Pickup->PrimaryPickupItemEntry.Count);

        bool force_give_weapon = false;

        Pickup->K2_DestroyActor();

        if (!ItemDef->IsA(UFortWeaponItemDefinition::StaticClass()) || !ItemDef->IsA(UFortWeaponRangedItemDefinition::StaticClass()) || !ItemDef->IsA(UFortConsumableItemDefinition::StaticClass()))
        {
            auto PrimaryQuickBarSlots = PlayerController->QuickBars->PrimaryQuickBar.Slots;

            for (int i = 1; i < PrimaryQuickBarSlots.Num(); i++)
            {
                if (i == PrimaryQuickBarSlots.Num() - 1 && PrimaryQuickBarSlots[i].Items.Data) //Slots are full
                {
                    auto FocusedSlot = QuickBars->PrimaryQuickBar.CurrentFocusedSlot;
                    if (!FocusedSlot) return; //Pickaxe 

                    i = FocusedSlot;

                    if (PrimaryQuickBarSlots.Num() < FocusedSlot) continue;
                    if (!PrimaryQuickBarSlots[FocusedSlot].Items.Num()) continue;

                    FGuid& FocusedGuid = PrimaryQuickBarSlots[FocusedSlot].Items[0];

                    for (int j = 0; j < ItemInstances.Num(); j++)
                    {
                        auto ItemInstance = ItemInstances[j];
                        if (!ItemInstance) continue;

                        if (ItemInstance->ItemEntry.ItemGuid == FocusedGuid)
                        {
                            auto Pickup = Spawners::SummonPickup(ItemInstance->ItemEntry.ItemDefinition, ItemInstance->ItemEntry.Count, Pawn->K2_GetActorLocation()); //Drop the weapon to replace
                            Pickup->Owner = PlayerController;

                            auto Weapon = Pawn->EquipWeaponDefinition((UFortWeaponItemDefinition*)ItemInstance->ItemEntry.ItemDefinition, FocusedGuid);
                            if (Weapon)
                                ItemInstance->ItemEntry.LoadedAmmo = Weapon->AmmoCount;

                            Pickup->PrimaryPickupItemEntry.LoadedAmmo = ItemInstance->ItemEntry.LoadedAmmo;

                            Abilities::RemoveItem(PlayerController, FocusedSlot);

                            force_give_weapon = true;

                            break;
                        }
                    }

                    continue;
                }

                if (!PrimaryQuickBarSlots[i].Items.Data || force_give_weapon)
                {
                    auto entry = Abilities::AddNewItem(PlayerController, ItemDef, i, EFortQuickBars::Primary, Params->Pickup->PrimaryPickupItemEntry.Count);

                    auto Instance = Abilities::GetInstanceFromGuid(PlayerController, entry.ItemGuid);
                    if (!Instance) return;

                    if (!Params->Pickup->Owner)
                    {
                        auto Weapon = Abilities::EquipWeaponDefinition(PlayerController, (UFortWeaponItemDefinition*)Params->Pickup->PrimaryPickupItemEntry.ItemDefinition, entry.ItemGuid, true);
                        if (Weapon)
                            Params->Pickup->PrimaryPickupItemEntry.LoadedAmmo = Weapon->AmmoCount;
                    }

                    Params->Pickup->Owner = PlayerController;
                    Instance->ItemEntry.LoadedAmmo = Params->Pickup->PrimaryPickupItemEntry.LoadedAmmo;

                    Params->Pickup->PickupLocationData.PickupTarget = Pawn;
                    Params->Pickup->PickupLocationData.PickupTarget = Pawn;
                    Params->Pickup->PickupLocationData.FlyTime = 0.75f;
                    Params->Pickup->PickupLocationData.ItemOwner = Pawn;
                    Params->Pickup->OnRep_PickupLocationData();
                    Params->Pickup->bPickedUp = true;
                    Params->Pickup->OnRep_bPickedUp();

                    //Params->Pickup->K2_DestroyActor();

                    Abilities::UpdateInventory(PlayerController);
                    return;
                }
            }

            Abilities::UpdateInventory(PlayerController);
        }
    }
    if (FuncName == "ServerCreateBuildingActor")
    {
        auto params = (AFortPlayerController_ServerCreateBuildingActor_Params*)(Parameters);
        auto BuildClass = params->BuildingClassData.BuildingClass;
        auto Loc = params->BuildLoc;
        auto Rot = params->BuildRot;

        auto BuildingActor = Spawners::SpawnActor<ABuildingSMActor>(Loc, Rot, BuildClass);
        if (BuildingActor)
        {
            BuildingActor->Team = ((AFortPlayerStateAthena*)((AFortPlayerController*)Object)->PlayerState)->TeamIndex;
            BuildingActor->OnRep_Team();
            BuildingActor->bPlayerPlaced = true;
            BuildingActor->ForceNetUpdate();
            BuildingActor->InitializeKismetSpawnedBuildingActor(BuildingActor, (AFortPlayerController*)Object);

            auto PC = (AFortPlayerController*)(Object);

            UFortResourceItemDefinition* ResourceDef = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");

            if (BuildingActor->ResourceType == EFortResourceType::Wood)
                ResourceDef = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/WoodItemData.WoodItemData");

            if (BuildingActor->ResourceType == EFortResourceType::Stone)
                ResourceDef = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/StoneItemData.StoneItemData");

            if (BuildingActor->ResourceType == EFortResourceType::Metal)
                ResourceDef = FindObjectFast<UFortResourceItemDefinition>("/Game/Items/ResourcePickups/MetalItemData.MetalItemData");

            if (ResourceDef)
            {
                auto WorldInventory = PC->WorldInventory;

                for (int i = 0; i < WorldInventory->Inventory.ItemInstances.Num(); i++)
                {
                    auto ItemInstance = WorldInventory->Inventory.ItemInstances[i];

                    if (ItemInstance->GetItemDefinitionBP() == ResourceDef)
                    {
                        int newCount = ItemInstance->ItemEntry.Count - 10;

                        WorldInventory->Inventory.ItemInstances.Remove(i);

                        for (int j = 0; j < WorldInventory->Inventory.ReplicatedEntries.Num(); j++)
                        {
                            auto Entry = WorldInventory->Inventory.ReplicatedEntries[j];

                            if (Entry.ItemDefinition == ResourceDef)
                            {
                                WorldInventory->Inventory.ReplicatedEntries.Remove(j);
                            }
                        }

                        if (newCount != 0)
                        {
                            auto NewWorldItem = (UFortWorldItem*)(ResourceDef->CreateTemporaryItemInstanceBP(newCount, 1));

                            WorldInventory->Inventory.ReplicatedEntries.Add(NewWorldItem->ItemEntry);
                            WorldInventory->Inventory.ItemInstances.Add(NewWorldItem);
                        }
                    }
                }

                Abilities::UpdateInventory(PC);
            }
        }
    }
    if (FuncName == "ServerAttemptInteract")
    {

        auto Params = (AFortPlayerController_ServerAttemptInteract_Params*)Parameters;
        auto PC = (AFortPlayerControllerAthena*)Object;

        if (Params->ReceivingActor)
        {
            if (Params->ReceivingActor->IsA(AFortPlayerPawnAthena::StaticClass()))
            {
                auto DBNOPawn = (AFortPlayerPawnAthena*)Params->ReceivingActor;
                auto DBNOPC = (AFortPlayerControllerAthena*)DBNOPawn->Controller;

                if (DBNOPawn && DBNOPC)
                {
                    DBNOPawn->ReviveFromDBNO(PC);
                }
            }
            else if (Params->ReceivingActor->IsA(ABuildingContainer::StaticClass()))
            {
                auto Container = (ABuildingContainer*)Params->ReceivingActor;
                auto ContainerName = Container->GetFullName();

                Container->bAlreadySearched = true;
                Container->OnRep_bAlreadySearched();

                auto Location = Container->K2_GetActorLocation();

                if (strstr(ContainerName.c_str(), "Chest"))
                {

                    auto Weapon = Abilities::GetRandomWeapon();
                    Spawners::SpawnPickup32(Weapon, 1, FVector(Location));
                    auto mat = Abilities::GetRandomMat();
                    Spawners::SpawnPickup32(mat, 30, FVector(Location));
                    auto cons = Abilities::GetRandomConsumable();
                    Spawners::SpawnPickup32(cons, 3, FVector(Location));
              

                }

                if (strstr(ContainerName.c_str(), "Ammo"))
                {
                    auto ammo = Abilities::GetRandomAmmo();
                    Spawners::SpawnPickup32(ammo, 30, FVector(Location));


                }
            }
        }
    }
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
    if (FuncName == "ServerBeginEditingBuildingActor")
    {
        auto PlayerController = (AFortPlayerControllerAthena*)Object;
        bool bFound = false;
        auto Pawn = (AFortPlayerPawnAthena*)PlayerController->Pawn;
        auto Params = (AFortPlayerController_ServerBeginEditingBuildingActor_Params*)Parameters;
        auto EditToolEntry = Abilities::FindItemInInventory<UFortEditToolItemDefinition>(PlayerController, bFound);
        if (PlayerController && Pawn && Params->BuildingActorToEdit && bFound)
        {
            auto EditTool = (AFortWeap_EditingTool*)Abilities::EquipWeaponDefinition(PlayerController, (UFortWeaponItemDefinition*)EditToolEntry.ItemDefinition, EditToolEntry.ItemGuid);

            if (EditTool)
            {
                EditTool->EditActor = Params->BuildingActorToEdit;
                EditTool->OnRep_EditActor();
                Params->BuildingActorToEdit->EditingPlayer = (AFortPlayerStateZone*)PlayerController->PlayerState;
                Params->BuildingActorToEdit->OnRep_EditingPlayer();
                return;

            }
        }
    }


    if (FuncName == "ServerEditBuildingActor")
    {
        auto Params = (AFortPlayerController_ServerEditBuildingActor_Params*)Parameters;
        auto PC = (AFortPlayerControllerAthena*)Object;

        if (PC && Params)
        {
            auto BuildingActor = Params->BuildingActorToEdit;
            auto NewBuildingClass = Params->NewBuildingClass;
            auto RotIterations = Params->RotationIterations;

            if (BuildingActor && NewBuildingClass)
            {
                auto location = BuildingActor->K2_GetActorLocation();
                auto rotation = BuildingActor->K2_GetActorRotation();

                if (BuildingActor->BuildingType != EFortBuildingType::Wall)
                {
                    int Yaw = (int(rotation.Yaw) + 360) % 360;

                    if (Yaw > 80 && Yaw < 100) // 90
                    {
                        if (RotIterations == 1)
                            location = location + FVector(-256, 256, 0);
                        else if (RotIterations == 2)
                            location = location + FVector(-512, 0, 0);
                        else if (RotIterations == 3)
                            location = location + FVector(-256, -256, 0);
                    }
                    else if (Yaw > 170 && Yaw < 190) // 180
                    {
                        if (RotIterations == 1)
                            location = location + FVector(-256, -256, 0);
                        else if (RotIterations == 2)
                            location = location + FVector(0, -512, 0);
                        else if (RotIterations == 3)
                            location = location + FVector(256, -256, 0);
                    }
                    else if (Yaw > 260 && Yaw < 280) // 270
                    {
                        if (RotIterations == 1)
                            location = location + FVector(256, -256, 0);
                        else if (RotIterations == 2)
                            location = location + FVector(512, 0, 0);
                        else if (RotIterations == 3)
                            location = location + FVector(256, 256, 0);
                    }
                    else // 0 - 360
                    {
                        if (RotIterations == 1)
                            location = location + FVector(256, 256, 0);
                        else if (RotIterations == 2)
                            location = location + FVector(0, 512, 0);
                        else if (RotIterations == 3)
                            location = location + FVector(-256, 256, 0);
                    }
                }

                rotation.Yaw += 90 * RotIterations;

                auto bWasInitiallyBuilding = BuildingActor->bIsInitiallyBuilding;
                auto HealthPercent = BuildingActor->GetHealthPercent();
                auto old_Team = BuildingActor->Team;

                BuildingActor->SilentDie();

                if (auto NewBuildingActor = (ABuildingSMActor*)SpawnActorByClass(NewBuildingClass, location, rotation, PC))
                {
                    if (!bWasInitiallyBuilding)
                        NewBuildingActor->ForceBuildingHealth(NewBuildingActor->GetMaxHealth() * HealthPercent);

                    NewBuildingActor->SetMirrored(Params->bMirrored);
                    NewBuildingActor->InitializeKismetSpawnedBuildingActor(NewBuildingActor, PC);
                    NewBuildingActor->Team = old_Team; //set team for build
                    return;
                }
            }
        }
    }
    if (FuncName == "ServerEndEditingBuildingActor")
    {
        auto Params = (AFortPlayerController_ServerEndEditingBuildingActor_Params*)Parameters;
        auto PC = (AFortPlayerControllerAthena*)Object;

        if (!PC->IsInAircraft() && Params->BuildingActorToStopEditing)
        {
            Params->BuildingActorToStopEditing->EditingPlayer = nullptr;
            Params->BuildingActorToStopEditing->OnRep_EditingPlayer();

            auto EditTool = (AFortWeap_EditingTool*)((AFortPlayerPawnAthena*)PC->Pawn)->CurrentWeapon;

            if (EditTool)
            {
                EditTool->bEditConfirmed = true;
                EditTool->EditActor = nullptr;
                EditTool->OnRep_EditActor();
                return;
            }
        }
    }
    if (FuncName == "ServerExecuteInventoryItem")
    {
        auto params = (AFortPlayerController_ServerExecuteInventoryItem_Params*)(Parameters);

        auto PC = (AFortPlayerControllerAthena*)Object;

        if (!PC)
            return oProcessEvent(Object, Function, Parameters);

        if (PC->IsInAircraft())
            return oProcessEvent(Object, Function, Parameters);

        auto WorldInventory = PC->WorldInventory;

        for (int i = 0; i < WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
        {
            auto Entry = WorldInventory->Inventory.ReplicatedEntries[i];

            if (Entry.ItemGuid == params->ItemGuid)
            {
                if (PC->Pawn) {
                    ((AFortPlayerPawn*)PC->Pawn)->EquipWeaponDefinition((UFortWeaponItemDefinition*)(Entry.ItemDefinition), Entry.ItemGuid);
                }
            }
        }
    }
    if (FuncName == "ServerAcknowledgePossession")
        ((APlayerController*)Object)->AcknowledgedPawn = *(APawn**)(Parameters);

    if (FuncName == "ReadyToStartMatch")
    {
        if (!bSetup)
        {
            Globals::Init();

            NewObject<UCheatManager>(GetEngine()->GameInstance->LocalPlayers[0]->PlayerController)->DestroyAll(APlayerController::StaticClass());

            auto GameState = (AFortGameStateAthena*)(GetWorld()->GameState);

            auto Playlist = FindObjectFast<UFortPlaylistAthena>("/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo");

            GameState->CurrentPlaylistData = Playlist;
            GameState->WarmupCountdownStartTime = 420000;
            GameState->OnRep_CurrentPlaylistData();

            GameState->GamePhase = EAthenaGamePhase::None;
            GameState->OnRep_GamePhase(EAthenaGamePhase::None);

            auto GameMode = (AFortGameModeAthena*)GetWorld()->AuthorityGameMode;

            //GameMode->StartMatch();
            //GameMode->StartPlay();

            Listen();

            bSetup = true;
        }
    }
        Abilities::Initialize(Object, Function, Parameters, ObjectName, FunctionName);
out:
        return oProcessEvent(Object, Function, Parameters);
    }

static inline char KickPatch(__int64 a1, __int64 a2)
{
    return 0;
}

static char CrashFix(__int64 a1, __int64 a2)
{
    return 0;
}


void InitMain()
{
    auto BaseAddr = (uintptr_t)GetModuleHandle(0);
    auto setworld = Util::FindPattern("48 89 5C 24 ? 57 48 83 EC 20 48 8B FA 48 8B D9 48 8B 91 ? ? ? ? 48 85 D2 74 28 E8 ? ? ? ? 48");
    auto initlisten = Util::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 50 48 8B BC 24 ? ? ? ? 49 8B F0");
    auto InitHost2 = Util::FindPattern("48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ?");
   
    CREATEHOOK(Util::FindPattern("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 48 8D 05 ? ? ? ? 49 89 73 F0 33 F6 40 38 35 ? ? ? ? 49 89 7B E8 48 8B F9"), TickFlushHook, &Drivers::TickFlush);
    CREATEHOOK(Util::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 49 8B F0 48 8B DA 48 85 D2 0F 84 ? ? ? ? 48 8B BA ? ? ? ? 48 85 FF 74 76 E8 ? ? ? ?"), KickPatch, nullptr);
    CREATEHOOK(BaseAddr + 0x2519130, SetClientLoginStateHook, &SetClientLoginState);
    CREATEHOOK(GetEngine()->Vtable[64], ProcessEventHook, &oProcessEvent);
    Drivers::SetWorld = decltype(Drivers::SetWorld)(setworld);
    Drivers::InitHost = decltype(Drivers::InitHost)(InitHost2);
    Drivers::InitListen = decltype(Drivers::InitListen)(initlisten);
}


