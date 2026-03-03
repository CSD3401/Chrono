#pragma once
#include "EngineAPI.hpp"
#include <array>
#include "Misc_MirrorPuzzleCellData.hpp"

/**
 * MirrorPuzzle - Dual-grid puzzle with mirrored movement (3x4 grid)
 *
 * Setup in editor:
 * 1. Assign all 12 original tiles to tile00..tile23
 * 2. Assign all 12 mirror tiles to mirrorTile00..mirrorTile23
 * 3. Each tile entity should have a Misc_MirrorPuzzleCellData script
 *    with tileRow, tileCol, UP, DOWN, LEFT, RIGHT set in the inspector
 * 4. Configure startRow/Col, endRow/Col
 */
class MirrorPuzzle : public IScript {
public:
    enum Direction : uint8_t {
        NONE = 0,
        UP = 1 << 0,
        DOWN = 1 << 1,
        LEFT = 1 << 2,
        RIGHT = 1 << 3,
        ALL = UP | DOWN | LEFT | RIGHT
    };

    MirrorPuzzle() {
        SCRIPT_GAMEOBJECT_REF(mazeServerDoor);

        SCRIPT_FIELD(originalMaterial, MaterialRef);
        SCRIPT_FIELD(steppedOnMaterial, MaterialRef);
        SCRIPT_FIELD(destinationMaterial, MaterialRef);

        SCRIPT_COMPONENT_REF(tile00, TransformRef);
        SCRIPT_COMPONENT_REF(tile01, TransformRef);
        SCRIPT_COMPONENT_REF(tile02, TransformRef);
        SCRIPT_COMPONENT_REF(tile03, TransformRef);
        SCRIPT_COMPONENT_REF(tile10, TransformRef);
        SCRIPT_COMPONENT_REF(tile11, TransformRef);
        SCRIPT_COMPONENT_REF(tile12, TransformRef);
        SCRIPT_COMPONENT_REF(tile13, TransformRef);
        SCRIPT_COMPONENT_REF(tile20, TransformRef);
        SCRIPT_COMPONENT_REF(tile21, TransformRef);
        SCRIPT_COMPONENT_REF(tile22, TransformRef);
        SCRIPT_COMPONENT_REF(tile23, TransformRef);

        SCRIPT_COMPONENT_REF(mirrorTile00, TransformRef);
        SCRIPT_COMPONENT_REF(mirrorTile01, TransformRef);
        SCRIPT_COMPONENT_REF(mirrorTile02, TransformRef);
        SCRIPT_COMPONENT_REF(mirrorTile03, TransformRef);
        SCRIPT_COMPONENT_REF(mirrorTile10, TransformRef);
        SCRIPT_COMPONENT_REF(mirrorTile11, TransformRef);
        SCRIPT_COMPONENT_REF(mirrorTile12, TransformRef);
        SCRIPT_COMPONENT_REF(mirrorTile13, TransformRef);
        SCRIPT_COMPONENT_REF(mirrorTile20, TransformRef);
        SCRIPT_COMPONENT_REF(mirrorTile21, TransformRef);
        SCRIPT_COMPONENT_REF(mirrorTile22, TransformRef);
        SCRIPT_COMPONENT_REF(mirrorTile23, TransformRef);
    }

    ~MirrorPuzzle() override = default;

    void Initialize(Entity entity) override {
        SCRIPT_FIELD(startRow, Int);
        SCRIPT_FIELD(startCol, Int);
        SCRIPT_FIELD(endRow, Int);
        SCRIPT_FIELD(endCol, Int);
        SCRIPT_FIELD(zOffset, Float);
        SCRIPT_FIELD(eventName, String);
        SCRIPT_FIELD(debugMode, Bool);
    }

    void Awake() override {
        LOG_DEBUG("MirrorPuzzle Awake");
    }

