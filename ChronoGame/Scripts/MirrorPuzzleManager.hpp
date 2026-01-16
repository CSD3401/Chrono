#pragma once
#include "EngineAPI.hpp"
#include "MirrorPuzzleCell.hpp"
#include <vector>

/**
 * MirrorPuzzleManager - Manages the dual-grid mirror puzzle
 * 
 * This puzzle consists of two grids (left and right) that mirror each other.
 * A navigator exists on both sides simultaneously. When the player clicks
 * a cell adjacent to the navigator, both navigators move - the right side
 * mirrors the left side's movement (X-axis flipped).
 * 
 * Movement rules:
 * - Clicked cell must be adjacent (4-directional, no diagonals)
 * - Both cells must have valid paths connecting them
 * - Both sides (left and right) must have valid paths
 * - Movement on right mirrors the X-direction of left
 */
class MirrorPuzzleManager : public IScript {
public:
    MirrorPuzzleManager() = default;
    ~MirrorPuzzleManager() override = default;

    void Initialize(Entity entity) override {
        // Grid dimensions
        SCRIPT_FIELD(gridWidth, Int);
        SCRIPT_FIELD(gridHeight, Int);
        SCRIPT_FIELD(cellSize, Float);
        
        // Positioning
        SCRIPT_FIELD(horizontalSpacing, Float);
        SCRIPT_FIELD(verticalSpacing, Float);
        SCRIPT_FIELD(betweenGridSpacing, Float);
        
        // Navigator settings
        SCRIPT_FIELD(startX, Int);
        SCRIPT_FIELD(startY, Int);
        SCRIPT_FIELD(targetX, Int);
        SCRIPT_FIELD(targetY, Int);
        SCRIPT_FIELD(moveSpeed, Float);
        
        // Navigator entities (assign in editor)
        SCRIPT_COMPONENT_REF(navigatorLeft, EntityRef);
        SCRIPT_COMPONENT_REF(navigatorRight, EntityRef);
        SCRIPT_COMPONENT_REF(targetLeft, EntityRef);
        SCRIPT_COMPONENT_REF(targetRight, EntityRef);
        
        // Debug
        SCRIPT_FIELD(debugMode, Bool);
    }

    void Awake() override {
        s_instance = this;
        
        // Initialize navigator positions
        navigatorLeftPos = Vec2Int(startX, startY);
        navigatorRightPos = Vec2Int(gridWidth - startX - 1, startY); // Mirrored X
        
        // Initialize cell grid references
        leftGrid.resize(gridWidth * gridHeight, 0);
        rightGrid.resize(gridWidth * gridHeight, 0);
        
        puzzleSolved = false;
    }

    void Start() override {
        LOG_DEBUG("MirrorPuzzleManager started - Grid: {}x{}", gridWidth, gridHeight);
        
        // Find and register all puzzle cells
        RegisterAllCells();
        
        // Position navigators at start
        PositionNavigators();
        
        // Listen for cell click events
        // Events will be received in OnEvent()
    }

    void Update(double deltaTime) override {
        // WASD keyboard input (alternative to clicking)
        if (!puzzleSolved) {
            HandleKeyboardInput();
        }
        
        // Debug: Print current positions
        if (debugMode && Input::WasKeyPressed('P')) {
            LOG_DEBUG("Navigator Left: ({}, {})", navigatorLeftPos.x, navigatorLeftPos.y);
            LOG_DEBUG("Navigator Right: ({}, {})", navigatorRightPos.x, navigatorRightPos.y);
            LOG_DEBUG("Target: ({}, {})", targetX, targetY);
        }
        
        // Debug: Reset puzzle
        if (Input::WasKeyPressed('R')) {
            ResetPuzzle();
        }
    }

    void OnEvent(const char* eventName, void* data) override {
        if (strcmp(eventName, "CellClicked") == 0 && data != nullptr) {
            // Extract cell click data
            struct CellClickData {
                int x, y;
                MirrorPuzzleCell::CellSide side;
            };
            
            CellClickData* clickData = static_cast<CellClickData*>(data);
            
            // Handle the cell click
            HandleCellClick(clickData->x, clickData->y, clickData->side);
        }
    }

    /**
     * Register a cell in the grid
     */
    void RegisterCell(Entity cellEntity, int x, int y, MirrorPuzzleCell::CellSide side) {
        int index = y * gridWidth + x;
        
        if (side == MirrorPuzzleCell::CellSide::Left) {
            leftGrid[index] = cellEntity;
        } else {
            rightGrid[index] = cellEntity;
        }
        
        LOG_DEBUG("Registered cell at ({}, {}) on {} side", 
            x, y, side == MirrorPuzzleCell::CellSide::Left ? "LEFT" : "RIGHT");
    }

