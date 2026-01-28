#pragma once
#include "EngineAPI.hpp"
#include "Highlightable_.hpp"
#include "Interactable_.hpp"
#include "Player_Controller.hpp"

#define GLFW_MOUSE_BUTTON_LEFT 0
/*
* By Chan Kuan Fu Ryan (c.kuanfuryan)
* Player_Raycast is responsible for point and click interactions.
* It needs to be attached to the player's camera to function properly.
*/

class Player_Raycast : public IScript {
public:
    Player_Raycast() {
        SCRIPT_FIELD(interval, Float);
        SCRIPT_FIELD(distance, Float);
        SCRIPT_FIELD_LAYERREF(targetLayer);
    }
    ~Player_Raycast() override = default;

    // === Custom Methods ===
    void NoInteract()
    {
        if (storedEntity)
        {
            GameObject storedObject = GameObject(storedEntity);
            Highlightable_* h = storedObject.GetComponent<Highlightable_>();
            if (h) { h->SetHighlight(false); }
            storedEntity = 0;
        }
    }

    // === Lifecycle Methods ===
    void Awake() override {}
    void Initialize(Entity entity) override {}
    void Start() override {}

    void Update(double deltaTime) override {

        // === Raycast Interval ===
        timer += static_cast<float>(deltaTime);
        if (timer > interval)
        {
            timer = 0.0f;
            Vec3 origin = TF_GetPosition();
            Vec3 direction = TF_GetForward();
            RaycastHit raycastHit = Raycast(
                origin, 
                direction, 
                distance, 
                targetLayer.ToMask());

            // Once we hit something, check for interactable and store it
            if (raycastHit.hasHit)
            {
                GameObject go = GameObject(raycastHit.entity);
                Highlightable_* h = go.GetComponent<Highlightable_>();

                LOG_DEBUG("Hit something!");

                // Only proceed if Highlightable component exists and we are hitting another entity
                if (h && raycastHit.entity != storedEntity)
                {
                    if (storedEntity)
                    {
                        // By virtue of only storing entities with Highlightable component,
                        // we can safe assume that the storedEntity already has one
                        // GameObject(storedEntity).GetComponent<Highlightable_>()->SetHighlight(false);
                    }

                    // Then we can set Highlight and store
                    // h->SetHighlight(true);
                    // storedEntity = raycastHit.entity;
                }
                else
                {
                    NoInteract();
                }
            }
            else
            {
                NoInteract();

				LOG_DEBUG("Hit nothing!");
            }
        }

        if (storedEntity)
        {
            if (Input::WasKeyPressed(GLFW_MOUSE_BUTTON_LEFT))
            {
                GameObject storedObject = GameObject(storedEntity);
                Interactable_* i = storedObject.GetComponent<Interactable_>();
                if (i) { i->Interact(); }
            }
        }
    }
    void OnDestroy() override {}

    // === Optional Callbacks ===
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}
    const char* GetTypeName() const override { return "Player_Raycast"; }

    // === Collision Callbacks ===
    void OnCollisionEnter(Entity other) override {}
    void OnCollisionExit(Entity other) override {}
    void OnTriggerEnter(Entity other) override {}
    void OnTriggerExit(Entity other) override {}

private:
	// === Inspector Fields ===
    float interval;
    float distance;
    LayerRef targetLayer;

	// === Private Fields ===
    float timer;
    Entity storedEntity;
};