    void Start() override {
        LOG_DEBUG("=== MirrorPuzzle Started ===");

        startRow = Clamp(startRow, 0, 2);
        startCol = Clamp(startCol, 0, 3);
        endRow = Clamp(endRow, 0, 2);
        endCol = Clamp(endCol, 0, 3);

        mirrorStartRow = startRow;
        mirrorStartCol = 3 - startCol;
        mirrorEndRow = endRow;
        mirrorEndCol = 3 - endCol;

        LOG_DEBUG("Original: Start({}, {}) -> End({}, {})", startRow, startCol, endRow, endCol);
        LOG_DEBUG("Mirror:   Start({}, {}) -> End({}, {})", mirrorStartRow, mirrorStartCol, mirrorEndRow, mirrorEndCol);

        CacheTileReferences();
        ApplyTileRestrictions();

        currentRow = startRow;
        currentCol = startCol;
        mirrorRow = mirrorStartRow;
        mirrorCol = mirrorStartCol;

        HighlightTiles();

        if (destinationMaterial.IsValid()) {
            int goalIndex = endRow * 4 + endCol;
            if (tileTransforms[goalIndex].IsValid()) {
                Entity goalTile = tileTransforms[goalIndex].GetEntity();
                NE::Renderer::Command::AssignMaterial(goalTile, destinationMaterial);
            }
            int mirrorGoalIndex = mirrorEndRow * 4 + mirrorEndCol;
            if (mirrorTileTransforms[mirrorGoalIndex].IsValid()) {
                Entity mirrorGoalTile = mirrorTileTransforms[mirrorGoalIndex].GetEntity();
                NE::Renderer::Command::AssignMaterial(mirrorGoalTile, destinationMaterial);
            }
        }

        puzzleSolved = false;
        LogCurrentState();
    }

    void Update(double deltaTime) override {
        if (puzzleSolved) return;

        if (Input::WasKeyPressed('I')) {
            PlayAudio("event:/COLOR_CLICK");
            TryMoveUp();
        }
        if (Input::WasKeyPressed('K')) {
            PlayAudio("event:/COLOR_CLICK");
            TryMoveDown();
        }
        if (Input::WasKeyPressed('J')) {
            PlayAudio("event:/COLOR_CLICK");
            TryMoveLeft();
        }
        if (Input::WasKeyPressed('L')) {
            PlayAudio("event:/COLOR_CLICK");
            TryMoveRight();
        }

        if (HasReachedEnd() && HasMirrorReachedEnd()) {
            if (!puzzleSolved) {
                puzzleSolved = true;
                LOG_DEBUG("=== PUZZLE SOLVED! ===");
                if (!eventName.empty()) {
                    Events::Send(eventName.c_str(), nullptr);
                    PlayAudio("event:/DOOR_OPEN");
                }
                SetActive(true, mazeServerDoor.GetEntity());
            }
        }

        if (debugMode && Input::WasKeyPressed('R')) {
            ResetPuzzle();
        }
    }

    const char* GetTypeName() const override { return "MirrorPuzzle"; }

    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }
    void OnDestroy() override {}
    void OnEnable() override {}
    void OnDisable() override {}
    void OnValidate() override {}

