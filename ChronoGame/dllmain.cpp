// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <ScriptSDK/ScriptAPI.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        // Restore host/display default gamma when editor stops play or game exits.
        // Last slider value remains in SavedSettings statics until the process ends.
        NE::Scripting::SetGamma(1.0f);
        break;
    }
    return TRUE;
}

