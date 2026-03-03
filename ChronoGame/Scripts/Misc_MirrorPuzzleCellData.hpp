#pragma once
#include "EngineAPI.hpp"


class Misc_MirrorPuzzleCellData : public IScript {
public:
    Misc_MirrorPuzzleCellData() {
        SCRIPT_FIELD(tileRow, Int);
        SCRIPT_FIELD(tileCol, Int);
        SCRIPT_FIELD(UP, Bool);
        SCRIPT_FIELD(DOWN, Bool);
        SCRIPT_FIELD(LEFT, Bool);
        SCRIPT_FIELD(RIGHT, Bool);
    }

    ~Misc_MirrorPuzzleCellData() override = default;

    void Awake() override {}
    void Initialize(Entity entity) override {}

    void Start() override {
    }

    void Update(double deltaTime) override {
    }

    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}
    const char* GetTypeName() const override { return "LaserListener"; }

    // === Collision Callbacks (required by IScript) ===
    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }

public: // imma jus access this from Puzzle_Mirror directly for ease of use - RF
	int tileRow = -1;
    int tileCol = -1;
    bool UP = false;
    bool DOWN = false;
    bool LEFT = false;
    bool RIGHT = false;
};