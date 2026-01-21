#pragma once
#include "../EngineAPI.hpp"
#include <cmath>
#include <algorithm>
#include <ScriptSDK/Math.h>

class Player_Controller_Example : public IScript {
public:
    Player_Controller_Example() = default;
    ~Player_Controller_Example() override = default;

    void Awake() override {}

    void Initialize(Entity entity) override {
        SCRIPT_FIELD(simulatedGravity, Float);
        SCRIPT_FIELD(moveSpeed, Float);
    }

    void Start() override {
        Vec3 r = TF_GetRotation();
        yawDeg = r.y;
    }

    void Update(double deltaTime) override {
        const float stick = -2.0f;

        bool isGrounded = CC_IsGrounded();

        auto [mdx, mdy] = Input::GetMouseDelta();
        yawDeg += static_cast<float>(mdx) * mouseSens;
        pitchDeg -= static_cast<float>(mdy) * mouseSens;

        pitchDeg = std::clamp(pitchDeg, pitchMin, pitchMax);
        
		LOG_INFO("Pitch: " << pitchDeg << " Yaw: " << yawDeg);

        CC_Rotate(yawDeg);

        Vec3 inputDir{ 0,0,0 };
        if (Input::IsKeyDown('W')) inputDir.x += 1.0f;
        if (Input::IsKeyDown('S')) inputDir.x -= 1.0f;
        if (Input::IsKeyDown('A')) inputDir.z -= 1.0f;
        if (Input::IsKeyDown('D')) inputDir.z += 1.0f;

        float mag = std::sqrt(inputDir.x * inputDir.x + inputDir.z * inputDir.z);
        if (mag > 0.01f) { inputDir.x /= mag; inputDir.z /= mag; } else { inputDir.x = 0; inputDir.z = 0; }

        static bool wasJumpKeyDown = false;
        bool isJumpKeyDown = Input::IsKeyDown(' ');
        if (isGrounded && isJumpKeyDown && !wasJumpKeyDown) {
            const float jumpHeight = 1.5f;
            playerVelocity.y = std::sqrt(jumpHeight * -2.0f * simulatedGravity);
            isGrounded = false;
        }
        wasJumpKeyDown = isJumpKeyDown;

        if (isGrounded) {
            if (playerVelocity.y < 0.f) playerVelocity.y = 0.f;
        } else {
            playerVelocity.y += simulatedGravity * (float)deltaTime;
        }
        
        //float yawRad = Math::D(yawDeg);
        //Vec3 forward = Vec3(std::cos(yawRad), 0.0f, std::sin(yawRad));
        //Vec3 right = Vec3(-std::sin(yawRad), 0.0f, std::cos(yawRad));

        // Use your inputDir.x as forward/back and inputDir.z as right/left
        //Vec3 wishDir = forward * inputDir.x + right * inputDir.z;
        //float len = std::sqrt(wishDir.x * wishDir.x + wishDir.z * wishDir.z);
        //if (len > 0.01f) { wishDir.x /= len; wishDir.z /= len; }

        Vec3 horizVel = inputDir * moveSpeed;

        Vec3 finalVel = horizVel;

        if (isGrounded) {
            Vec3 n = CC_GetGroundNormal();
            finalVel = finalVel - n * finalVel.Dot(n);
            finalVel += n * stick;
        } else {
            finalVel.y = playerVelocity.y;
        }

        CC_Move(finalVel * (float)deltaTime);
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
	float simulatedGravity = -9.81f;
    float moveSpeed = 5.0f;
    Vec3 playerVelocity{ 0.0f, 0.0f, 0.0f };

    float mouseSens = 0.12f;
    float yawDeg = 0.0f;
    float pitchDeg = 0.0f;
    float pitchMin = -85.0f;
    float pitchMax = 85.0f;
};