    static MirrorPuzzleManager* GetInstance() { return s_instance; }

    const char* GetTypeName() const override {
        return "MirrorPuzzleManager";
    }

private:
    struct Vec2Int {
        int x, y;
        Vec2Int(int _x = 0, int _y = 0) : x(_x), y(_y) {}
        bool operator==(const Vec2Int& other) const {
            return x == other.x && y == other.y;
        }
    };

    /**
     * Find all PuzzleCell entities in the scene and register them
     */
    void RegisterAllCells() {
        // TODO: Implement entity query system to find all cells
        // For now, cells will auto-register via events or manual setup
        LOG_DEBUG("Cell registration system initialized");
    }

    /**
     * Handle cell click and attempt to move navigator
     */
    void HandleCellClick(int clickedX, int clickedY, MirrorPuzzleCell::CellSide side) {
        if (puzzleSolved) {
            LOG_DEBUG("Puzzle already solved, ignoring input");
            return;
        }
        
        LOG_DEBUG("Cell clicked: ({}, {}) on {} side", 
            clickedX, clickedY, side == MirrorPuzzleCell::CellSide::Left ? "LEFT" : "RIGHT");
        
        // Determine which side was clicked and get positions
        Vec2Int currentPos, oppositePos;
        std::vector<Entity>* currentGridRef;
        std::vector<Entity>* oppositeGridRef;
        
        if (side == MirrorPuzzleCell::CellSide::Left) {
            currentPos = navigatorLeftPos;
            oppositePos = navigatorRightPos;
            currentGridRef = &leftGrid;
            oppositeGridRef = &rightGrid;
        } else {
            currentPos = navigatorRightPos;
            oppositePos = navigatorLeftPos;
            currentGridRef = &rightGrid;
            oppositeGridRef = &leftGrid;
        }
        
        // Check if clicked cell is adjacent (4-directional only, no diagonals)
        int dx = clickedX - currentPos.x;
        int dy = clickedY - currentPos.y;
        
        if (!IsValidAdjacency(dx, dy)) {
            LOG_DEBUG("Invalid move: Cell not adjacent (dx={}, dy={})", dx, dy);
            return;
        }
        
        // Validate paths on current side
        if (!ValidateMovement(currentPos, Vec2Int(clickedX, clickedY), dx, dy, *currentGridRef)) {
            LOG_DEBUG("Invalid move: No valid path on clicked side");
            return;
        }
        
        // Calculate mirrored position on opposite side
        int mirroredX = gridWidth - clickedX - 1;
        int mirroredY = clickedY;
        int mirroredDx = -dx; // Mirror the X direction
        int mirroredDy = dy;  // Y stays the same
        
        // Validate paths on opposite side
        if (!ValidateMovement(oppositePos, Vec2Int(mirroredX, mirroredY), mirroredDx, mirroredDy, *oppositeGridRef)) {
            LOG_DEBUG("Invalid move: No valid path on mirrored side");
            return;
        }
        
        // Valid move! Update positions
        if (side == MirrorPuzzleCell::CellSide::Left) {
            navigatorLeftPos = Vec2Int(clickedX, clickedY);
            navigatorRightPos = Vec2Int(mirroredX, mirroredY);
        } else {
            navigatorRightPos = Vec2Int(clickedX, clickedY);
            navigatorLeftPos = Vec2Int(mirroredX, mirroredY);
        }
        
        // Move navigator entities
        PositionNavigators();
        
        // Check win condition
        CheckWinCondition();
        
        LOG_DEBUG("Move successful! Left: ({}, {}), Right: ({}, {})", 
            navigatorLeftPos.x, navigatorLeftPos.y,
            navigatorRightPos.x, navigatorRightPos.y);
    }

    /**
     * Check if movement direction is valid (4-directional, no diagonals)
     */
    bool IsValidAdjacency(int dx, int dy) const {
        // Must be exactly one step in one direction
        if (dx == 0 && dy == 0) return false;  // Same cell
        if (dx != 0 && dy != 0) return false;  // Diagonal
        if (abs(dx) > 1 || abs(dy) > 1) return false;  // Too far
        return true;
    }

    /**
     * Validate that movement is allowed based on cell paths
     */
    bool ValidateMovement(Vec2Int from, Vec2Int to, int dx, int dy, std::vector<Entity>& grid) {
        // Get cells
        Entity fromEntity = GetCellAt(from.x, from.y, grid);
        Entity toEntity = GetCellAt(to.x, to.y, grid);
        
        if (fromEntity == 0 || toEntity == 0) {
            LOG_DEBUG("Invalid cells in path validation");
            return false;
        }
        
        // TODO: Get PuzzleCell component and check paths
        // For now, assume all moves are valid (you'll need to implement component access)
        // Proper implementation:
        // PuzzleCell* fromCell = GetScript<PuzzleCell>(fromEntity);
        // PuzzleCell* toCell = GetScript<PuzzleCell>(toEntity);
        // return fromCell->HasPathInDirection(dx, dy) && toCell->HasPathFromDirection(dx, dy);
        
        return true; // Placeholder
    }

