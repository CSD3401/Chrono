#pragma once
#include "EngineAPI.hpp"
#include "Puzzle_OneWaySwitch.hpp"

/*
* Puzzle_OneWaySwitch_TestTrigger
* Press a key to call Interact() on a Puzzle_OneWaySwitch.
*/

class Puzzle_OneWaySwitch_TestTrigger : public IScript {
public:
    Puzzle_OneWaySwitch_TestTrigger() {
        SCRIPT_GAMEOBJECT_REF(targetSwitch);
        SCRIPT_FIELD(toggleKey, Int);
        SCRIPT_FIELD(resetKey, Int);
    }

    ~Puzzle_OneWaySwitch_TestTrigger() override = default;

    // === Lifecycle Methods ===
    void Awake() override {}
    void Initialize(Entity entity) override {}
    void Start() override {}

    void Update(double deltaTime) override {
        if (!targetSwitch.IsValid()) {
            LOG_WARNING("Puzzle_OneWaySwitch_TestTrigger: targetSwitch not set");
            return;
        }

        auto* oneWaySwitch = GameObject(targetSwitch).GetComponent<Puzzle_OneWaySwitch>();
        if (!oneWaySwitch) {
            LOG_WARNING("Puzzle_OneWaySwitch_TestTrigger: missing Puzzle_OneWaySwitch on target");
            return;
        }

        if (Input::WasKeyPressed(toggleKey)) {
            oneWaySwitch->Interact();
            LOG_INFO("Puzzle_OneWaySwitch_TestTrigger: Interact invoked");
        }

        if (Input::WasKeyPressed(resetKey)) {
            oneWaySwitch->ResetState();
            LOG_INFO("Puzzle_OneWaySwitch_TestTrigger: ResetState invoked");
        }
    }

    void OnDestroy() override {}

    // === Optional Callbacks ===
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}

    const char* GetTypeName() const override {
        return "Puzzle_OneWaySwitch_TestTrigger";
    }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override {}
    void OnCollisionExit(Entity other) override {}
    void OnTriggerEnter(Entity other) override {}
    void OnTriggerExit(Entity other) override {}

private:
    GameObjectRef targetSwitch;
    int toggleKey = 'T';
    int resetKey = 'R';
};
