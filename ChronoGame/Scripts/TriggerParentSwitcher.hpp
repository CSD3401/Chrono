#pragma once
#include "EngineAPI.hpp"

/**
 * TriggerParentSwitcher
 * ----------------------
 * Attach this script to a trigger-box entity (one with a trigger collider).
 * When something enters the trigger:
 *   - deactivates `inactiveParent`
 *   - activates `activeParent`
 *
 * Optionally, assign `playerRef` to only trigger when the player enters.
 */
class TriggerParentSwitcher : public IScript {
public:
    TriggerParentSwitcher() {
        SCRIPT_GAMEOBJECT_REF(inactiveParent);
        SCRIPT_GAMEOBJECT_REF(activeParent);
        SCRIPT_GAMEOBJECT_REF(playerRef);
        SCRIPT_FIELD(oneShot, Bool);
    }

    ~TriggerParentSwitcher() override = default;

    void Awake() override {}
    void Initialize(Entity /*entity*/) override {}

    void Start() override {
        m_switched = false;

        if (!inactiveParent.IsValid() || !activeParent.IsValid()) {
            LOG_WARNING("TriggerParentSwitcher: inactiveParent or activeParent is not assigned");
        }
    }

    void Update(double /*dt*/) override {}

    void OnTriggerEnter(Entity other) override {
        if (oneShot && m_switched) return;

        if (!inactiveParent.IsValid() || !activeParent.IsValid()) return;

        // If a playerRef is assigned, only react when that entity enters.
        if (playerRef.IsValid() && other != playerRef.GetEntity()) return;

        SetActive(false, inactiveParent.GetEntity());
        SetActive(true, activeParent.GetEntity());

        m_switched = true;
    }

    void OnTriggerExit(Entity /*other*/) override {}
    void OnTriggerStay(Entity /*other*/) override {}

    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}

    const char* GetTypeName() const override { return "TriggerParentSwitcher"; }

    void OnCollisionEnter(Entity /*other*/) override {}
    void OnCollisionExit(Entity /*other*/) override {}
    void OnCollisionStay(Entity /*other*/) override {}

private:
    GameObjectRef inactiveParent;
    GameObjectRef activeParent;
    GameObjectRef playerRef;

    bool oneShot = true;
    bool m_switched = false;
};

