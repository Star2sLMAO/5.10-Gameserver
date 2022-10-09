#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

#include "SDK/SDK.hpp"

#include "minhook/minhook.h"

#include "Base.h"
#include "Globals.h"
#include "Utils.h"
#include "UE4.h"
#include "Abilities.h"

void Main()
{
    AllocConsole();
    FILE* pFile;
    freopen_s(&pFile, "CONOUT$", "w", stdout);


    UObject::GObjects = decltype(UObject::GObjects)(Util::FindPattern("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1 EB 03 48 8B ? 81 48 08 ? ? ? 40 49", true, 3));
    FNameToString = decltype(FNameToString)(Util::FindPattern("48 89 5C 24 ? 57 48 83 EC 30 83 79 04 00 48 8B DA 48 8B F9"));
    FMemory_Free = decltype(FMemory_Free)(Util::FindPattern("48 85 C9 74 2E 53 48 83 EC 20 48 8B D9 48 8B 0D ? ? ? ? 48 85 C9 75 0C E8 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B 01 48 8B D3 FF 50 20 48 83 C4 20 5B C3"));
    FMemory_Malloc = decltype(FMemory_Malloc)(Util::FindPattern("48 89 5C 24 ? 57 48 83 EC 20 48 8B F9 8B DA 48 8B 0D ? ? ? ? 48 85 C9"));
    FMemory_Realloc = decltype(FMemory_Realloc)(Util::FindPattern("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B F1 41 8B D8 48 8B 0D ? ? ? ? 48 8B FA 48 85 C9 75 0C"));

    MH_Initialize();

    InitMain();

   Globals::Init();

   Abilities::Init();

   GetGameplayStatics()->GetPlayerController(GetWorld(), 0)->SwitchLevel(L"Athena_Terrain?game=/Game/Athena/Athena_GameMode.Athena_GameMode_C");


    return;

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        Main();

    return TRUE;
}