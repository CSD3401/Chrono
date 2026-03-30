#pragma once
#include "EngineAPI.hpp"
#include <string>

/**
 * Misc_PauseMenuHotkey
 * --------------------
 * When the player presses P, loads the pause menu scene by UUID.
 *
 * Uses the same delayed-switch state pattern as UIButton_SwitchScene scripts.
 *
 * Attach to any active entity in gameplay levels (e.g. empty GameObject or manager).
 *
 * Inspector:
 *   pauseMenuSceneUuid   Scene asset UUID (default: PauseMenu.scene from project meta).
 *   enableHotkey         Turn off to disable P without removing the script.
 *   switchDelaySeconds   Delay before SwitchScene (seconds).
 */
class Misc_PauseMenuHotkey : public IScript {
public:
    Misc_PauseMenuHotkey() {
        SCRIPT_FIELD(pauseMenuSceneUuid, String);
        SCRIPT_FIELD(enableHotkey, Bool);
        SCRIPT_FIELD(switchDelaySeconds, Float);
    }

    ~Misc_PauseMenuHotkey() override = default;

    void Awake() override {}
    void Initialize(Entity /*entity*/) override {}
    void Start() override {
        if (pauseMenuSceneUuid.empty())
            pauseMenuSceneUuid = "73931a4d-60a8-48f0-9eef-b832ef3a3a88";
    }

    void Update(double deltaTime) override {
        if (!enableHotkey) return;
        if (pauseMenuSceneUuid.empty()) return;

        if (m_pendingDelay) {
            m_delayElapsed += static_cast<float>(deltaTime);
            if (m_delayElapsed >= switchDelaySeconds) {
                m_sent = true;
                NE::Scripting::SwitchScene(pauseMenuSceneUuid);
            }
            return;
        }

        if (!Input::WasKeyPressed(static_cast<int>('P'))) return;
        if (m_sent) return;

        if (switchDelaySeconds <= 0.0f) {
            m_sent = true;
            NE::Scripting::SwitchScene(pauseMenuSceneUuid);
        } else {
            m_pendingDelay = true;
            m_delayElapsed = 0.0f;
        }
    }

    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}

    const char* GetTypeName() const override { return "Misc_PauseMenuHotkey"; }

    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }

private:
    std::string pauseMenuSceneUuid;
    bool enableHotkey = true;
    float switchDelaySeconds = 0.05f;
    bool m_sent = false;
    bool m_pendingDelay = false;
    float m_delayElapsed = 0.0f;
};
