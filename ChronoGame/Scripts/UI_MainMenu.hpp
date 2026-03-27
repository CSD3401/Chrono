#pragma once
#include "EngineAPI.hpp"
#include <ScriptSDK/UI.h>

/*
* UI_MainMenu:
* - ensures the cursor is visible and unlocked in the main menu
* - optionally fades in a target UICanvas on scene start
*
* Setup:
* - Attach to any entity in MainMenu scene
* - Optional: assign fadeCanvas to a UICanvas entity you want to fade
* - If fadeCanvas is not assigned, script tries to fade its own entity if it has UICanvas
*/
class UI_MainMenu : public IScript {
public:
    UI_MainMenu() {
        RegisterGameObjectRefVectorField("fadeCanvases", &fadeCanvases);
        SCRIPT_FIELD(enableFadeIn, Bool);
        SCRIPT_FIELD(fadeInDuration, Float);
    }
    ~UI_MainMenu() override = default;

    void Awake() override {}
    void Initialize(Entity entity) override {}

    void Start() override {
        Input::SetMouseLocked(false);
        NE::Scripting::SetMouseVisible(true);

        if (!enableFadeIn) return;

        m_targetCanvasEntities = ResolveTargetCanvases();
        if (m_targetCanvasEntities.empty()) return;

        if (fadeInDuration <= 0.0f) fadeInDuration = 0.01f;
        m_fading = true;
        m_fadeTimer = 0.0f;
        SetAllTargetCanvasAlpha(0.0f);
    }

    void Update(double deltaTime) override {
        if (!m_fading || m_targetCanvasEntities.empty()) return;

        m_fadeTimer += static_cast<float>(deltaTime);
        float t = m_fadeTimer / fadeInDuration;
        if (t >= 1.0f) {
            t = 1.0f;
            m_fading = false;
        }
        SetAllTargetCanvasAlpha(t);
    }
    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}
    const char* GetTypeName() const override { return "UI_MainMenu"; }

    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }

private:
    std::vector<GameObjectRef> fadeCanvases;
    bool enableFadeIn = true;
    float fadeInDuration = 2.0f;

    std::vector<Entity> m_targetCanvasEntities;
    bool m_fading = false;
    float m_fadeTimer = 0.0f;

    std::vector<Entity> ResolveTargetCanvases() const {
        std::vector<Entity> resolved;

        for (const auto& ref : fadeCanvases) {
            if (!ref.IsValid()) continue;
            const Entity e = ref.GetEntity();
            if (NE::ECS::Query::HasUICanvas(e)) {
                resolved.push_back(e);
            }
            else {
                LOG_WARNING("UI_MainMenu: fadeCanvases contains entity without UICanvas");
            }
        }
        if (resolved.empty()) {
            LOG_WARNING("UI_MainMenu: fadeCanvases is empty or invalid");
        }
        return resolved;
    }

    void SetAllTargetCanvasAlpha(float alpha) const {
        for (Entity e : m_targetCanvasEntities) {
            NE::ECS::Command::SetUICanvasAlpha(e, alpha);
        }
    }
};
