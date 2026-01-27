#pragma once
#include "EngineAPI.hpp"

/**
 * Template - Auto-generated script template
 * Implement your game logic in the lifecycle methods below.
 */

// Slap this script onto the game object you want to move after something

class Listener_MoveObject : public IScript {
public:
    Listener_MoveObject() {
        // Register any editable fields here
        // Example: SCRIPT_FIELD(speed, float);
        // Example: SCRIPT_FIELD_VECTOR(blingstring, String);;
        SCRIPT_FIELD(startingPos, Vec3);
        SCRIPT_FIELD(targetPos, Vec3);
        SCRIPT_FIELD(tweenDuration, Float);
        SCRIPT_FIELD(listenToMessage, String);
    }

    ~Listener_MoveObject() override = default;

    // === Lifecycle Methods ===

    void Awake() override {
        // Called when the script component is first created
    }

    void Initialize(Entity entity) override {
        // Called to initialize the script with its entity
    }

    void Start() override {
        // Called when the script is enabled and play mode starts
        SetPosition(GetTransformRef(GetEntity()), startingPos);
    }

    void Update(double deltaTime) override {
        // Called every frame while the script is enabled
        if (!destinationReached)
        {
            Vec3 currentPos = GetPosition(GetTransformRef(GetEntity()));
            float dist = (targetPos - currentPos).Length();
            if (dist < 1.0f)
            {
                destinationReached = true;
                isMoving = false;

                Vec3 dummy = startingPos;
                startingPos = targetPos;
                targetPos = dummy;
            }
        }
    }

    void OnDestroy() override {
        // Called when the script is about to be destroyed
    }

    // === Optional Callbacks ===

    void OnEnable() override {
        // Called when the script is enabled
        if (!listenToMessage.empty())
        {
            Events::Listen(listenToMessage.c_str(), [this](void* data) {
                this->MoveObject();
                });
        }
    }

    void OnDisable() override {
        // Called when the script is disabled
    }

    void OnValidate() override {
        // Called when a field value is changed in the editor
    }

    const char* GetTypeName() const override {
        return "Example_Template";
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

    void MoveObject()
    {
        if (isMoving)
            return;

        isMoving = true;
        destinationReached = false;

        Tweener::StartVec3(
            [this](const Vec3& pos) {
                SetPosition(GetTransformRef(GetEntity()), pos);
            },
            startingPos,
            targetPos,
            tweenDuration,
            Tweener::Type::CUBIC_EASE_IN,
            GetEntity()
        );
    }

private:
    // Add your private member variables here
    // Example: float speed = 5.0f;
    Vec3 startingPos;
    Vec3 targetPos;
    bool isMoving = false;
    bool destinationReached = false;
    float tweenDuration = 1.5f;

    std::string listenToMessage;
};