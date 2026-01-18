// GameEntry.cpp

// Include the SDK from Engine DLL
#include "pch.h"
#include <ScriptSDK/ScriptAPI.h>

// Include headers for all scripts you want to register
#include "Scripts/Example_Template.hpp"
#include "Scripts/Interactable_.hpp"

// extern "C" ensures C linkage so the Engine DLL can find this function
extern "C" {
    // Export this function so it can be called from the Engine DLL
    __declspec(dllexport)
        void RegisterEngineScripts(NE::Scripting::IScriptRegistrar* registrar) {

        // Validate that we received a valid registrar
        if (!registrar) {
            return;
        }

        // Register all your game-specific scripts here
        registrar->RegisterScript("Example_Template", []() -> NE::Scripting::IScript* {
            return new Example_Template();
            });
        registrar->RegisterScript("Interactable_", []() -> NE::Scripting::IScript* {
            return new Interactable_();
            });
        }
}
