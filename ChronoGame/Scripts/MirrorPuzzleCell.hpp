#pragma once
#include "EngineAPI.hpp"

/**
 * PuzzleCell - Represents a single cell in the mirror puzzle grid
 * 
 * Each cell has 4 directional paths (up, down, left, right) that determine
 * if the navigator can move to/from this cell in those directions.
 * 
 * Cells are clickable - when clicked, they notify the MirrorPuzzleManager.
 */
class MirrorPuzzleCell : public IScript {
public:
    enum class CellSide {
        Left = 0,
        Right = 1
    };

    MirrorPuzzleCell() = default;
    ~MirrorPuzzleCell() override = default;

    void Initialize(Entity entity) override {
        // Directional paths
        SCRIPT_FIELD(hasRightPath, Bool);
        SCRIPT_FIELD(hasUpPath, Bool);
        SCRIPT_FIELD(hasLeftPath, Bool);
        SCRIPT_FIELD(hasDownPath, Bool);
        
        // Visual objects for paths (optional)
        SCRIPT_FIELD(showPathVisuals, Bool);
        
        // Grid position (set by manager)
        SCRIPT_FIELD(gridX, Int);
        SCRIPT_FIELD(gridY, Int);
        
        // Which side this cell belongs to
        RegisterEnumField("cellSide", &side, {
            "Left",
            "Right"
        });
    }

    void Awake() override {
        // Cache initial state
        UpdateVisuals();
    }

    void Start() override {
        LOG_DEBUG("PuzzleCell initialized at ({}, {}) on {} side", 
            gridX, gridY, side == CellSide::Left ? "LEFT" : "RIGHT");
    }

    void Update(double deltaTime) override {
        // Check for mouse click on this cell
        if (Input::WasMouseButtonPressed(0)) { // Left mouse button
            if (IsMouseOverCell()) {
                OnCellClicked();
            }
        }
    }

    void OnValidate() override {
        UpdateVisuals();
    }

    /**
     * Check if mouse cursor is over this cell (simplified raycast)
     */
    bool IsMouseOverCell() const {
        // Get mouse world position (you'll need to implement this based on your engine's camera system)
        // For now, this is a placeholder that checks distance
        Vec3 cellPos = GetPosition();
        Vec3 mouseWorldPos = GetMouseWorldPosition();
        
        float distance = Vec3::Distance(cellPos, mouseWorldPos);
        return distance < 0.5f; // Within half a grid cell
    }

    /**
     * Called when this cell is clicked
     */
    void OnCellClicked() {
        LOG_DEBUG("Cell clicked at ({}, {}) on {} side", 
            gridX, gridY, side == CellSide::Left ? "LEFT" : "RIGHT");
        
        // Send event to manager with cell position and side
        struct CellClickData {
            int x, y;
            CellSide side;
        };
        
        CellClickData data = { gridX, gridY, side };
        Events::Send("CellClicked", &data);
    }

    /**
     * Check if this cell has a path in a given direction
     */
    bool HasPathInDirection(int dx, int dy) const {
        if (dx == 1 && dy == 0) return hasRightPath;
        if (dx == -1 && dy == 0) return hasLeftPath;
        if (dx == 0 && dy == 1) return hasUpPath;
        if (dx == 0 && dy == -1) return hasDownPath;
        return false;
    }

    /**
     * Get the opposite direction path
     */
    bool HasPathFromDirection(int dx, int dy) const {
        // Opposite direction
        return HasPathInDirection(-dx, -dy);
    }

    // Getters
    int GetGridX() const { return gridX; }
    int GetGridY() const { return gridY; }
    CellSide GetSide() const { return side; }
    
    bool HasRight() const { return hasRightPath; }
    bool HasUp() const { return hasUpPath; }
    bool HasLeft() const { return hasLeftPath; }
    bool HasDown() const { return hasDownPath; }

    const char* GetTypeName() const override {
        return "MirrorPuzzleCell";
    }

private:
    /**
     * Get mouse world position (placeholder - implement based on your camera system)
     */
    Vec3 GetMouseWorldPosition() const {
        // TODO: Implement proper screen-to-world raycast based on your engine's camera
        // For now, return a dummy position
        // This should use: Camera::ScreenToWorldPoint(Input::GetMousePosition())
        return Vec3(0, 0, 0);
    }

    void UpdateVisuals() {
        // Update visual representations of paths
        // You could enable/disable arrow game objects here
        // For now, just log the state
        if (showPathVisuals) {
            // Placeholder for visual updates
        }
    }

    // Path data - which directions can the navigator move from this cell
    bool hasRightPath = false;
    bool hasUpPath = false;
    bool hasLeftPath = false;
    bool hasDownPath = false;
    
    // Visual settings
    bool showPathVisuals = false;
    
    // Grid position
    int gridX = 0;
    int gridY = 0;
    
    // Which side of the puzzle
    CellSide side = CellSide::Left;
};
