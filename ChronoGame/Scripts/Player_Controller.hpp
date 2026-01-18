#pragma once
#include <algorithm>
#include "EngineAPI.hpp"
#include "Player_ColliderChecker.hpp"
/*
* By Chan Kuan Fu Ryan (c.kuanfuryan)
* Player_Controller is script which handles player input and movement.
* It also handles camera controls.
*/

class Player_Controller : public IScript {
public:
    Player_Controller() {
		SCRIPT_GAMEOBJECT_REF(playerCameraRef);
        SCRIPT_FIELD(cameraSensitivity, Float);
        SCRIPT_GAMEOBJECT_REF(groundCheckRef);
    }
    ~Player_Controller() override = default;

    // == Custom Methods ==
    void Reset()
    {
        cameraY = 0.0f;
        cameraX = 0.0f;
    }

    // === Lifecycle Methods ===
    void Awake() override {}
    void Initialize(Entity entity) override {}
    void Start() override {
		// Get player camera game object
        if (playerCameraRef.IsValid()) {
			playerCameraEntity = playerCameraRef.GetEntity();
        }
        Reset();
    }
    void Update(double deltaTime) override {
        // Camera controls
        std::pair<double, double> const& mouseDelta = Input::GetMouseDelta();
        cameraY -= static_cast<float>(mouseDelta.second) * cameraSensitivity;
        cameraY = std::clamp(cameraY, -89.0f, 89.0f);
        cameraX += static_cast<float>(mouseDelta.first) * cameraSensitivity;
        SetRotation(-cameraX, 0.0f, 0.0f);
        SetRotation(cameraY, cameraX, 0.0f, playerCameraEntity);

        // Movement controls
        if (GameObject(groundCheckRef).GetComponent<Player_ColliderChecker>()->IsColliding())
        {
            // LOG_DEBUG("GROUNDED");
        }
        else
        {
            // LOG_DEBUG("NOPE");
        }
    }
    void OnDestroy() override {}

    // === Optional Callbacks ===
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}
    const char* GetTypeName() const override { return "Player_Controller"; }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override {}
    void OnCollisionExit(Entity other) override {}
    void OnTriggerEnter(Entity other) override {}
    void OnTriggerExit(Entity other) override {}

private:
    // === Camera ===
	GameObjectRef playerCameraRef;
	Entity playerCameraEntity;
    float cameraX;
    float cameraY;
    float cameraSensitivity;

    // === Ground Check ===
    GameObjectRef groundCheckRef;

};