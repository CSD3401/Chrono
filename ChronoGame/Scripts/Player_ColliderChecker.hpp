#pragma once
#include "EngineAPI.hpp"
/*
* By Chan Kuan Fu Ryan (c.kuanfuryan)
* Player_ColliderChecker checks if this object is colliding with something.
* If it is, the function IsColliding returns true.
*/

class Player_ColliderChecker : public IScript {
public:
    Player_ColliderChecker() {}
    ~Player_ColliderChecker() override = default;

    // == Custom Methods ==
    bool IsColliding()
    {
        return isColliding;
    }

    // === Lifecycle Methods ===
    void Awake() override {}
    void Initialize(Entity entity) override {}
    void Start() override {
        isColliding = false;
    }
    void Update(double deltaTime) override {}
    void OnDestroy() override {}

    // === Optional Callbacks ===
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}
    const char* GetTypeName() const override { return "Player_ColliderChecker"; }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override {}
    void OnCollisionExit(Entity other) override {}
    void OnTriggerEnter(Entity other) override {}
    void OnTriggerExit(Entity other) override {}

private:
    bool isColliding;
};