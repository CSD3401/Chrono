#pragma once
#include "EngineAPI.hpp"
#include "Watch_Controller.hpp"

/*
* Misc_ForcePastOnCollision
* When the player collides or triggers this object, the watch is locked into
* the past forever:
*   - switches to past immediately (if not already there)
*   - stops the drain / auto-return timer
*   - ignores manual Q toggles back to present
*
* Setup:
* 1. Attach this script to the collision/trigger object.
* 2. Assign playerRef to the player entity.
* 3. Assign watchRef to the entity that has Watch_Controller.
*/
class Misc_ForcePastOnCollision : public IScript {
public:
    Misc_ForcePastOnCollision() {
        SCRIPT_GAMEOBJECT_REF(playerRef);
        SCRIPT_GAMEOBJECT_REF(watchRef);
        SCRIPT_FIELD(disableAfterTrigger, Bool);
        SCRIPT_FIELD(refillClockToFull, Bool);
    }

    ~Misc_ForcePastOnCollision() override = default;

    void Awake() override {
        CacheWatch();
    }

    void Initialize(Entity entity) override {
        (void)entity;
    }

    void Start() override {
        CacheWatch();
    }

    void Update(double deltaTime) override {
        (void)deltaTime;
    }

    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {
        CacheWatch();
    }

    const char* GetTypeName() const override {
        return "Misc_ForcePastOnCollision";
    }

    void OnCollisionEnter(Entity other) override {
        HandlePlayerEnter(other);
    }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }

    void OnTriggerEnter(Entity other) override {
        HandlePlayerEnter(other);
    }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }

private:
    GameObjectRef playerRef;
    GameObjectRef watchRef;
    Watch_Controller* watch = nullptr;
    bool disableAfterTrigger = true;
    bool refillClockToFull = true;
    bool hasTriggered = false;

    void CacheWatch() {
        if (!watchRef.IsValid()) {
            watch = nullptr;
            return;
        }

        watch = GameObject(watchRef).GetComponent<Watch_Controller>();
        if (!watch) {
            LOG_WARNING("Misc_ForcePastOnCollision: watchRef has no Watch_Controller");
        }
    }

    void HandlePlayerEnter(Entity other) {
        if (hasTriggered) {
            return;
        }

        if (!IsActiveInHierarchy()) {
            return;
        }

        if (!playerRef.IsValid()) {
            LOG_WARNING("Misc_ForcePastOnCollision: missing playerRef");
            return;
        }

        if (other != playerRef.GetEntity()) {
            return;
        }

        if (!watch) {
            CacheWatch();
        }

        if (!watch) {
            LOG_WARNING("Misc_ForcePastOnCollision: Watch_Controller not found");
            return;
        }

        watch->ForcePastForever(refillClockToFull);
        hasTriggered = true;

        LOG_INFO("Misc_ForcePastOnCollision: Player entered trigger - forcing permanent past state");

        if (disableAfterTrigger) {
            SetActive(false, GetEntity());
        }
    }
};
