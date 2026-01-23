#pragma once
// GameEntry.cpp

// Include the SDK from Engine DLL
#include "pch.h"
#include <ScriptSDK/ScriptAPI.h>

#include "Scripts/Examples/Player_Controller_Example.hpp"

// Include headers for all scripts you want to register
#include "Scripts/Interactable_.hpp"
#include "Scripts/Player_Controller.hpp"
#include "Scripts/Player_ColliderChecker.hpp"
#include "Scripts/Manager_.hpp"
#include "Scripts/Miscellaneous_ICOSwitcher.hpp"
#include "Scripts/Puzzle_TwoStateRotater.hpp"
#include "Scripts/Puzzle_OneWaySwitch.hpp"

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
        registrar->RegisterScript("Interactable_", []() -> NE::Scripting::IScript* {
            return new Interactable_();
            });
        registrar->RegisterScript("Player_Controller", []() -> NE::Scripting::IScript* {
            return new Player_Controller();
            });
        registrar->RegisterScript("Player_ColliderChecker", []() -> NE::Scripting::IScript* {
            return new Player_ColliderChecker();
            });
        registrar->RegisterScript("Manager_", []() -> NE::Scripting::IScript* {
            return new Manager_();
            });

        registrar->RegisterScript("Player_Controller_Example", []() -> NE::Scripting::IScript* {
            return new Player_Controller_Example();
            });

        registrar->RegisterScript("Miscellaneous_ICOSwitcher", []() -> NE::Scripting::IScript* {
            return new Miscellaneous_ICOSwitcher();
            });
        registrar->RegisterScript("Puzzle_TwoStateRotater", []() -> NE::Scripting::IScript* {
            return new Puzzle_TwoStateRotater();
            });
        registrar->RegisterScript("Puzzle_OneWaySwitch", []() -> NE::Scripting::IScript* {
            return new Puzzle_OneWaySwitch();
            });
        }
}
