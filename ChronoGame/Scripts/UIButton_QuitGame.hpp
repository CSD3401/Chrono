#pragma once
#include "EngineAPI.hpp"
#include <cstdlib>

/*
 * UIButton_QuitGame
 * -----------------
 * Attach this to a UI Button entity in the main menu.
 * When the button is clicked and is interactable, the game process exits.
 *
 * Note: In editor, this will also close the running game process.
 */
class UIButton_QuitGame : public IScript {
public:
    UIButton_QuitGame() = default;
    ~UIButton_QuitGame() override = default;

    void Awake() override {}

    void Initialize(Entity entity) override {
        m_Button = entity;
    }

    void Start() override {}

    void Update(double /*dt*/) override {
        if (m_sent) return;

        if (NE::Scripting::WasButtonClicked(m_Button) &&
            NE::Scripting::IsButtonInteractable(m_Button)) {
            m_sent = true;
            std::exit(0);
        }
    }

    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}

    const char* GetTypeName() const override { return "UIButton_QuitGame"; }

    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }

private:
    Entity m_Button{};
    bool m_sent = false;
};
