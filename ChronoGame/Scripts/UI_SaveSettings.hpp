#pragma once
#include "EngineAPI.hpp"

/**
 * SavedSettings
 * -------------
 * Static storage for settings that persist across scene loads.
 * Not saved to disk — resets on application quit.
 */
struct SavedSettings {
    static inline float masterVolume = 1.0f;
    static inline bool  hasBeenSaved = false;
};

/**
 * UI_SaveSettings
 * ---------------
 * Attach to the Save button entity (must have UIButton).
 * Assign the volume slider so this script can read its value on click.
 *
 * On click: saves the current slider value to SavedSettings::masterVolume.
 */
class UI_SaveSettings : public IScript {
public:
    UI_SaveSettings() {
        SCRIPT_GAMEOBJECT_REF(volumeSlider);
    }

    ~UI_SaveSettings() override = default;

    void Awake() override {}
    void Initialize(Entity entity) override { m_buttonEntity = entity; }

    void Start() override {
        m_slider = volumeSlider.IsValid() ? volumeSlider.GetEntity() : 0;

        if (m_slider == 0)
            LOG_WARNING("UI_SaveSettings: volumeSlider not set.");
    }

    void Update(double /*dt*/) override {
        if (m_buttonEntity == 0) return;
        //if (!NE::ECS::Query::HasUIButton(m_buttonEntity)) return;

        if (NE::Scripting::WasButtonClicked(m_buttonEntity)) {
            SaveCurrentSettings();
            PlayAudio("Event:/MAIN_MENU/BUTTON_CLICK");
            LOG_INFO("UI_SaveSettings: Settings saved.");
        }
    }

    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}

    const char* GetTypeName() const override { return "UI_SaveSettings"; }

    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }

private:
    GameObjectRef volumeSlider;
    Entity m_buttonEntity = 0;
    Entity m_slider = 0;

    void SaveCurrentSettings() {
        if (m_slider != 0) {
            float norm = UI::GetSliderNormalizedValue(m_slider);
            if (norm < 0.0f) norm = 0.0f;
            if (norm > 1.0f) norm = 1.0f;
            SavedSettings::masterVolume = norm;
        }
        SavedSettings::hasBeenSaved = true;
    }
};