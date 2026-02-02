#pragma once
#include "EngineAPI.hpp"
#include "Interactable_Grabbable.hpp"
/*
* By Chan Kuan Fu Ryan (c.kuanfuryan)
* Interactable_ is the parent class for all interactable objects in the game.
* It simply provides a virtual function Interact that can be overridden by child classes.
*/

class Misc_Grabber : public IScript {
public:
    Misc_Grabber() {
    }
    ~Misc_Grabber() override = default;

    // == Custom Methods ==
    virtual void Interact()
    {
        GameObjectRef obj;
    }

    // === Lifecycle Methods ===
    void Awake() override {}
    void Initialize(Entity entity) override {}
    void Start() override
    {
    }
    void Update(double deltaTime) override 
    {
    }
    void OnDestroy() override {}

    // === Optional Callbacks ===
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}
    const char* GetTypeName() const override { return "Misc_Grabber"; }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }

    bool IsGrabbing()
    {
        return isGrabbing;
    }

    void Grab(Interactable_Grabbable g)
    {
        currentlyGrabbing = g;
        isGrabbing = true;
    }
    
    void LetGo()
    {
        isGrabbing = false;
    }

private:
    // no instance cause whats singleton lmao
    float distance;
    float grabStrength;
    float damping;
    float timerBuffer;

    Vec3 targetPosition;
    Interactable_Grabbable currentlyGrabbing;
    bool isGrabbing = false;

    float defaultDrag;
    float defaultAngularDrag;
    float timer;


};