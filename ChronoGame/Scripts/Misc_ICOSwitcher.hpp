#pragma once
#include "EngineAPI.hpp"
/*
* Miscellaneous_ICOSwitcher:
* - Listens for ChronoActivated/ChronoDeactivated events
* - Toggles two referenced GameObjects (idle vs running)
* - Destroys itself if references are invalid
*/

class Misc_ICOSwitcher : public IScript {
public:
    Misc_ICOSwitcher() {
        SCRIPT_GAMEOBJECT_REF(objectsIdle);
        SCRIPT_GAMEOBJECT_REF(objectsRunning);
    }

    ~Misc_ICOSwitcher() override = default;

    // === Lifecycle Methods ===
    void Awake() override {
        RegisterEventListeners();
        LOG_INFO("Misc_ICOSwitcher: listeners registered");
    }

    void Initialize(Entity entity) override {}
    void Start() override {
        // Addition: Ensure a consistent initial state at play start (running on, idle off).
        Activate();
    }
    void Update(double deltaTime) override {}

    void OnDestroy() override {
        listeningEnabled = false;
    }

    // === Optional Callbacks ===
    void OnEnable() override {
        listeningEnabled = true;
        LOG_INFO("Misc_ICOSwitcher: enabled");
    }

    void OnDisable() override {
        listeningEnabled = false;
        LOG_INFO("Misc_ICOSwitcher: disabled");
    }

    void OnValidate() override {}

    const char* GetTypeName() const override {
        return "Misc_ICOSwitcher";
    }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override {}
    void OnCollisionExit(Entity other) override {}
    void OnTriggerEnter(Entity other) override {}
    void OnTriggerExit(Entity other) override {}

private:
    GameObjectRef objectsIdle;
    GameObjectRef objectsRunning;
    bool eventsRegistered = false;
    bool listeningEnabled = false;

    void RegisterEventListeners() {
        if (eventsRegistered) {
            return;
        }

        Events::Listen("ChronoActivated", [this](void*) {
            if (!listeningEnabled) {
                LOG_INFO("Misc_ICOSwitcher: ChronoActivated ignored (disabled)");
                return;
            }
            Activate();
        });

        Events::Listen("ChronoDeactivated", [this](void*) {
            if (!listeningEnabled) {
                LOG_INFO("Misc_ICOSwitcher: ChronoDeactivated ignored (disabled)");
                return;
            }
            Deactivate();
        });

        eventsRegistered = true;
    }

    bool CheckObjectsValid() const {
        return objectsIdle.IsValid() && objectsRunning.IsValid();
    }

    void Activate() {
        if (CheckObjectsValid()) {
            LOG_INFO("Misc_ICOSwitcher: ChronoActivated -> idle off, running on");
            SetActive(false, objectsIdle.GetEntity());
            SetActive(true, objectsRunning.GetEntity());
        } else {
            LOG_WARNING("Misc_ICOSwitcher: Invalid references on activate, destroying");
            Command::DestroyEntity(GetEntity());
        }
    }

    void Deactivate() {
        if (CheckObjectsValid()) {
            LOG_INFO("Misc_ICOSwitcher: ChronoDeactivated -> idle on, running off");
            SetActive(true, objectsIdle.GetEntity());
            SetActive(false, objectsRunning.GetEntity());
        } else {
            LOG_WARNING("Misc_ICOSwitcher: Invalid references on deactivate, destroying");
            Command::DestroyEntity(GetEntity());
        }
    }
};
