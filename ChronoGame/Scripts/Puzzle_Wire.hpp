#pragma once
#include "EngineAPI.hpp"
#include "Puzzle_.hpp"
#include <map>
#include <vector>

/**
 * Template - Auto-generated script template
 * Implement your game logic in the lifecycle methods below.
 */

enum WirePuzzleIndex
{
    _01,
    _02,
    _03
};
class Puzzle_Wire : public Puzzle_ {
public:
    Puzzle_Wire() {
        // Register any editable fields here
        // Example: SCRIPT_FIELD(speed, float);
        // Example: SCRIPT_FIELD_VECTOR(blingstring, String);;
        SCRIPT_GAMEOBJECT_REF(wireHolderObject);
        SCRIPT_FIELD_VECTOR(wireColours, Int);
        SCRIPT_FIELD_VECTOR(correctColours, Int);
        SCRIPT_FIELD(index, Int);

    }

    ~Puzzle_Wire() override = default;

    // === Lifecycle Methods ===

    void Awake() override {
        // Called when the script component is first created
    }

    void Initialize(Entity entity) override {
        // Called to initialize the script with its entity
        // Grab all the children from the entity
        numWires = GetChildren(wireHolderObject).size();
    }

    void Start() override {
        // Called when the script is enabled and play mode starts

        // Tell all the children to change to their respective colour
        InitWireColours();
    }

    void Update(double deltaTime) override {
        // Called every frame while the script is enabled
    }

    void OnDestroy() override {
        // Called when the script is about to be destroyed
    }

    // === Optional Callbacks ===

    void OnEnable() override {
        // Called when the script is enabled
    }

    void OnDisable() override {
        // Called when the script is disabled
    }

    void OnValidate() override {
        // Called when a field value is changed in the editor
    }

    const char* GetTypeName() const override {
        return "Puzzle_Wire";
    }

    // === Collision Callbacks ===

    void OnCollisionEnter(Entity other) override {
        // Called when this entity starts colliding with another
    }

    void OnCollisionExit(Entity other) override {
        // Called when this entity stops colliding with another
    }

    void OnTriggerEnter(Entity other) override {
        // Called when this entity enters a trigger
    }

    void OnTriggerExit(Entity other) override {
        // Called when this entity exits a trigger
    }

    enum WIRE_COLOUR
    {
        BLUE = 0,
        RED,
        YELLOW,
        ORANGE,
        PURPLE,
        GREEN,
        PINK,
        WHITE // Finished
    };

    void InitWireColours()
    {

        std::string message = "SetupWirePuzzle" + std::to_string(index);
        for (int i = 0; i < numWires; ++i)
        {
            Events::Send(message.c_str(), (void*)(wireColours[i]));
        }
    }

    void SwapWireColours(int leftIndex, int rightIndex)
    {
        // Update the colours in the vector
        std::swap(wireColours[leftIndex], wireColours[rightIndex]);

        // update the wire gameobjects to their new colour
        std::string message = "WirePuzzle" + std::to_string(index);
        Events::Send((message + std::to_string(leftIndex)).c_str(), (void*)((WIRE_COLOUR)wireColours[leftIndex]));
        Events::Send((message + std::to_string(rightIndex)).c_str(), (void*)((WIRE_COLOUR)wireColours[leftIndex]));

        // Check if the puzzle is solved
        if (CheckWireColours())
        {
            // Change all wire colours to white
            for (int i = 0; i < numWires; ++i)
            {
                Events::Send((message + std::to_string(i)).c_str(), (void*)WIRE_COLOUR::WHITE);
            }
            // Send message that puzzle is solved
            Solve();
        }
    }

    bool CheckWireColours()
    {
        for (int i = 0; i < wireColours.size(); ++i)
        {
            if (wireColours[i] != correctColours[i])
            {
                return false;
            }
        }
        return true;
    }

private:
    // Add your private member variables here
    // Example: float speed = 5.0f;
    GameObjectRef wireHolderObject;
    int numWires = 3;
    std::vector<int> wireColours;
    std::vector<int> correctColours;
    int index;
};