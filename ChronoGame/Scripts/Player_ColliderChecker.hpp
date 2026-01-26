#pragma once
#include "EngineAPI.hpp"
/*
* By Chan Kuan Fu Ryan (c.kuanfuryan)
* Player_ColliderChecker uses SphereCast to check for collision in its vicinity.
* If there is, the function CheckCollision returns true.
*/

class Player_ColliderChecker : public IScript {
public:
    Player_ColliderChecker() {
        SCRIPT_FIELD(radius, Float);
        SCRIPT_FIELD_LAYERREF(targetLayer);
    }
    ~Player_ColliderChecker() override = default;

    // == Custom Methods ==
    bool CheckCollision()
    {
        //Vec3 origin = TF_GetWorldPosition(GetTransformRef(GetEntity()));
        Vec3 origin = TF_GetPosition(GetTransformRef(GetEntity()));

        //LOG_DEBUG(
        //    "Casting from origin {" 
        //    << origin.x << ", " << origin.y << ", " << origin.z 
        //    << "} with radius of " << radius
        //);

        //NE::Scripting::RaycastHit raycastHit = SphereCast(
        //    origin, 
        //    radius, 
        //    { 0.0f, -1.0f, 0.0f },
        //    1.0f, 
        //    targetLayer.ToMask()
        //);
        //return raycastHit.hasHit;
        return false;
    }

    // === Lifecycle Methods ===
    void Awake() override {}
    void Initialize(Entity entity) override {}
    void Start() override {}
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
    float radius;
    LayerRef targetLayer;
};