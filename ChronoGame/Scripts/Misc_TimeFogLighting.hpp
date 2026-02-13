#pragma once
#include <algorithm>
#include <string>
#include "EngineAPI.hpp"

/*
    Misc_TimeFogLighting
    -------------------
    Listens to the time-switch events:
      - ChronoActivated   -> PAST
      - ChronoDeactivated -> PRESENT

    Per request:
      - PRESENT  : thicker fog + redder lighting
      - PAST     : restore to the scene's original (captured at Start)

    Usage:
      1) Add this script to any always-alive entity (e.g., a "GameManager" object).
      2) Tweak the exposed fields for the "present" look.

    Note:
      - This script ONLY affects global render settings (fog + ambient).
      - It automatically restores the original settings when the script is disabled/destroyed
        (e.g., when you stop the game in the editor).
*/

class Misc_TimeFogLighting : public IScript {
public:
    Misc_TimeFogLighting() {
        // Toggles
        SCRIPT_FIELD(affectFog, Bool);
        SCRIPT_FIELD(affectAmbient, Bool);
        SCRIPT_FIELD(applyStateOnStart, Bool);
        SCRIPT_FIELD(startInPresent, Bool);

        // PRESENT fog look
        SCRIPT_FIELD(presentFogEnabled, Bool);
        SCRIPT_FIELD(presentFogColor, Vec3);
        SCRIPT_FIELD(presentFogDensity, Float);    // Used if mode is Exponential / ExponentialSquared
        SCRIPT_FIELD(presentFogStart, Float);      // Used if mode is Linear
        SCRIPT_FIELD(presentFogEnd, Float);        // Used if mode is Linear

        // PRESENT ambient look
        SCRIPT_FIELD(presentAmbientColor, Vec3);
        SCRIPT_FIELD(presentAmbientIntensity, Float);
    }

    ~Misc_TimeFogLighting() override = default;

    void Awake() override {
        listeningEnabled = true;
        RegisterEventListeners();
    }

    void Initialize(Entity entity) override { (void)entity; }

    void Start() override {
        CaptureBaseRenderSettings();

        if (applyStateOnStart) {
            if (startInPresent) {
                ApplyPresent();
            }
            else {
                ApplyPast();
            }
        }
    }

    void Update(double) override {}

    void OnDestroy() override {
        listeningEnabled = false;
        RestoreBaseIfCaptured();
    }
    void OnEnable() override { listeningEnabled = true; }
    void OnDisable() override {
        listeningEnabled = false;
        RestoreBaseIfCaptured();
    }
    void OnValidate() override {
        // Safety clamps for inspector-driven values
        presentFogDensity = std::max(0.0f, presentFogDensity);
        presentFogStart = std::max(0.0f, presentFogStart);
        presentFogEnd = std::max(presentFogStart + 0.01f, presentFogEnd);
        presentAmbientIntensity = std::max(0.0f, presentAmbientIntensity);
    }

    const char* GetTypeName() const override { return "Misc_TimeFogLighting"; }

    void OnCollisionEnter(Entity) override {}
    void OnCollisionExit(Entity) override {}
    void OnCollisionStay(Entity) override {}
    void OnTriggerEnter(Entity) override {}
    void OnTriggerExit(Entity) override {}
    void OnTriggerStay(Entity) override {}

private:
    // ===== Inspector fields =====
    bool affectFog = true;
    bool affectAmbient = true;

    // If true, applies either present or past immediately at Start() after caching base.
    bool applyStateOnStart = true;
    bool startInPresent = false; // Most of your scripts treat ChronoActivated as "past".

    // PRESENT fog settings (thicker = higher density, closer end distance, etc.)
    bool  presentFogEnabled = true;
    Vec3  presentFogColor = Vec3(0.35f, 0.05f, 0.05f); // reddish haze
    float presentFogDensity = 0.055f;
    float presentFogStart = 0.0f;
    float presentFogEnd = 18.0f;

    // PRESENT ambient settings (redder)
    Vec3  presentAmbientColor = Vec3(0.45f, 0.10f, 0.10f);
    float presentAmbientIntensity = 1.0f;

    // ===== Runtime state =====
    bool eventsRegistered = false;
    bool listeningEnabled = false;

    // Base render settings (PAST / "normal")
    bool baseCaptured = false;

    bool baseFogEnabled = false;
    RenderSettings::FogMode baseFogMode = RenderSettings::FogMode::Exponential;
    Vec3 baseFogColor = Vec3(0.5f, 0.5f, 0.5f);
    float baseFogStart = 0.0f;
    float baseFogEnd = 50.0f;
    float baseFogDensity = 0.01f;

