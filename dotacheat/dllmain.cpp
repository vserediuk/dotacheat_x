#include "pch.h"

inline uintptr_t GetModuleBaseAddress(const char* moduleName) {
    return reinterpret_cast<uintptr_t>(GetModuleHandleA(moduleName));
}

inline uintptr_t GetExportAddress(uintptr_t moduleBase, const char* exportName) {
    return reinterpret_cast<uintptr_t>(GetProcAddress((HMODULE)(moduleBase), exportName));
}

inline uintptr_t GetAbsoluteAddress(uintptr_t instruction_ptr, int offset, int size)
{
    return instruction_ptr + *reinterpret_cast<int32_t*>(instruction_ptr + offset) + size;
}


class CInterfaceRegister
{
public:
    void* (__cdecl* m_pfnCreate)();
    const char* m_pszName;
    CInterfaceRegister* m_pNext;
};

typedef CBaseEntity* (__fastcall* OnAddRemoveEntity_t)(CGameEntitySystem*, CBaseEntity*);
OnAddRemoveEntity_t oOnAddEntity = nullptr;
OnAddRemoveEntity_t oOnRemoveEntity = nullptr;

CBaseEntity* __fastcall hkOnAddEntity(CGameEntitySystem* pThis, CBaseEntity* pEntity)
{
    auto binaryName = pEntity->Schema_DynamicBinding(   )->binaryName;
	if (strstr(binaryName,"C_DOTA_BaseNPC_Hero"))
	{
		auto player = reinterpret_cast<CDotaBaseNPC*>(pEntity);
        if (player != nullptr) {
			auto isIllusion = player->IsIllusion();
            auto a = "hui";
        }
	}
    return oOnAddEntity(pThis, pEntity);
}


CBaseEntity* __fastcall hkOnRemoveEntity(CGameEntitySystem* pThis, CBaseEntity* pEntity)
{
    return oOnRemoveEntity(pThis, pEntity);
}

void install_hooks(HMODULE hModule) {
    DisableThreadLibraryCalls(hModule);
    if (MH_Initialize() != MH_OK) {
        return;
    }

    auto handle = GetModuleHandle(L"client.dll");
    if (handle == nullptr) {
        return;
    }

    MODULEINFO modInfo;
    if (!GetModuleInformation(GetCurrentProcess(), handle, &modInfo, sizeof(MODULEINFO))) {
        return;
    }

    const void* CGame_EntitySystem = Sig::find(handle, modInfo.SizeOfImage, signatures::ReinitPredictables_CGame_EntitySystem.c_str());
    if (CGame_EntitySystem == nullptr) {
        return;
    }

    // CPrediction::ReinitPredictables()
    // Look for string "ReinitPredictables".
    // The start of this function contains both a pointer to CGameEntitySystem and a call to CGameEntitySystem::GetHighestEntityIndex();
    // 41 C6 86 68 01 00 00 01                 mov     byte ptr[r14 + 168h], 1
    // 48 8D 54 24 60                          lea     rdx, [rsp + 58h + arg_0]
    // 48 8B 0D 31 90 B3 03                    mov     rcx, cs:qword_184FBFB10
    // E8 3C 1A E9 FF                          call    GetHighestEntityIndex

    CGameEntitySystem* entitySystem = *reinterpret_cast<CGameEntitySystem**>(GetAbsoluteAddress(reinterpret_cast<uintptr_t>(const_cast<void*>(CGame_EntitySystem)) + 13, 3, 7));

    uintptr_t getHighestEntIndexFnAddr = GetAbsoluteAddress(reinterpret_cast<uintptr_t>(const_cast<void*>(CGame_EntitySystem)) + 20, 1, 5);

    // 8B 81 00 21 00 00                       mov     eax, [rcx+2100h]
    // 89 02                                   mov[rdx], eax
    // 48 8B C2                                mov     rax, rdx
    // C3                                      retn
    highestEntIndexOffset = *reinterpret_cast<uint32_t*>(getHighestEntIndexFnAddr + 2);

    uintptr_t** vtable = *reinterpret_cast<uintptr_t***>(entitySystem);

    auto funcRem = vtable[16];

    auto funcAdd = vtable[15];

    if (MH_CreateHook(funcAdd, &hkOnAddEntity, reinterpret_cast<LPVOID*>(&oOnAddEntity)) != MH_OK) {
        return;
    }

	if (MH_EnableHook(funcAdd) != MH_OK) {
		return;
	}

    if (MH_CreateHook(funcRem, &hkOnRemoveEntity, reinterpret_cast<LPVOID*>(&oOnRemoveEntity)) != MH_OK) {
        return;
    }

    if (MH_EnableHook(funcRem) != MH_OK) {
        return;
    }

    uintptr_t moduleBase = GetModuleBaseAddress("client.dll");
    uintptr_t pfnCreateInterface = GetExportAddress(moduleBase, "CreateInterface");

    //.text:0000000182DD2390; __int64 __fastcall CreateInterface(__int64, _DWORD*)
    //    .text:0000000182DD2390                                         public CreateInterface
    //    .text:0000000182DD2390                         CreateInterface proc near; DATA XREF : .rdata : off_184914778↓o
    //    .text : 0000000182DD2390 4C 8B 0D B1 CE 45 02                    mov     r9, cs:qword_18522F248
    //    .text : 0000000182DD2397 4C 8B D2                                mov     r10, rdx
    //    .text:0000000182DD239A 4C 8B D9                                mov     r11, rcx
    //    .text:0000000182DD239D 4D 85 C9                                test    r9, r9
    //    .text:0000000182DD23A0 74 2E                                   jz      short loc_182DD23D0

    CInterfaceRegister* pRegister = *reinterpret_cast<CInterfaceRegister**>(GetAbsoluteAddress(pfnCreateInterface, 3, 7));

    CSource2Client* client = nullptr;

    while (pRegister != nullptr) {
        if (pRegister->m_pszName != nullptr && strstr(pRegister->m_pszName, "Source2Client002")) {
            auto pfnCreate = pRegister->m_pfnCreate;
            if (pfnCreate != nullptr) {  
                client = reinterpret_cast<CSource2Client*>(pfnCreate());
                if (client != nullptr) {
                    break;
                }
            }
        }
        pRegister = pRegister->m_pNext;
    }

    if (client) {
        Netvars::CacheNetvars(client);
    }
    else {
        printf("Failed to find Source2Client002\n");
    }

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        std::thread(install_hooks, hModule).detach();

    return TRUE;
}

