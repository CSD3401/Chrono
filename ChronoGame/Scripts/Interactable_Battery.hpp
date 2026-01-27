#pragma once
#include "EngineAPI.hpp"
#include "Interactable_Grabbable.hpp"

/**
 * Template - Auto-generated script template
 * Implement your game logic in the lifecycle methods below.
 */
class Interactable_Battery : public Interactable_Grabbable {
public:
    Interactable_Battery() {
        // Register any editable fields here
        // Example: SCRIPT_FIELD(speed, float);
        // Example: SCRIPT_FIELD_VECTOR(blingstring, String);;
    }

    ~Interactable_Battery() override = default;

    // === Lifecycle Methods ===

    void Awake() override {
        // Called when the script component is first created
    }

    void Initialize(Entity entity) override {
        // Called to initialize the script with its entity
    }

    void Start() override {
        // Called when the script is enabled and play mode starts
    }

    void Update(double deltaTime) override {
        // Called every frame while the script is enabled
    }

    void OnDestroy() override {
        // Called when the script is about to be destroyed
    }

    // === Optional Callbacks ===

    void OnEnable() override {
        // Called when the script is enabled
    }

    void OnDisable() override {
        // Called when the script is disabled
    }

    void OnValidate() override {
        // Called when a field value is changed in the editor
    }

    const char* GetTypeName() const override {
        return "Interactable_Battery";
    }

    // === Collision Callbacks ===

    void OnCollisionEnter(Entity other) override {
        // Called when this entity starts colliding with another
    }

    void OnCollisionExit(Entity other) override {
        // Called when this entity stops colliding with another
    }

    void OnTriggerEnter(Entity other) override {
        // Called when this entity enters a trigger
    }

    void OnTriggerExit(Entity other) override {
        // Called when this entity exits a trigger
    }

    void Align(Vec3 pos, Vec3 scale, Vec3 rot)
    {
        TransformRef t = this->GetTransformRef(GetEntity());
        SetPosition(t, pos);
        SetScale(t, scale);
        SetRotation(t, rot);
    }

private:
    // Add your private member variables here
    // Example: float speed = 5.0f;

};