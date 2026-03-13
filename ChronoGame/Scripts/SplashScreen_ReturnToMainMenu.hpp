#pragma once
#include "EngineAPI.hpp"

/**
 * SplashScreen_ReturnToMainMenu
 * ----------------------------
 * Attach to any entity in the Splash Screen scene (e.g. same as SplashScreen_Controller or an empty).
 * Listens for "SplashScreenDone" (fired by SplashScreen_Controller when all slides finish)
 * and switches to the Main Menu scene.
 */
class SplashScreen_ReturnToMainMenu : public IScript {
public:
    SplashScreen_ReturnToMainMenu() = default;
    ~SplashScreen_ReturnToMainMenu() override = default;

    void Awake() override {}
    void Initialize(Entity) override {}
    void Start() override {
        Events::Listen("SplashScreenDone", [this](void*) {
            NE::Scripting::SwitchScene("e17cc794-74d9-40ee-9c9e-efaa829ab09a");
        });
    }

    void Update(double /*dt*/) override {}
    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}

    const char* GetTypeName() const override { return "SplashScreen_ReturnToMainMenu"; }

    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }
};
