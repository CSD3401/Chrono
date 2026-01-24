#pragma once
#include "../EngineAPI.hpp"
#include <cmath>
#include <algorithm>
#include <ScriptSDK/Math.h>

class Player_Camera_Example : public IScript {
public:
    Player_Camera_Example() = default;
    ~Player_Camera_Example() override = default;

    void Awake() override {}

    void Initialize(Entity entity) override {
        SCRIPT_FIELD_LAYERREF(raycastLayer);
    }

    void Start() override {

    }

    void Update(double deltaTime) override {

    }

    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}

    const char* GetTypeName() const override {
        return "Player_Controller_Example";
    }

    void OnCollisionEnter(Entity other) override {}
    void OnCollisionExit(Entity other) override {}
    void OnTriggerEnter(Entity other) override {}
    void OnTriggerExit(Entity other) override {}

private:
    GameObjectRef playerCameraRef;
    float simulatedGravity = -9.81f;
    float moveSpeed = 5.0f;
    Vec3 playerVelocity{ 0.0f, 0.0f, 0.0f };
    LayerRef raycastLayer;

    float jumpHeight = 1.5f;
    float groundStickForce = -2.0f;
    float mouseSens = 0.12f;
    float yawDeg = 0.0f;
    float pitchDeg = 0.0f;
    float pitchMin = -85.0f;
    float pitchMax = 85.0f;
};