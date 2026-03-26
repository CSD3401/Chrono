#pragma once
#include "../EngineAPI.hpp"

class TaskManager : public IScript {
    struct Task {
        std::string description;
        bool isCompleted = false;
	};

    struct Checkpoint {
        std::string name;
        std::vector<Task> tasks;
	};

public:
    TaskManager() {
    }

    ~TaskManager() override = default;

    void Awake() override {
        Events::Listen("CheckpointReached", [this](void*) {
            CheckpointReached();
            }
        );
    }
    void Initialize(Entity entity) override {
    }
    void Start() override {
        { // Tutorial
		    Task task1{ "Open the gate" };
		    Task task2{ "Find a way to cross over to get to the elevator" };
			Checkpoint checkpoint1{ "Tutorial", { task1, task2 } };
        }
    }

    void Update(double /*dt*/) override {
    }

    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}

    const char* GetTypeName() const override { return "TaskManager"; }

    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }

    void CheckpointReached() {
        checkpointCount++;

	}

private:
	int checkpointCount = 0;
	std::vector<Checkpoint> checkpoints;

    GameObjectRef titleUIRef;
    GameObjectRef task1UIRef;
    GameObjectRef task2UIRef;
    GameObjectRef task3UIRef;
    GameObjectRef task4UIRef;
};