    RenderSettings::EnvSource baseEnvSource = RenderSettings::EnvSource::Color;
    Vec3 baseAmbientColor = Vec3(1.0f, 1.0f, 1.0f);
    float baseAmbientIntensity = 1.0f;

    // Restore helper so we don't accidentally "recapture" current (modified) state during shutdown.
    void RestoreBaseIfCaptured() {
        if (!baseCaptured) return;

        // Fog
        if (affectFog) {
            RenderSettings::SetFogEnabled(baseFogEnabled);
            RenderSettings::SetFogMode(baseFogMode);
            RenderSettings::SetFogColor(baseFogColor);
            RenderSettings::SetFogStart(baseFogStart);
            RenderSettings::SetFogEnd(baseFogEnd);
            RenderSettings::SetFogDensity(baseFogDensity);
        }

        // Ambient
        if (affectAmbient) {
            RenderSettings::SetEnvSource(baseEnvSource);
            RenderSettings::SetAmbientColor(baseAmbientColor);
            RenderSettings::SetAmbientIntensity(baseAmbientIntensity);
        }

        LOG_INFO("Misc_TimeFogLighting: restored base render settings on disable/destroy");
    }

    void RegisterEventListeners() {
        if (eventsRegistered) return;

        // PAST
        Events::Listen("ChronoActivated", [this](void*) {
            if (!listeningEnabled) return;
            ApplyPast();
            });

        // PRESENT
        Events::Listen("ChronoDeactivated", [this](void*) {
            if (!listeningEnabled) return;
            ApplyPresent();
            });

        eventsRegistered = true;
    }

    void CaptureBaseRenderSettings() {
        if (baseCaptured) return;

        // Fog
        baseFogEnabled = RenderSettings::IsFogEnabled();
        baseFogMode = RenderSettings::GetFogMode();
        baseFogColor = RenderSettings::GetFogColor();
        baseFogStart = RenderSettings::GetFogStart();
        baseFogEnd = RenderSettings::GetFogEnd();
        baseFogDensity = RenderSettings::GetFogDensity();

        // Ambient
        baseEnvSource = RenderSettings::GetEnvSource();
        baseAmbientColor = RenderSettings::GetAmbientColor();
        baseAmbientIntensity = RenderSettings::GetAmbientIntensity();

        baseCaptured = true;

        LOG_INFO("Misc_TimeFogLighting: cached base (past) render settings");
    }

    void ApplyPresent() {
        if (!baseCaptured) CaptureBaseRenderSettings();

        // ===== Fog =====
        if (affectFog) {
            RenderSettings::SetFogEnabled(presentFogEnabled);
            RenderSettings::SetFogColor(presentFogColor);

            // Keep whatever fog mode the scene is using, but push its params to be "thicker".
            // (You can still change the mode in the editor's global settings.)
            RenderSettings::FogMode mode = RenderSettings::GetFogMode();

            if (mode == RenderSettings::FogMode::Linear) {
                float start = std::max(0.0f, presentFogStart);
                float end = std::max(start + 0.01f, presentFogEnd);
                RenderSettings::SetFogStart(start);
                RenderSettings::SetFogEnd(end);
            }
            else {
                RenderSettings::SetFogDensity(std::max(0.0f, presentFogDensity));
            }
        }

        // ===== Ambient =====
        if (affectAmbient) {
            // Preserve the env source (skybox/gradient/color) unless your scene already uses Color.
            // We always set the ambient tint + intensity to get the redder mood.
            RenderSettings::SetAmbientColor(presentAmbientColor);
            RenderSettings::SetAmbientIntensity(std::max(0.0f, presentAmbientIntensity));
        }

        LOG_INFO("Misc_TimeFogLighting: applied PRESENT look (thicker fog + red lighting)");
    }

    void ApplyPast() {
        if (!baseCaptured) CaptureBaseRenderSettings();

        // ===== Fog =====
        if (affectFog) {
            RenderSettings::SetFogEnabled(baseFogEnabled);
            RenderSettings::SetFogMode(baseFogMode);
            RenderSettings::SetFogColor(baseFogColor);
            RenderSettings::SetFogStart(baseFogStart);
            RenderSettings::SetFogEnd(baseFogEnd);
            RenderSettings::SetFogDensity(baseFogDensity);
        }

        // ===== Ambient =====
        if (affectAmbient) {
            RenderSettings::SetEnvSource(baseEnvSource);
            RenderSettings::SetAmbientColor(baseAmbientColor);
            RenderSettings::SetAmbientIntensity(baseAmbientIntensity);
        }

        LOG_INFO("Misc_TimeFogLighting: restored PAST look (normal lighting + fog)");
    }
};
