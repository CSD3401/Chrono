#pragma once
#include "EngineAPI.hpp"

/**
 * TimeWallController
 *
 * Controls a wall/barrier that exists in one time period but not another.
 * Listens to time shift events to enable/disable the collider.
 *
 * Setup:
 * 1. Add this script to your wall entity
 * 2. Assign wallObject (the GameObject with the collider)
 * 3. Wall will be disabled when ChronoActivated, enabled when ChronoDeactivated
 */
class TimeWallController : public IScript {
public:
    TimeWallController() {
        SCRIPT_GAMEOBJECT_REF(wallObject);
    }

    ~TimeWallController() override = default;

    void Awake() override {}
    void Initialize(Entity entity) override {}

    void Start() override {
        // Listen for chrono watch events
        Events::Listen("ChronoActivated", [this](void* data) {
            (void)data;
            DisableWall();
        });

        Events::Listen("ChronoDeactivated", [this](void* data) {
            (void)data;
            EnableWall();
        });

        LOG_DEBUG("TimeWallController listening to Chrono events");
    }

    void Update(double deltaTime) override {
        (void)deltaTime;
    }

    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}
    const char* GetTypeName() const override { return "TimeWallController"; }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }

private:
    void DisableWall() {
        if (wallObject.IsValid()) {
            Entity wallEntity = wallObject.GetEntity();
            RB_SetIsTrigger(true, wallEntity);
            LOG_DEBUG("TimeWall: Disabled (ChronoActivated)");
        }
    }

    void EnableWall() {
        if (wallObject.IsValid()) {
            Entity wallEntity = wallObject.GetEntity();
            RB_SetIsTrigger(false, wallEntity);
            LOG_DEBUG("TimeWall: Enabled (ChronoDeactivated)");
        }
    }

    GameObjectRef wallObject;
};