    /**
     * Get cell entity at grid position
     */
    Entity GetCellAt(int x, int y, std::vector<Entity>& grid) const {
        if (x < 0 || x >= gridWidth || y < 0 || y >= gridHeight) {
            return 0;
        }
        int index = y * gridWidth + x;
        return grid[index];
    }

    /**
     * Position navigator entities at their grid positions
     */
    void PositionNavigators() {
        // Calculate world positions based on grid positions
        Vec3 leftPos = CalculateWorldPosition(navigatorLeftPos.x, navigatorLeftPos.y, true);
        Vec3 rightPos = CalculateWorldPosition(navigatorRightPos.x, navigatorRightPos.y, false);
        
        // Set navigator positions
        if (navigatorLeft != 0) {
            SetPosition(navigatorLeft, leftPos);
        }
        if (navigatorRight != 0) {
            SetPosition(navigatorRight, rightPos);
        }
    }

    /**
     * Calculate world position from grid coordinates
     */
    Vec3 CalculateWorldPosition(int gridX, int gridY, bool isLeftSide) const {
        float originY = -((gridHeight - 1) * verticalSpacing) / 2.0f;
        
        if (isLeftSide) {
            float originX = -((gridWidth - 1) * horizontalSpacing) - (betweenGridSpacing / 2.0f);
            return Vec3(
                originX + gridX * horizontalSpacing,
                originY + gridY * verticalSpacing,
                0.0f
            );
        } else {
            float originX = betweenGridSpacing / 2.0f;
            return Vec3(
                originX + gridX * horizontalSpacing,
                originY + gridY * verticalSpacing,
                0.0f
            );
        }
    }

    /**
     * Handle keyboard input (WASD)
     */
    void HandleKeyboardInput() {
        int dx = 0, dy = 0;
        
        if (Input::WasKeyPressed('W') || Input::WasKeyPressed(VK_UP)) {
            dy = 1;
        } else if (Input::WasKeyPressed('S') || Input::WasKeyPressed(VK_DOWN)) {
            dy = -1;
        } else if (Input::WasKeyPressed('A') || Input::WasKeyPressed(VK_LEFT)) {
            dx = -1;
        } else if (Input::WasKeyPressed('D') || Input::WasKeyPressed(VK_RIGHT)) {
            dx = 1;
        }
        
        if (dx != 0 || dy != 0) {
            int targetX = navigatorLeftPos.x + dx;
            int targetY = navigatorLeftPos.y + dy;
            
            // Treat keyboard input as clicking the target cell on left side
            HandleCellClick(targetX, targetY, MirrorPuzzleCell::CellSide::Left);
        }
    }

    /**
     * Check if puzzle is solved
     */
    void CheckWinCondition() {
        if (navigatorLeftPos.x == targetX && navigatorLeftPos.y == targetY) {
            puzzleSolved = true;
            LOG_DEBUG("PUZZLE SOLVED! 🎉");
            Events::Send("MirrorPuzzleSolved", nullptr);
        }
    }

    /**
     * Reset puzzle to start state
     */
    void ResetPuzzle() {
        navigatorLeftPos = Vec2Int(startX, startY);
        navigatorRightPos = Vec2Int(gridWidth - startX - 1, startY);
        puzzleSolved = false;
        PositionNavigators();
        LOG_DEBUG("Puzzle reset");
    }

    // Grid settings
    int gridWidth = 4;
    int gridHeight = 4;
    float cellSize = 1.0f;
    float horizontalSpacing = 1.0f;
    float verticalSpacing = 1.0f;
    float betweenGridSpacing = 2.0f;
    
    // Navigator settings
    int startX = 0;
    int startY = 0;
    int targetX = 3;
    int targetY = 3;
    float moveSpeed = 5.0f;
    
    // Entity references
    Entity navigatorLeft = 0;
    Entity navigatorRight = 0;
    Entity targetLeft = 0;
    Entity targetRight = 0;
    
    // State
    Vec2Int navigatorLeftPos;
    Vec2Int navigatorRightPos;
    bool puzzleSolved = false;
    bool debugMode = true;
    
    // Cell grids
    std::vector<Entity> leftGrid;
    std::vector<Entity> rightGrid;
    
    // Singleton
    static inline MirrorPuzzleManager* s_instance = nullptr;
};
