#pragma once

#include <functional>

#include "GuiComponent.h"
#include "Window.h"
#include "components/ImageComponent.h"
#include "components/PillChipComponent.h"
#include "components/PillHelpBarComponent.h"
#include "components/DotsIndicatorComponent.h"
#include "components/TextListComponent.h"
#include "SaveState.h"

class FileData;
class SaveStateRepository;

enum class SaveStatePillActionType
{
	Continue,
	Start,
	StartAutoSave,
	Delete
};

// Full-screen, pill-styled save-state selector shown instead of GuiSaveState when the pill-style
// UI (SystemViewStyle == "list") is enabled. A dimmed full-screen screenshot of the currently
// highlighted save state sits behind a top-left game-name pill, a pill-styled action list
// (CONTINUE / START / START (AUTO SAVE) / DELETE, filtered by availability), and a row of dots -
// one per real save state - browsed with left/right while up/down moves through the action list.
class GuiSaveStatePillView : public GuiComponent
{
public:
	GuiSaveStatePillView(Window* window, FileData* game, const std::function<void(SaveState* state)>& callback);

	bool input(InputConfig* config, Input input) override;
	void update(int deltaTime) override;
	void render(const Transform4x4f& parentTrans) override;
	std::vector<HelpPrompt> getHelpPrompts() override;

private:
	void onDotIndexChanged(int newIndex);
	void reloadBackgroundImage();
	void rebuildDotsAndSaveStates();
	void rebuildActionList();
	void runAction(SaveStatePillActionType action);
	void confirmDelete();

	FileData* mGame;
	SaveStateRepository* mRepository;
	std::function<void(SaveState* state)> mRunCallback;

	ImageComponent mBackgroundImage;
	PillChipComponent mNamePill;
	TextListComponent<SaveStatePillActionType> mActionList;
	DotsIndicatorComponent mDotsIndicator;
	PillHelpBarComponent mHelpBar;

	std::vector<SaveState*> mSaveStates; // real states only, sorted; no virtual placeholders
	int mDotIndex;
	SaveState* mCurrentSaveState; // mSaveStates[mDotIndex], or nullptr if mSaveStates is empty
};