private:
    int Clamp(int val, int min, int max) const {
        if (val < min) return min;
        if (val > max) return max;
        return val;
    }

    Vec3 GetTileWorldPosition(const TransformRef& tileRef) const {
        if (!tileRef.IsValid()) return Vec3(0, 0, 0);
        return GetPosition(tileRef);
    }

    void CacheTileReferences() {
        tileTransforms[0] = tile00;  tileTransforms[1] = tile01;
        tileTransforms[2] = tile02;  tileTransforms[3] = tile03;
        tileTransforms[4] = tile10;  tileTransforms[5] = tile11;
        tileTransforms[6] = tile12;  tileTransforms[7] = tile13;
        tileTransforms[8] = tile20;  tileTransforms[9] = tile21;
        tileTransforms[10] = tile22; tileTransforms[11] = tile23;

        mirrorTileTransforms[0] = mirrorTile00;  mirrorTileTransforms[1] = mirrorTile01;
        mirrorTileTransforms[2] = mirrorTile02;  mirrorTileTransforms[3] = mirrorTile03;
        mirrorTileTransforms[4] = mirrorTile10;  mirrorTileTransforms[5] = mirrorTile11;
        mirrorTileTransforms[6] = mirrorTile12;  mirrorTileTransforms[7] = mirrorTile13;
        mirrorTileTransforms[8] = mirrorTile20;  mirrorTileTransforms[9] = mirrorTile21;
        mirrorTileTransforms[10] = mirrorTile22; mirrorTileTransforms[11] = mirrorTile23;
    }

    void ClearAllRestrictions() {
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 4; col++) {
                grid[row][col] = NONE;
                mirrorGrid[row][col] = NONE;
                UpdateTileIndicators(row, col, NONE, false);
                UpdateTileIndicators(row, col, NONE, true);
            }
        }
        LOG_DEBUG("All tile restrictions cleared");
    }

    /**
     * Read Misc_MirrorPuzzleCellData from a tile entity and apply its restrictions.
     */
    bool ReadAndApplyCellData(const TransformRef& tileRef, bool isMirror) {
        if (!tileRef.IsValid()) return false;

        Entity tileEntity = tileRef.GetEntity();
        NE::Scripting::GameObject go(tileRef);
        auto* cellData = go.GetComponent<Misc_MirrorPuzzleCellData>();

        if (!cellData) return false;

        int row = cellData->tileRow;
        int col = cellData->tileCol;

        if (row < 0 || row > 2 || col < 0 || col > 3) {
            LOG_WARNING("MirrorPuzzle: CellData on entity {} has invalid row/col ({}, {})",
                tileEntity, row, col);
            return false;
        }

        if (isMirror) {
            SetMirrorTileRestriction(row, col,
                cellData->UP, cellData->DOWN, cellData->LEFT, cellData->RIGHT);
        }
        else {
            SetTileRestriction(row, col,
                cellData->UP, cellData->DOWN, cellData->LEFT, cellData->RIGHT);
        }

        if (debugMode) {
            LOG_DEBUG("CellData: {} tile ({}, {}) U={} D={} L={} R={}",
                isMirror ? "mirror" : "original", row, col,
                cellData->UP, cellData->DOWN, cellData->LEFT, cellData->RIGHT);
        }

        return true;
    }

    /**
     * Apply tile restrictions by reading Misc_MirrorPuzzleCellData
     * from each tile entity instead of hardcoding.
     */
    void ApplyTileRestrictions() {
        ClearAllRestrictions();

        int originalCount = 0;
        int mirrorCount = 0;

        for (int i = 0; i < 12; i++) {
            if (ReadAndApplyCellData(tileTransforms[i], false))
                originalCount++;
        }

        for (int i = 0; i < 12; i++) {
            if (ReadAndApplyCellData(mirrorTileTransforms[i], true))
                mirrorCount++;
        }

        LOG_DEBUG("ApplyTileRestrictions: {} original, {} mirror tiles configured from CellData",
            originalCount, mirrorCount);

        if (originalCount == 0 && mirrorCount == 0) {
            LOG_WARNING("MirrorPuzzle: No Misc_MirrorPuzzleCellData found on any tiles! "
                "Attach CellData scripts to tile entities in the editor.");
        }
    }

    void SetTileRestriction(int row, int col, bool up, bool down, bool left, bool right) {
        if (row >= 0 && row < 3 && col >= 0 && col < 4) {
            Direction allowed = NONE;
            if (up)    allowed = static_cast<Direction>(allowed | UP);
            if (down)  allowed = static_cast<Direction>(allowed | DOWN);
            if (left)  allowed = static_cast<Direction>(allowed | LEFT);
            if (right) allowed = static_cast<Direction>(allowed | RIGHT);
            grid[row][col] = allowed;
            UpdateTileIndicators(row, col, allowed, false);
        }
    }

    void SetMirrorTileRestriction(int row, int col, bool up, bool down, bool left, bool right) {
        if (row >= 0 && row < 3 && col >= 0 && col < 4) {
            Direction allowed = NONE;
            if (up)    allowed = static_cast<Direction>(allowed | UP);
            if (down)  allowed = static_cast<Direction>(allowed | DOWN);
            if (left)  allowed = static_cast<Direction>(allowed | LEFT);
            if (right) allowed = static_cast<Direction>(allowed | RIGHT);
            mirrorGrid[row][col] = allowed;
            UpdateTileIndicators(row, col, allowed, true);
        }
    }

    void UpdateTileIndicators(int row, int col, Direction allowed, bool isMirror) {
        int tileIndex = row * 4 + col;
        const TransformRef& tileRef = isMirror ? mirrorTileTransforms[tileIndex] : tileTransforms[tileIndex];

        if (!tileRef.IsValid()) return;

        Entity tileEntity = tileRef.GetEntity();
        size_t childCount = GetChildCount(tileEntity);
        if (childCount < 4) {
            if (debugMode)
                LOG_DEBUG("Tile ({}, {}) has {} children, need 4 for arrows", row, col, childCount);
            return;
        }

        Entity upInd = GetChild(0, tileEntity);
        Entity rightInd = GetChild(1, tileEntity);
        Entity downInd = GetChild(2, tileEntity);
        Entity leftInd = GetChild(3, tileEntity);

        if (upInd != 0)    SetActive((allowed & UP) != 0, upInd);
        if (rightInd != 0) SetActive((allowed & RIGHT) != 0, rightInd);
        if (downInd != 0)  SetActive((allowed & DOWN) != 0, downInd);
        if (leftInd != 0)  SetActive((allowed & LEFT) != 0, leftInd);
    }

    void HighlightTiles() {
        if (lastOriginalTile != 0 && originalMaterial.IsValid())
            NE::Renderer::Command::AssignMaterial(lastOriginalTile, originalMaterial);
        if (lastMirrorTile != 0 && originalMaterial.IsValid())
            NE::Renderer::Command::AssignMaterial(lastMirrorTile, originalMaterial);

        int tileIndex = currentRow * 4 + currentCol;
        if (tileTransforms[tileIndex].IsValid()) {
            Entity tileEntity = tileTransforms[tileIndex].GetEntity();
            lastOriginalTile = tileEntity;
            if (steppedOnMaterial.IsValid())
                NE::Renderer::Command::AssignMaterial(tileEntity, steppedOnMaterial);
        }

        int mirrorTileIndex = mirrorRow * 4 + mirrorCol;
        if (mirrorTileTransforms[mirrorTileIndex].IsValid()) {
            Entity mirrorTileEntity = mirrorTileTransforms[mirrorTileIndex].GetEntity();
            lastMirrorTile = mirrorTileEntity;
            if (steppedOnMaterial.IsValid())
                NE::Renderer::Command::AssignMaterial(mirrorTileEntity, steppedOnMaterial);
        }
    }

    // ========== Movement ==========

    void TryMoveUp() {
        bool o = TryMoveOriginal(UP, -1, 0);
        bool m = TryMoveMirror(UP, -1, 0);
        if (o || m) { HighlightTiles(); LogCurrentState(); }
    }

    void TryMoveDown() {
        bool o = TryMoveOriginal(DOWN, 1, 0);
        bool m = TryMoveMirror(DOWN, 1, 0);
        if (o || m) { HighlightTiles(); LogCurrentState(); }
    }

    void TryMoveLeft() {
        bool o = TryMoveOriginal(LEFT, 0, -1);
        bool m = TryMoveMirror(RIGHT, 0, 1);
        if (o || m) { HighlightTiles(); LogCurrentState(); }
    }

    void TryMoveRight() {
        bool o = TryMoveOriginal(RIGHT, 0, 1);
        bool m = TryMoveMirror(LEFT, 0, -1);
        if (o || m) { HighlightTiles(); LogCurrentState(); }
    }

    bool TryMoveOriginal(Direction dir, int rowDelta, int colDelta) {
        if (!CanMoveInDirection(grid[currentRow][currentCol], dir)) return false;
        int newRow = currentRow + rowDelta;
        int newCol = currentCol + colDelta;
        if (newRow < 0 || newRow > 2 || newCol < 0 || newCol > 3) return false;
        if (!CanMoveInDirection(grid[newRow][newCol], GetOppositeDirection(dir))) return false;
        currentRow = newRow;
        currentCol = newCol;
        if (debugMode) LOG_DEBUG("Original moved to ({}, {})", currentRow, currentCol);
        return true;
    }

    bool TryMoveMirror(Direction dir, int rowDelta, int colDelta) {
        if (!CanMoveInDirection(mirrorGrid[mirrorRow][mirrorCol], dir)) return false;
        int newRow = mirrorRow + rowDelta;
        int newCol = mirrorCol + colDelta;
        if (newRow < 0 || newRow > 2 || newCol < 0 || newCol > 3) return false;
        if (!CanMoveInDirection(mirrorGrid[newRow][newCol], GetOppositeDirection(dir))) return false;
        mirrorRow = newRow;
        mirrorCol = newCol;
        if (debugMode) LOG_DEBUG("Mirror moved to ({}, {})", mirrorRow, mirrorCol);
        return true;
    }

    bool CanMoveInDirection(Direction tile, Direction dir) const {
        return (static_cast<uint8_t>(tile) & static_cast<uint8_t>(dir)) != 0;
    }

    Direction GetOppositeDirection(Direction dir) const {
        switch (dir) {
        case UP:    return DOWN;
        case DOWN:  return UP;
        case LEFT:  return RIGHT;
        case RIGHT: return LEFT;
        default:    return NONE;
        }
    }

    bool HasReachedEnd() const { return currentRow == endRow && currentCol == endCol; }
    bool HasMirrorReachedEnd() const { return mirrorRow == mirrorEndRow && mirrorCol == mirrorEndCol; }

    void ResetPuzzle() {
        currentRow = startRow;  currentCol = startCol;
        mirrorRow = mirrorStartRow;  mirrorCol = mirrorStartCol;
        puzzleSolved = false;
        HighlightTiles();
        LogCurrentState();
        LOG_DEBUG("Puzzle reset!");
    }

    void LogCurrentState() const {
        LOG_DEBUG("Original: ({}, {}) -> Goal({}, {})", currentRow, currentCol, endRow, endCol);
        LOG_DEBUG("Mirror:   ({}, {}) -> Goal({}, {})", mirrorRow, mirrorCol, mirrorEndRow, mirrorEndCol);
    }

    // ========== Fields ==========
    GameObjectRef mazeServerDoor;
    MaterialRef originalMaterial;
    MaterialRef steppedOnMaterial;
    MaterialRef destinationMaterial;
    Entity lastOriginalTile = 0;
    Entity lastMirrorTile = 0;

    TransformRef tile00, tile01, tile02, tile03;
    TransformRef tile10, tile11, tile12, tile13;
    TransformRef tile20, tile21, tile22, tile23;
    TransformRef mirrorTile00, mirrorTile01, mirrorTile02, mirrorTile03;
    TransformRef mirrorTile10, mirrorTile11, mirrorTile12, mirrorTile13;
    TransformRef mirrorTile20, mirrorTile21, mirrorTile22, mirrorTile23;

    int startRow = 2;  int startCol = 0;
    int endRow = 0;    int endCol = 3;
    float zOffset = 0.2f;
    std::string eventName = "MirrorPuzzleSolved";
    bool debugMode = true;

    int mirrorStartRow = 0;  int mirrorStartCol = 0;
    int mirrorEndRow = 0;    int mirrorEndCol = 0;
    int currentRow = 0;      int currentCol = 0;
    int mirrorRow = 0;       int mirrorCol = 0;
    bool puzzleSolved = false;

    std::array<std::array<Direction, 4>, 3> grid;
    std::array<std::array<Direction, 4>, 3> mirrorGrid;
    std::array<TransformRef, 12> tileTransforms;
    std::array<TransformRef, 12> mirrorTileTransforms;
};