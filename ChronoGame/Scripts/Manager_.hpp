#pragma once
#include "EngineAPI.hpp"
/*
* === WORK IN PROGRESS ===
* By Chan Kuan Fu Ryan (c.kuanfuryan)
* Manager_ is a static instance which provides centralised storage
* for frequently used game data and methods.
*/

class Manager_ : public IScript {
public:
	static Manager_* instance;
    Manager_() {
		SCRIPT_FIELD(highlightMaterial, MaterialRef);
    }
    ~Manager_() override = default;

    // == Custom Methods ==
    MaterialRef GetHighlightMaterial() const {
        return highlightMaterial;
	}

    // === Lifecycle Methods ===
    void Awake() override {
        if (!instance) {
            instance = this;
        }
        else {
			LOG_WARNING("Multiple instances of Manager_ detected!");
        }
    }
    void Initialize(Entity entity) override {}
    void Start() override {}
    void Update(double deltaTime) override {}
    void OnDestroy() override {}

    // === Optional Callbacks ===
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}
    const char* GetTypeName() const override { return "Manager_"; }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override {}
    void OnCollisionExit(Entity other) override {}
    void OnTriggerEnter(Entity other) override {}
    void OnTriggerExit(Entity other) override {}

private:
    MaterialRef highlightMaterial;
};