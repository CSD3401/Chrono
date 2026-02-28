#pragma once
#include "EngineAPI.hpp"

#define GLFW_MOUSE_BUTTON_LEFT 0

/**
 * Cutscene_Controller
 *
 * Displays a series of cutscene "pages" (UI image GameObjects) one at a time.
 * Each page has a matching caption shown on a UIText entity at the bottom of
 * the screen. A left mouse click advances to the next page; the cutscene ends
 * and hides itself when all pages are done.
 *
 * ── Editor Setup ────────────────────────────────────────────────────────────
 *  1. Create a parent GameObject for this script (e.g. "CutsceneManager").
 *
 *  2. Assign eventName (String) — the event that triggers this cutscene.
 *     Leave blank / default to auto-catch any mistake (see Start()).
 *     Example: "StartCutscene_Intro"
 *
 *  3. Build your pages in the scene as separate UI Image GameObjects
 *     (one per page). Set them all INACTIVE in the editor — the script
 *     activates them one at a time at runtime.
 *     Drag them into the pageImages list in order (page 1 → page 2 → …).
 *
 *  4. Create ONE UIText GameObject for the caption bar (position it at the
 *     bottom of your canvas however you like in the editor).
 *     Set it INACTIVE in the editor.
 *     Drag it into captionTextRef.
 *
 *  5. Fill in the pageCaptions string vector — one entry per page, in the
 *     same order as pageImages.  Leave an entry as "" for no caption.
 *
 *  6. Fire the event from any other script to start the cutscene:
 *       Events::Send("StartCutscene_Intro");
 *
 * ── Auto-Advance ─────────────────────────────────────────────────────────────
 *  - Toggle autoAdvance ON in the inspector to enable timed page progression.
 *  - autoAdvanceDelay controls how many seconds each page stays visible
 *    before automatically moving to the next (default: 2.0s).
 *  - A left-click still advances immediately even when autoAdvance is ON,
 *    and resets the timer so the new page gets its full delay.
 *  - The ignoreNextClick guard still applies on cutscene open.
 *
 * ── Behaviour ────────────────────────────────────────────────────────────────
 *  - Receiving the event shows page 0 and its caption.
 *  - Each left-click advances to the next page (with a 1-frame click guard
 *    so the opening click never skips page 0).
 *  - After the last page the cutscene hides everything and fires
 *    "CutsceneDone_<eventName>" so other systems can react.
 */
class Cutscene_Controller : public IScript {
public:
    Cutscene_Controller() {
        // ── Inspector fields ────────────────────────────────────────────────
        SCRIPT_FIELD(eventName, String);  // event that triggers this cutscene
        SCRIPT_FIELD(autoAdvance, Bool);    // enable timed auto page flip
        SCRIPT_FIELD(autoAdvanceDelay, Float);   // seconds per page (default 2.0)
        RegisterStringVectorField("pageCaptions", &pageCaptions);
        SCRIPT_GAMEOBJECT_REF(captionTextRef);   // UIText GameObject
        RegisterGameObjectRefVectorField("pageImages", &pageImages);
    }

    ~Cutscene_Controller() override = default;

    // ── Lifecycle ────────────────────────────────────────────────────────────

    void Awake() override {}

    void Initialize(Entity entity) override {
        (void)entity;
    }

    void Start() override {
        // ── Failsafe: warn if eventName was never set ─────────────────────
        if (eventName.empty() || eventName == "emptyEvent") {
            LOG_ERROR("Cutscene_Controller: eventName is not set (defaulting to "
                "'emptyEvent'). Assign a real event name in the inspector "
                "or this cutscene will never trigger!");
            eventName = "emptyEvent";
        }

        // ── Validate page data ────────────────────────────────────────────
        if (pageImages.empty()) {
            LOG_WARNING("Cutscene_Controller [" << eventName
                << "]: pageImages list is empty — cutscene has no pages!");
        }

        if (pageCaptions.size() < pageImages.size()) {
            LOG_WARNING("Cutscene_Controller [" << eventName
                << "]: pageCaptions has fewer entries (" << pageCaptions.size()
                << ") than pageImages (" << pageImages.size()
                << "). Missing captions will show as blank.");
        }

        // ── Clamp autoAdvanceDelay to a sensible minimum ──────────────────
        // Catches the case where the field was left at 0 in the inspector
        if (autoAdvanceDelay <= 0.0f) {
            autoAdvanceDelay = 2.0f;
        }

        // ── Register event listener ───────────────────────────────────────
        Events::Listen(eventName.c_str(), [this](void* /*data*/) {
            TriggerCutscene();
            });

        // ── Make sure every page starts hidden ───────────────────────────
        HideAllPages();
        SetCaptionVisible(false);

        isPlaying = false;
        currentPageIndex = 0;
        ignoreNextClick = false;
        pageTimer = 0.0f;

        LOG_DEBUG("Cutscene_Controller [" << eventName << "]: Ready with "
            << pageImages.size() << " page(s)."
            << (autoAdvance
                ? " Auto-advance ON (" + std::to_string(autoAdvanceDelay) + "s)"
                : " Auto-advance OFF"));
    }

