#pragma once
#include <memory>
#include <algorithm>
#include "EngineAPI.hpp"
/*
* === WORK IN PROGRESS ===
* By Chan Kuan Fu Ryan (c.kuanfuryan)
* Manager_ is a static instance which provides centralised storage
* for frequently used game data and methods.
*/

class Manager_ : public IScript {
public:
    Manager_() {
		SCRIPT_FIELD(highlightMaterial, MaterialRef);
    }
    ~Manager_() override = default;

    // == Custom Methods ==
    MaterialRef GetHighlightMaterial() const {
        return highlightMaterial;
	}
    float Lerp(float a, float b, float t) {
        t = std::clamp(t, 0.0f, 1.0f);
        return a + (b - a) * t;
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
    const char* GetTypeName() const override { return "Manager_"; }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override {}
    void OnCollisionExit(Entity other) override {}
    void OnTriggerEnter(Entity other) override {}
    void OnTriggerExit(Entity other) override {}

private:
    MaterialRef highlightMaterial;
};