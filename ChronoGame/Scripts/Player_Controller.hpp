#pragma once
#include <algorithm>
#include "EngineAPI.hpp"
#include "Player_ColliderChecker.hpp"
#include "Manager_.hpp"
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
        SCRIPT_FIELD(moveSpeed, Float);
        SCRIPT_FIELD(snappiness, Float);
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
        auto m = GameObject::FindObjectsOfType<Manager_>();
        if (m.size() == 0) {
            LOG_ERROR("No managers found!");
        }
        else if (m.size() > 1) {
            LOG_WARNING("Multiple managers found!");
        }
        else {
            manager = m.begin()->GetComponent<Manager_>();
        }
        Reset();
    }
    void Update(double deltaTime) override {
        // === Camera controls ===
        std::pair<double, double> const& mouseDelta = Input::GetMouseDelta();
        cameraY -= static_cast<float>(mouseDelta.second) * cameraSensitivity;
        cameraY = std::clamp(cameraY, -89.0f, 89.0f);
        cameraX += static_cast<float>(mouseDelta.first) * cameraSensitivity;
        SetRotation(-cameraX, 0.0f, 0.0f);
        SetRotation(cameraY, cameraX, 0.0f, playerCameraEntity);

        // === Movement controls ===
        // Read input
        Vec3 inputDirection = Vec3::Zero();
        if (Input::IsKeyDown('W')) { inputDirection.z -= 1.0f; LOG_DEBUG("W"); }
        if (Input::IsKeyDown('S')) { inputDirection.z += 1.0f; LOG_DEBUG("S"); }
        if (Input::IsKeyDown('A')) { inputDirection.x -= 1.0f; LOG_DEBUG("A"); }
        if (Input::IsKeyDown('D')) { inputDirection.x += 1.0f; LOG_DEBUG("D"); }
        inputDirection = inputDirection.Normalized();

        // Velocity
        Vec3 velocity = GetVelocity(); // Must have Rigidbody
        Vec3 targetVelocity
        {
            inputDirection.x * moveSpeed,
            velocity.y,
            inputDirection.z * moveSpeed
        };

        // Lerp velocity
        float t = snappiness * static_cast<float>(deltaTime);
        velocity.x = manager->Lerp(velocity.x, targetVelocity.x, t);
        velocity.y = manager->Lerp(velocity.y, targetVelocity.y, t);

        // Assign
        SetVelocity(targetVelocity);
        
        // Ground Check
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
    // === Manager ===
    Manager_* manager;

    // === Camera ===
	GameObjectRef playerCameraRef;
	Entity playerCameraEntity;
    float cameraX;
    float cameraY;
    float cameraSensitivity;

    // === Movement ===
    GameObjectRef groundCheckRef;
    float moveSpeed;
    float snappiness;
};