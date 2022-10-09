#pragma once
#include "Abilities.h"
namespace Pawn
{

    inline AFortPlayerPawnAthena* InitializePawn(AFortPlayerControllerAthena* PlayerController, FVector Location)
    {
        if (PlayerController->Pawn)
            PlayerController->Pawn->K2_DestroyActor();

        auto Pawn = Spawners::SpawnActor<AFortPlayerPawnAthena>(Location, {}, Globals::PawnClass);

        Pawn->Owner = PlayerController;
        Pawn->OnRep_Owner();

        Pawn->Controller = PlayerController;
        Pawn->OnRep_Controller();

        Pawn->PlayerState = PlayerController->PlayerState;
        Pawn->OnRep_PlayerState();

        Pawn->bAlwaysRelevant = true;

        PlayerController->Pawn = Pawn;
        PlayerController->AcknowledgedPawn = Pawn;
        PlayerController->Character = Pawn;
        PlayerController->OnRep_Pawn();
        PlayerController->Possess(Pawn);

        Pawn->SetMaxHealth(100);
        Pawn->SetMaxShield(100);

        Pawn->SetHealth(100);

        Pawn->bCanBeDamaged = false;


        return Pawn;
    }
}
  
    static inline void Listen()
    {
        auto BaseAddr = (uintptr_t)GetModuleHandle(0);

        auto Beacon = Spawners::SpawnActor<AOnlineBeaconHost>();
        Beacon->ListenPort = 6969;
        Drivers::InitHost(Beacon);

        Beacon->NetDriverName = FName(282);
        Beacon->NetDriver->NetDriverName = FName(282);
        Beacon->NetDriver->World = GetWorld();

        FURL Url;
        Url.Port = 7777;

        FString Error;

        Drivers::InitListen((UIpNetDriver*)Beacon->NetDriver, GetWorld(), Url, false, Error);

        GetWorld()->NetDriver = Beacon->NetDriver;
        Drivers::SetWorld(Beacon->NetDriver, GetWorld());

        GetWorld()->LevelCollections[0].NetDriver = Beacon->NetDriver;
        GetWorld()->LevelCollections[1].NetDriver = Beacon->NetDriver;

        Drivers::ServerReplicateActors = decltype(Drivers::ServerReplicateActors)(Beacon->NetDriver->ReplicationDriver->Vtable[0x54]);

    }
