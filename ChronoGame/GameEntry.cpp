#pragma once
// GameEntry.cpp

// Include the SDK from Engine DLL
#include "pch.h"
#include <ScriptSDK/ScriptAPI.h>

// Include headers for all scripts you want to register
#include "Scripts/Interactable_.hpp"
#include "Scripts/Player_Controller.hpp"
#include "Scripts/Player_ColliderChecker.hpp"
#include "Scripts/Manager_.hpp"
#include "Scripts/Miscellaneous_ICOSwitcher.hpp"
#include "Scripts/Puzzle_TwoStateRotater.hpp"
#include "Scripts/Puzzle_OneWaySwitch.hpp"
#include "Scripts/Puzzle_Sinkhole.hpp"
#include "Scripts/Puzzle_Sinkhole_TestTrigger.hpp"
#include "Scripts/Puzzle_OneWaySwitch_TestTrigger.hpp"
#include "Scripts/Puzzle_TwoStateRotater_TestTrigger.hpp"

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
        registrar->RegisterScript("Miscellaneous_ICOSwitcher", []() -> NE::Scripting::IScript* {
            return new Miscellaneous_ICOSwitcher();
            });
        registrar->RegisterScript("Puzzle_TwoStateRotater", []() -> NE::Scripting::IScript* {
            return new Puzzle_TwoStateRotater();
            });
        registrar->RegisterScript("Puzzle_OneWaySwitch", []() -> NE::Scripting::IScript* {
            return new Puzzle_OneWaySwitch();
            });
        registrar->RegisterScript("Puzzle_Sinkhole", []() -> NE::Scripting::IScript* {
            return new Puzzle_Sinkhole();
            });
        registrar->RegisterScript("Puzzle_Sinkhole_TestTrigger", []() -> NE::Scripting::IScript* {
            return new Puzzle_Sinkhole_TestTrigger();
            });
        registrar->RegisterScript("Puzzle_TwoStateRotater_TestTrigger", []() -> NE::Scripting::IScript* {
            return new Puzzle_TwoStateRotater_TestTrigger();
            });
        registrar->RegisterScript("Puzzle_OneWaySwitch_TestTrigger", []() -> NE::Scripting::IScript* {
            return new Puzzle_OneWaySwitch_TestTrigger();
            });
        }
}
