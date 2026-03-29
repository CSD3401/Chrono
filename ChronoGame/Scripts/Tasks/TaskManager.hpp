#pragma once
#include "../EngineAPI.hpp"

class TaskManager : public IScript {
    struct Checkpoint {
        std::string title;
        std::vector<std::string> tasks;
    };

public:
    TaskManager() {
        SCRIPT_GAMEOBJECT_REF(titleUIRef);
        SCRIPT_GAMEOBJECT_REF(task1UIRef);
        SCRIPT_GAMEOBJECT_REF(task1ToggleRef);
        SCRIPT_GAMEOBJECT_REF(task2UIRef);
        SCRIPT_GAMEOBJECT_REF(task2ToggleRef);
        SCRIPT_GAMEOBJECT_REF(task3UIRef);
        SCRIPT_GAMEOBJECT_REF(task3ToggleRef);
        SCRIPT_GAMEOBJECT_REF(task4UIRef);
        SCRIPT_GAMEOBJECT_REF(task4ToggleRef);
    }

    ~TaskManager() override = default;

    void Awake() override {}
    void Initialize(Entity entity) override { (void)entity; }

    void Start() override {
        currentCheckpointIndex = 0;
        completedTaskCount = 0;

        BuildCheckpoints();
        NormalizeCheckpointState();
        RefreshUI();

        Events::Listen("TaskCheckpointCompleted", [this](void*) {
            HandleTaskCompleted();
        });
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

private:
    static constexpr std::size_t kMaxVisibleTasks = 4;

    std::vector<Checkpoint> checkpoints;
    std::size_t currentCheckpointIndex = 0;
    std::size_t completedTaskCount = 0;

    GameObjectRef titleUIRef;
    GameObjectRef task1UIRef;
    GameObjectRef task1ToggleRef;
    GameObjectRef task2UIRef;
    GameObjectRef task2ToggleRef;
    GameObjectRef task3UIRef;
    GameObjectRef task3ToggleRef;
    GameObjectRef task4UIRef;
    GameObjectRef task4ToggleRef;

    void BuildCheckpoints() {
        checkpoints.clear();

		checkpoints.push_back({ 
            "Tutorial", 
                { 
                    "Unlock Security Gate", 
                    "Traverse Sinkhole",
                    "Enter Basement Access"
                } 
            }
        );

        checkpoints.push_back({
            "Transit Hub",
                {
                    "Bypass Entry Gantry"
                }
            }
        );

        checkpoints.push_back({
            "Control Room Alpha",
                {
                    "Restore Power",
                    "Unlock Exit Door"
                }
            }
        );

        checkpoints.push_back({
            "Dual Conduit",
                {
                    "Disable Laser Control Panel A",
                    "Disable Laser Control Panel B",
                    "Bypass Laser Grid",
                    "Override Central Laser Gate"
                }
            }
        );

        checkpoints.push_back({
            "Catwalk Alpha",
                {
                    "Cross Maintenance Catwalk"
                }
            }
        );

        checkpoints.push_back({
            "Control Room Beta",
                {
                    "Restore Power",
                    "Disable Laser Gantry"
                }
            }
        );

        checkpoints.push_back({
            "Catwalk Beta",
                {
                    "Bypass Laser Grid",
                    "Cross Maintenance Catwalk"
                }
            }
        );

        checkpoints.push_back({
            "Aether Core",
                {
                    "Restore Power to Door System",
                    "Bypass Laser Grid",
                    "Locate Lynn's Experimental Lab"
                }
            }
        );

        checkpoints.push_back({
            "Zero Point Reactor",
                {
                    "Disable Stabilizer Module",
                    "Override Coolant System",
                    "Decouple Power Core",
                    "Initiate Final Shutdown Sequence"
                }
            }
        );
    }

    void HandleTaskCompleted() {
        if (!HasActiveCheckpoint()) {
            return;
        }

        const std::size_t taskCount = GetTrackedTaskCount(checkpoints[currentCheckpointIndex]);
        if (taskCount == 0) {
            AdvanceCheckpoint();
            return;
        }

        if (completedTaskCount < taskCount) {
            ++completedTaskCount;
        }

        if (completedTaskCount >= taskCount) {
            AdvanceCheckpoint();
            return;
        }

        RefreshUI();
    }

    void AdvanceCheckpoint() {
        if (HasActiveCheckpoint()) {
            ++currentCheckpointIndex;
        }

        completedTaskCount = 0;
        NormalizeCheckpointState();
        RefreshUI();
    }

    void NormalizeCheckpointState() {
        while (HasActiveCheckpoint() && GetTrackedTaskCount(checkpoints[currentCheckpointIndex]) == 0) {
            ++currentCheckpointIndex;
        }

        if (!HasActiveCheckpoint()) {
            completedTaskCount = 0;
            return;
        }

        const std::size_t taskCount = GetTrackedTaskCount(checkpoints[currentCheckpointIndex]);
        if (completedTaskCount > taskCount) {
            completedTaskCount = taskCount;
        }
    }

    bool HasActiveCheckpoint() const {
        return currentCheckpointIndex < checkpoints.size();
    }

    void RefreshUI() {
        if (!HasActiveCheckpoint()) {
            SetAllUITexts("", "", "", "", "");
            return;
        }

        const Checkpoint& checkpoint = checkpoints[currentCheckpointIndex];

        SetUITextIfValid(titleUIRef, checkpoint.title);
        SetUITextIfValid(task1UIRef, BuildTaskLine(checkpoint, 0));
        SetUITextIfValid(task2UIRef, BuildTaskLine(checkpoint, 1));
        SetUITextIfValid(task3UIRef, BuildTaskLine(checkpoint, 2));
        SetUITextIfValid(task4UIRef, BuildTaskLine(checkpoint, 3));
    }

    std::string BuildTaskLine(const Checkpoint& checkpoint, std::size_t taskIndex) const {
        if (taskIndex >= GetTrackedTaskCount(checkpoint)) {
            return "";
        }

        const bool isCompleted = taskIndex < completedTaskCount;
        if (isCompleted) {
            switch (taskIndex) {
            case 0:
                NE::Scripting::SetToggleOn(task1ToggleRef.GetEntity(), true);
                break;
            case 1:
                NE::Scripting::SetToggleOn(task2ToggleRef.GetEntity(), true);
                break;
            case 2:
                NE::Scripting::SetToggleOn(task3ToggleRef.GetEntity(), true);
                break;
            case 3:
                NE::Scripting::SetToggleOn(task4ToggleRef.GetEntity(), true);
                break;
            }
        }
        return checkpoint.tasks[taskIndex];
    }

    void SetAllUITexts(const std::string& title,
        const std::string& task1,
        const std::string& task2,
        const std::string& task3,
        const std::string& task4) {
        SetUITextIfValid(titleUIRef, title);
        SetUITextIfValid(task1UIRef, task1);
		NE::Scripting::SetToggleOn(task1ToggleRef.GetEntity(), false);
        SetUITextIfValid(task2UIRef, task2);
		NE::Scripting::SetToggleOn(task2ToggleRef.GetEntity(), false);
        SetUITextIfValid(task3UIRef, task3);
		NE::Scripting::SetToggleOn(task3ToggleRef.GetEntity(), false);
        SetUITextIfValid(task4UIRef, task4);
		NE::Scripting::SetToggleOn(task4ToggleRef.GetEntity(), false);
    }

    void SetUITextIfValid(const GameObjectRef& ref, const std::string& text) {
        if (!ref.IsValid()) {
            return;
        }

        NE::Scripting::SetUIText(ref.GetEntity(), text.c_str());
    }

    //void SetUITextIfValid(const GameObjectRef& ref, const std::string& text) {
    //    if (!ref.IsValid()) {
    //        return;
    //    }

    //    NE::Scripting::SetUIText(ref.GetEntity(), text.c_str());
    //}

    std::size_t GetTrackedTaskCount(const Checkpoint& checkpoint) const {
        return checkpoint.tasks.size() < kMaxVisibleTasks ? checkpoint.tasks.size() : kMaxVisibleTasks;
    }
};
