#pragma once
#include "EngineAPI.hpp"

/**
 * Interactable_DoorHinge
 *
 * Rotates door around hinge (Y-axis rotation) when player presses E nearby.
 * Attach this to the door HINGE parent entity, not the door mesh itself.
 *
 * Setup:
 * 1. Add this script to the door hinge parent entity
 * 2. Assign playerRef (drag Player entity)
 * 3. Set targetRotationY (e.g., -100 to rotate 100 degrees)
 * 4. Press E when near door to open it
 */
class Interactable_DoorHinge : public IScript {
public:
    Interactable_DoorHinge() {
        SCRIPT_GAMEOBJECT_REF(playerRef);
        SCRIPT_FIELD(interactionDistance, Float);
        SCRIPT_FIELD(targetRotationY, Float);
        SCRIPT_FIELD(tweenDuration, Float);
        SCRIPT_FIELD(logInteractions, Bool);
    }

    ~Interactable_DoorHinge() override = default;

    void Awake() override {}
    void Initialize(Entity entity) override {}

    void Start() override {
        hingeEntity = GetEntity();
        Vec3 rot = TF_GetLocalRotation(hingeEntity);
        startingRotation = rot;

        if (!playerRef.IsValid()) {
            LOG_ERROR("Interactable_DoorHinge: playerRef not assigned!");
        }

        if (interactionDistance <= 0.0f) {
            LOG_WARNING("Interactable_DoorHinge: interactionDistance must be > 0, setting to 5.0");
            interactionDistance = 5.0f;
        }

        if (tweenDuration <= 0.0f) {
            LOG_WARNING("Interactable_DoorHinge: tweenDuration must be > 0, setting to 1.5");
            tweenDuration = 1.5f;
        }

        LOG_DEBUG("Interactable_DoorHinge initialized - StartRot: ({}, {}, {}), TargetRotY: {}",
            startingRotation.x, startingRotation.y, startingRotation.z, targetRotationY);
    }

    void Update(double deltaTime) override {
        if (!playerRef.IsValid() || isRotating) return;

        // Get player entity and positions
        Entity player = playerRef.GetEntity();
        Vec3 playerPos = GetPosition(GetTransformRef(player));
        Vec3 hingePos = GetPosition(GetTransformRef(hingeEntity));

        // Calculate distance
        Vec3 delta = playerPos - hingePos;
        float distance = delta.Length();

        // Check if player is in range and presses E
        if (distance <= interactionDistance) {
            if (Input::WasKeyPressed('E')) {
                OpenDoor();
            }
        }
    }

    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}
    const char* GetTypeName() const override { return "Interactable_DoorHinge"; }

    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }

private:
    void OpenDoor() {
        isRotating = true;

        // Target rotation: only Y changes
        Vec3 targetRotation = startingRotation;
        targetRotation.y = targetRotationY;

        // Tween rotation using TF_SetRotation
        Tweener::StartVec3(
            [this](const Vec3& rot) {
                TF_SetRotation(rot, hingeEntity);
            },
            startingRotation,
            targetRotation,
            tweenDuration,
            Tweener::Type::CUBIC_EASE_BOTH,
            hingeEntity
        );

        if (logInteractions) {
            LOG_DEBUG("Door opening! Rotating from Y={} to Y={}", startingRotation.y, targetRotationY);
        }
    }

    // Exposed fields
    GameObjectRef playerRef;
    float interactionDistance = 5.0f;
    float targetRotationY = -100.0f;  // Target Y rotation in degrees
    float tweenDuration = 2.0f;
    bool logInteractions = true;

    // Runtime
    Entity hingeEntity = 0;
    Vec3 startingRotation;
    bool isRotating = false;
};