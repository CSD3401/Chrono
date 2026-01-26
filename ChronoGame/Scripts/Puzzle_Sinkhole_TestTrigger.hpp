#pragma once
#include "EngineAPI.hpp"

/*
* Puzzle_Sinkhole_TestTrigger
* Press keys to fire ChronoActivated/ChronoDeactivated events.
*/

class Puzzle_Sinkhole_TestTrigger : public IScript {
public:
    Puzzle_Sinkhole_TestTrigger() {
        SCRIPT_FIELD(activateKey, Int);
        SCRIPT_FIELD(deactivateKey, Int);
    }

    ~Puzzle_Sinkhole_TestTrigger() override = default;

    // === Lifecycle Methods ===
    void Awake() override {}
    void Initialize(Entity entity) override {}
    void Start() override {}

    void Update(double deltaTime) override {
        if (Input::WasKeyPressed(activateKey)) {
            Events::Send("ChronoActivated");
            LOG_INFO("Puzzle_Sinkhole_TestTrigger: ChronoActivated sent");
        }

        if (Input::WasKeyPressed(deactivateKey)) {
            Events::Send("ChronoDeactivated");
            LOG_INFO("Puzzle_Sinkhole_TestTrigger: ChronoDeactivated sent");
        }
    }

    void OnDestroy() override {}

    // === Optional Callbacks ===
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}

    const char* GetTypeName() const override {
        return "Puzzle_Sinkhole_TestTrigger";
    }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override {}
    void OnCollisionExit(Entity other) override {}
    void OnTriggerEnter(Entity other) override {}
    void OnTriggerExit(Entity other) override {}

private:
    int activateKey = 'C';
    int deactivateKey = 'V';
};
