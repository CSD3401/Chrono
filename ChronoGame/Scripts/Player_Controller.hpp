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
        cameraRotation = Vec3::Zero();
    }

    // === Lifecycle Methods ===
    void Awake() override {}
    void Initialize(Entity entity) override {}
    void Start() override {
		// Get player camera game object
        if (playerCameraRef.IsValid()) {
			playerCameraTransformRef = GetTransformRef(playerCameraRef.GetEntity());
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
        transformRef = GetTransformRef(GetEntity());
        rigidbodyRef = GetRigidbodyRef(GetEntity());
        Reset();
    }
    void Update(double deltaTime) override {
        // === Camera controls ===
        std::pair<double, double> const& mouseDelta = Input::GetMouseDelta();
        cameraRotation.y -= static_cast<float>(mouseDelta.second) * cameraSensitivity;
        cameraRotation.y = std::clamp(cameraRotation.y, -89.0f, 89.0f);
        cameraRotation.x += static_cast<float>(mouseDelta.first) * cameraSensitivity;
        SetRotation(transformRef, {-cameraRotation.x, 0.0f, 0.0f });
        SetRotation(playerCameraTransformRef, { cameraRotation.y, cameraRotation.x, 0.0f });

        // === Movement controls ===
        // Input Direction
        Vec3 inputDirection = Vec3::Zero();
        if (Input::IsKeyDown('W')) { inputDirection.z -= 1.0f; }
        if (Input::IsKeyDown('S')) { inputDirection.z += 1.0f; }
        if (Input::IsKeyDown('A')) { inputDirection.x -= 1.0f; }
        if (Input::IsKeyDown('D')) { inputDirection.x += 1.0f; }
        inputDirection = inputDirection.Normalized();

        // Camera-relative direction
        Vec3 cameraForward = TF_GetForward(playerCameraRef.GetEntity());
        Vec3 cameraRight = TF_GetRight(playerCameraRef.GetEntity());
        cameraForward.y = 0.0f;
        cameraRight.y = 0.0f;
        cameraForward = cameraForward.Normalized();
        cameraRight = cameraRight.Normalized();

        // Movement direction
        Vec3 moveDirection = cameraRight * inputDirection.x + cameraForward * inputDirection.z;
        moveDirection = moveDirection.Normalized();

        // Velocity
        Vec3 velocity = GetVelocity(rigidbodyRef); // Must have Rigidbody
        Vec3 targetVelocity
        {
            moveDirection.x * moveSpeed,
            velocity.y,
            moveDirection.z * moveSpeed
        };
        
        // Lerp velocity
        float t = snappiness * static_cast<float>(deltaTime);
        velocity.x = manager->Lerp(velocity.x, targetVelocity.x, t);
        velocity.z = manager->Lerp(velocity.z, targetVelocity.z, t);

        // Assign
        SetVelocity(rigidbodyRef, velocity);
        
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
    TransformRef transformRef;
    RigidbodyRef rigidbodyRef;

    // === Camera ===
	GameObjectRef playerCameraRef;
    TransformRef playerCameraTransformRef;
    Vec3 cameraRotation;
    float cameraSensitivity;

    // === Movement ===
    GameObjectRef groundCheckRef;
    float moveSpeed;
    float snappiness;
};