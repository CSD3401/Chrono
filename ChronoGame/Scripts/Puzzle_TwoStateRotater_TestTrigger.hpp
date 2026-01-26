#pragma once
#include "EngineAPI.hpp"
#include "Puzzle_TwoStateRotater.hpp"

/*
* Puzzle_TwoStateRotater_TestTrigger
* Press a key to toggle a Puzzle_TwoStateRotater on a target entity.
*/

class Puzzle_TwoStateRotater_TestTrigger : public IScript {
public:
    Puzzle_TwoStateRotater_TestTrigger() {
        SCRIPT_GAMEOBJECT_REF(targetRotater);
        SCRIPT_FIELD(toggleKey, Int);
    }

    ~Puzzle_TwoStateRotater_TestTrigger() override = default;

    // === Lifecycle Methods ===
    void Awake() override {}
    void Initialize(Entity entity) override {}
    void Start() override {}

    void Update(double deltaTime) override {
        if (!Input::WasKeyPressed(toggleKey)) {
            return;
        }

        if (!targetRotater.IsValid()) {
            LOG_WARNING("Puzzle_TwoStateRotater_TestTrigger: targetRotater not set");
            return;
        }

        auto* rotater = GameObject(targetRotater).GetComponent<Puzzle_TwoStateRotater>();
        if (!rotater) {
            LOG_WARNING("Puzzle_TwoStateRotater_TestTrigger: missing Puzzle_TwoStateRotater on target");
            return;
        }

        rotater->SwitchState();
        LOG_INFO("Puzzle_TwoStateRotater_TestTrigger: SwitchState invoked");
    }

    void OnDestroy() override {}

    // === Optional Callbacks ===
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}

    const char* GetTypeName() const override {
        return "Puzzle_TwoStateRotater_TestTrigger";
    }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override {}
    void OnCollisionExit(Entity other) override {}
    void OnTriggerEnter(Entity other) override {}
    void OnTriggerExit(Entity other) override {}

private:
    GameObjectRef targetRotater;
    int toggleKey = 'T';
};
