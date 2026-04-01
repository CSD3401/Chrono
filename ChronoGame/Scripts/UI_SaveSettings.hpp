#pragma once
#include "EngineAPI.hpp"
#include <ScriptSDK/UI.h>
#include <algorithm>

/**
 * SavedSettings
 * -------------
 * Runtime-only settings cache that persists across scene switches while the game
 * is still running. It does NOT save to disk yet.
 */
struct SavedSettings {
    static inline float masterVolume = 1.0f;
    static inline float bgmVolume = 1.0f;
    static inline float sfxVolume = 1.0f;
    static inline float ambienceVolume = 1.0f;
    static inline float gammaNormalized = 1.0f;
    static inline bool hasBeenSaved = false;

    static float Clamp01(float value) {
        return std::clamp(value, 0.0f, 1.0f);
    }

    static void SaveAll(float master, float bgm, float sfx, float ambience, float gamma) {
        masterVolume = Clamp01(master);
        bgmVolume = Clamp01(bgm);
        sfxVolume = Clamp01(sfx);
        ambienceVolume = Clamp01(ambience);
        gammaNormalized = Clamp01(gamma);
        hasBeenSaved = true;
    }
};

/**
 * UI_SaveSettings
 * ---------------
 * Attach to the Save button entity and assign all relevant settings sliders.
 * One button saves all current slider values into the runtime cache.
 */
class UI_SaveSettings : public IScript {
public:
    UI_SaveSettings() {
        SCRIPT_GAMEOBJECT_REF(masterSlider);
        SCRIPT_GAMEOBJECT_REF(bgmSlider);
        SCRIPT_GAMEOBJECT_REF(sfxSlider);
        SCRIPT_GAMEOBJECT_REF(ambienceSlider);
        SCRIPT_GAMEOBJECT_REF(gammaSlider);
    }

    ~UI_SaveSettings() override = default;

    void Awake() override {}
    void Initialize(Entity entity) override { m_buttonEntity = entity; }
    void Start() override {}

    void Update(double /*dt*/) override {
        if (m_buttonEntity == 0) return;
        if (!UI::WasButtonClicked(m_buttonEntity) || !UI::IsButtonInteractable(m_buttonEntity))
            return;

        SaveCurrentSettings();
        LOG_INFO("UI_SaveSettings: runtime settings saved.");
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
    GameObjectRef masterSlider;
    GameObjectRef bgmSlider;
    GameObjectRef sfxSlider;
    GameObjectRef ambienceSlider;
    GameObjectRef gammaSlider;
    Entity m_buttonEntity = 0;

    static float ReadSlider(const GameObjectRef& ref, float fallback) {
        if (!ref.IsValid()) return SavedSettings::Clamp01(fallback);
        return SavedSettings::Clamp01(UI::GetSliderNormalizedValue(ref.GetEntity()));
    }

    void SaveCurrentSettings() {
        SavedSettings::SaveAll(
            ReadSlider(masterSlider, SavedSettings::masterVolume),
            ReadSlider(bgmSlider, SavedSettings::bgmVolume),
            ReadSlider(sfxSlider, SavedSettings::sfxVolume),
            ReadSlider(ambienceSlider, SavedSettings::ambienceVolume),
            ReadSlider(gammaSlider, SavedSettings::gammaNormalized));
    }
};