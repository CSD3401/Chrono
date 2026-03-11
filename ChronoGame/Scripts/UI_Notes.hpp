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
        SCRIPT_GAMEOBJECT_REF(objectToActivate);
    }

    const char* GetTypeName() const override { return "UI_Notes"; }

    // Open with raycast interaction only
    void Interact() override {
        if (!objectToActivate) {
            return;
        }

        Entity target = objectToActivate.GetEntity();

        if (!noteIsOpen) {
            noteIsOpen = true;
            SetActive(true, target);
            waitingForMouseReleaseAfterOpen = true;
            LOG_DEBUG("open note!");
            PlayAudio("event:/COLOR_CLICK");
        }
    }

    // Close without raycast: any later left click will close it
    void Update(double /*dt*/) override {
        if (!objectToActivate) return;

        Entity target = objectToActivate.GetEntity();

        if (!noteIsOpen) {
            waitingForMouseReleaseAfterOpen = false;
            return;
        }

        // Ignore the same click that opened the note
        if (waitingForMouseReleaseAfterOpen) {
            if (!Input::IsMouseDown(GLFW_MOUSE_BUTTON_LEFT)) {
                waitingForMouseReleaseAfterOpen = false;
            }
            return;
        }

        // Any new left click closes the note
        if (Input::WasMousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
            noteIsOpen = false;
            SetActive(false, target);
            LOG_DEBUG("close note!");
            PlayAudio("event:/COLOR_CLICK");
        }
    }

private:
    GameObjectRef objectToActivate;
    bool noteIsOpen = false;
    bool waitingForMouseReleaseAfterOpen = false;
};