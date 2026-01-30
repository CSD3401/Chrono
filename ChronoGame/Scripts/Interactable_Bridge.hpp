#pragma once
#include "EngineAPI.hpp"
#include "Interactable_.hpp"
#include "Highlightable_.hpp"

/*
* Interactable_Bridge
* Bridges raycast to specific interactable/highlight scripts by name.
*/

class Interactable_Bridge : public Interactable_ {
public:
    Interactable_Bridge() = default;
    ~Interactable_Bridge() override = default;

    // === Lifecycle Methods ===
    void Awake() override {}

    void Initialize(Entity entity) override {
        SCRIPT_FIELD(interactableScriptName, String);
        SCRIPT_FIELD(highlightScriptName, String);
    }

    void Start() override {}
    void Update(double deltaTime) override {}
    void OnDestroy() override {}

    // === Optional Callbacks ===
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}

    const char* GetTypeName() const override {
        return "Interactable_Bridge";
    }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override {}
    void OnCollisionExit(Entity other) override {}
    void OnTriggerEnter(Entity other) override {}
    void OnTriggerExit(Entity other) override {}

    void Interact() override {
        if (interactableScriptName.empty()) {
            return;
        }
        IScript* script = GameObject(GetEntity()).GetScript(interactableScriptName);
        auto* interactable = dynamic_cast<Interactable_*>(script);
        if (interactable) {
            interactable->Interact();
        }
    }

    void SetHighlight(bool enable) {
        if (highlightScriptName.empty()) {
            return;
        }
        IScript* script = GameObject(GetEntity()).GetScript(highlightScriptName);
        auto* highlightable = dynamic_cast<Highlightable_*>(script);
        if (highlightable) {
            highlightable->SetHighlight(enable);
        }
    }

private:
    std::string interactableScriptName;
    std::string highlightScriptName;
};