    void Update(double deltaTime) override {
        if (!isPlaying) return;

        // ── Auto-advance timer ────────────────────────────────────────────
        // Counts up every frame. When it hits the delay threshold the page
        // flips automatically regardless of any click input.
        if (autoAdvance) {
            pageTimer += static_cast<float>(deltaTime);
            if (pageTimer >= autoAdvanceDelay) {
                pageTimer = 0.0f;
                ignoreNextClick = false; // clear guard so clicks work on new page
                AdvancePage();
                return; // skip click check this frame so we don't double-advance
            }
        }

        // ── Manual left-click advance ─────────────────────────────────────
        if (Input::WasMousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
            // Guard: ignore the same click that opened the cutscene
            if (ignoreNextClick) {
                ignoreNextClick = false;
                return;
            }
            // Reset timer so the new page gets its full autoAdvanceDelay
            pageTimer = 0.0f;
            AdvancePage();
        }
    }

    void OnDestroy() override {
        if (isPlaying) {
            HideAllPages();
            SetCaptionVisible(false);
        }
    }

    void OnEnable()   override {}
    void OnDisable()  override {}
    void OnValidate() override {}
    const char* GetTypeName() const override { return "Cutscene_Controller"; }

    void OnCollisionEnter(Entity other) override { (void)other; }
    void OnCollisionExit(Entity other) override { (void)other; }
    void OnCollisionStay(Entity other) override { (void)other; }
    void OnTriggerEnter(Entity other) override { (void)other; }
    void OnTriggerExit(Entity other) override { (void)other; }
    void OnTriggerStay(Entity other) override { (void)other; }

private:
    // ── Inspector fields ──────────────────────────────────────────────────
    std::string                eventName = "emptyEvent";
    bool                       autoAdvance = true;  // enable timed page flip
    float                      autoAdvanceDelay = 2.0f;   // seconds per page
    std::vector<GameObjectRef> pageImages;
    GameObjectRef              captionTextRef;
    std::vector<std::string>   pageCaptions;

    // ── Runtime state ─────────────────────────────────────────────────────
    bool  isPlaying = false;
    int   currentPageIndex = 0;
    bool  ignoreNextClick = false;
    float pageTimer = 0.0f;  // accumulates deltaTime while page is visible


    /** Called when the trigger event fires — starts the cutscene from page 0. */
    void TriggerCutscene() {
        if (pageImages.empty()) {
            LOG_ERROR("Cutscene_Controller [" << eventName
                << "]: Cannot start — pageImages list is empty!");
            return;
        }

        LOG_INFO("Cutscene_Controller [" << eventName << "]: Starting.");
        isPlaying = true;
        currentPageIndex = 0;
        ignoreNextClick = true;
        pageTimer = 0.0f;

        ShowPage(currentPageIndex);
    }

    /** Hides the current page, shows the next one, or ends the cutscene. */
    void AdvancePage() {
        HidePage(currentPageIndex);

        ++currentPageIndex;

        if (currentPageIndex < static_cast<int>(pageImages.size())) {
            ShowPage(currentPageIndex);
        }
        else {
            EndCutscene();
        }
    }

    /** Activates a page image and updates the caption text. */
    void ShowPage(int index) {
        if (index < 0 || index >= static_cast<int>(pageImages.size())) return;

        if (pageImages[index].IsValid()) {
            SetActive(true, pageImages[index].GetEntity());
        }
        else {
            LOG_WARNING("Cutscene_Controller [" << eventName
                << "]: pageImages[" << index << "] is not a valid reference!");
        }

        const std::string& caption = (index < static_cast<int>(pageCaptions.size()))
            ? pageCaptions[index]
            : "";
        UpdateCaption(caption);
    }

    /** Deactivates a single page image (leaves caption alone). */
    void HidePage(int index) {
        if (index < 0 || index >= static_cast<int>(pageImages.size())) return;
        if (pageImages[index].IsValid()) {
            SetActive(false, pageImages[index].GetEntity());
        }
    }

    /** Deactivates every page image at once. */
    void HideAllPages() {
        for (auto& page : pageImages) {
            if (page.IsValid()) {
                SetActive(false, page.GetEntity());
            }
        }
    }

    /** Shows or hides the caption UIText entity. */
    void SetCaptionVisible(bool visible) {
        if (captionTextRef.IsValid()) {
            SetActive(visible, captionTextRef.GetEntity());
        }
    }

    /**
     * Writes text to the caption UIText component and ensures it is visible.
     * If caption is empty the text bar is hidden instead.
     */
    void UpdateCaption(const std::string& text) {
        if (!captionTextRef.IsValid()) return;

        if (text.empty()) {
            SetCaptionVisible(false);
            return;
        }

        NE::Scripting::SetUIText(captionTextRef.GetEntity(), text.c_str());
        SetCaptionVisible(true);
    }

    /** Cleans up after the last page and fires the "done" event. */
    void EndCutscene() {
        isPlaying = false;
        HideAllPages();
        SetCaptionVisible(false);

        std::string doneEvent = "CutsceneDone_" + eventName;
        Events::Send(doneEvent.c_str());

        LOG_INFO("Cutscene_Controller [" << eventName << "]: Finished. "
            "Fired event: " << doneEvent);
    }
};