#pragma once
#include "EngineAPI.hpp"
#include "Interactable_.hpp"
/*
* By Chan Kuan Fu Ryan (c.kuanfuryan)
* Interactable_ is the parent class for all interactable objects in the game.
* It simply provides a virtual function Interact that can be overridden by child classes.
*/

class Interactable_Grabbable : public Interactable_ {
public:
    Interactable_Grabbable() {
        SCRIPT_COMPONENT_REF(body, RigidbodyRef);
        SCRIPT_FIELD(isHeavy, Bool);
        SCRIPT_FIELD(activatesPressurePlates, Bool);
        SCRIPT_GAMEOBJECT_REF(playerGrabber);
    }
    ~Interactable_Grabbable() override = default;

    // == Custom Methods ==
    virtual void Interact() 
    {
    }

    // === Lifecycle Methods ===
    void Awake() override {}
    void Initialize(Entity entity) override {}
    void Start() override 
    {
        playerGrabber.SetEntity(NE::Scripting::GameObject::Find("PlayerGrabber").GetEntityId());
        if (!playerGrabber.IsValid())
        {
            LOG_ERROR("Player Grabber Not Found!");
        }
    }
    void Update(double deltaTime) override {}
    void OnDestroy() override {}

    // === Optional Callbacks ===
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}
    const char* GetTypeName() const override { return "Interactable_Grabbable"; }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }

    RigidbodyRef GetBody()
    {
        return body;
    }

private:
    RigidbodyRef body;
    bool isHeavy = false;
    bool activatesPressurePlates = false;
    GameObjectRef playerGrabber;
};