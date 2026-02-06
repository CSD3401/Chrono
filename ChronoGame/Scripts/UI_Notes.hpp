#pragma once
#include "EngineAPI.hpp"
#include "Interactable_.hpp"

#define GLFW_MOUSE_BUTTON_LEFT 0

class UI_Notes : public Interactable_ {
public:
    UI_Notes() = default;
    ~UI_Notes() override = default;

    void Initialize(Entity entity) override {
        _SetEntity(entity);

        // IMPORTANT: use the macro so the field key is stable for the editor/serialization
        SCRIPT_GAMEOBJECT_REF(objectToActivate);
    }

    const char* GetTypeName() const override { return "UI_Notes"; }

    // Raycast-click opens (only if currently inactive)
    void Interact() override {
        if (!objectToActivate) {
            //LOG_DEBUG("UI_Notes: objectToActivate is NOT set");
            return;
        }

        Entity target = objectToActivate.GetEntity();
        if (!IsActive(target)) {
            SetActive(true, target);

            // ignore the same click that opened it
            ignoreNextMouseClick = true;
        }
    }

    // While active: ANY click closes (no raycast)
    void Update(double /*dt*/) override {
        if (!objectToActivate) return;

        Entity target = objectToActivate.GetEntity();
        if (!IsActive(target)) return;

        if (Input::WasMousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
            if (ignoreNextMouseClick) {
                ignoreNextMouseClick = false;
                return;
            }

            SetActive(false, target);
        }
    }

private:
    GameObjectRef objectToActivate;
    bool ignoreNextMouseClick = false;
};
