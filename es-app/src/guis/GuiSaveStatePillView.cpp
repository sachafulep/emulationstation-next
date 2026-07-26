#include "guis/GuiSaveStatePillView.h"

#include <algorithm>
#include "FileData.h"
#include "SystemData.h"
#include "SystemConf.h"
#include "LocaleES.h"
#include "guis/GuiMsgBox.h"
#include "SaveStateRepository.h"
#include "components/PillMetrics.h"
#include "components/PillListStyle.h"

namespace
{
	constexpr float kMargin = 24.0f;
	constexpr float kActionListGap = 12.0f;
	constexpr float kActionListWidth = 480.0f;
	// Dots sit just above the help bar's safe area (i.e. above the bottom corner pills):
	// dot diameter (8) + a comfortable gap above the safe area's bottom edge.
	constexpr float kDotsSafeGap = 24.0f;
	constexpr unsigned int kOverlayColor = 0x000000A0;
}

GuiSaveStatePillView::GuiSaveStatePillView(Window* window, FileData* game, const std::function<void(SaveState* state)>& callback) :
	GuiComponent(window), mBackgroundImage(window), mNamePill(window), mActionList(window), mDotsIndicator(window), mHelpBar(window),
	mDotIndex(0), mCurrentSaveState(nullptr)
{
	mGame = game;
	mRepository = game->getSourceFileData()->getSystem()->getSaveStateRepository();
	mRunCallback = callback;

	setSize((float)Renderer::getScreenWidth(), (float)Renderer::getScreenHeight());

	mBackgroundImage.setPosition(0.0f, 0.0f);
	mBackgroundImage.setMinSize(mSize.x(), mSize.y());

	// Match the action list's own selector-pill padding (TextListComponent.h's padH = font size *
	// 0.6, at the same kFontSizeLarge both pills use) so the name pill and the list's pill look
	// like they belong to the same family.
	mNamePill.setColors(0x000000FF, 0xFFFFFFFF);
	mNamePill.setFont(Font::get((int)PillMetrics::kFontSizeLarge, PillMetrics::kFontPath));
	mNamePill.setHorizontalPadding(16.0f);
	mNamePill.setFixedHeight(PillMetrics::kOuterPillHeight);
	mNamePill.setText(mGame->getDisplayName());
	mNamePill.setPosition(kMargin, kMargin);

	PillListStyle::configure(&mActionList, mSize);
	mActionList.setPosition(kMargin, kMargin + PillMetrics::kOuterPillHeight + kActionListGap);
	mActionList.setSize(kActionListWidth, PillMetrics::kOuterPillHeight * 4.0f);

	Vector2f safeAreaPosition, safeAreaSize;
	float topRowMaxRight = 0.0f;
	mHelpBar.layout(mSize, safeAreaPosition, safeAreaSize, topRowMaxRight, true, _("OKAY"));

	float dotsY = safeAreaPosition.y() + safeAreaSize.y() - kDotsSafeGap;
	mDotsIndicator.setCenteredPosition(mSize.x() / 2.0f, dotsY);

	rebuildDotsAndSaveStates();
	rebuildActionList();
}

void GuiSaveStatePillView::rebuildDotsAndSaveStates()
{
	mSaveStates = mRepository->getSaveStates(mGame);

	bool supportsIncrementalSaveStates = SystemConf::getIncrementalSaveStates();
	bool incrementalSaveStates = supportsIncrementalSaveStates && mRepository->supportsIncrementalSaveStates();

	std::sort(mSaveStates.begin(), mSaveStates.end(), [&, supportsIncrementalSaveStates, incrementalSaveStates](const SaveState* file1, const SaveState* file2)
		{
			if (file1->config != nullptr && file2->config != nullptr && !file1->config->equals(file2->config))
				return file1->config->isActiveConfig(mGame);

			if (supportsIncrementalSaveStates && file1->config != nullptr ? file1->config->incremental : incrementalSaveStates)
				return file1->creationDate >= file2->creationDate;

			return file1->slot < file2->slot;
		});

	mDotIndex = mSaveStates.empty() ? 0 : Math::min(mDotIndex, (int)mSaveStates.size() - 1);
	mCurrentSaveState = mSaveStates.empty() ? nullptr : mSaveStates[mDotIndex];

	mDotsIndicator.setCount((int)mSaveStates.size());
	mDotsIndicator.setSelectedIndex(mDotIndex);

	reloadBackgroundImage();
}

void GuiSaveStatePillView::rebuildActionList()
{
	mActionList.clear();

	bool hasHighlightedSave = !mSaveStates.empty();

	if (hasHighlightedSave)
		mActionList.add(_("Continue"), SaveStatePillActionType::Continue, 0);

	mActionList.add(_("Start"), SaveStatePillActionType::Start, 0);

	if (mRepository->supportsAutoSave() && mGame->getCurrentGameSetting("autosave") == "1")
	{
		auto existingAuto = std::find_if(mSaveStates.begin(), mSaveStates.end(), [](SaveState* s) { return s->slot == -1; });
		if (existingAuto == mSaveStates.end())
			mActionList.add(_("Start (auto save)"), SaveStatePillActionType::StartAutoSave, 0);
	}

	if (hasHighlightedSave)
		mActionList.add(_("Delete"), SaveStatePillActionType::Delete, 0);
}

void GuiSaveStatePillView::onDotIndexChanged(int newIndex)
{
	if (mSaveStates.empty() || newIndex == mDotIndex)
		return;

	mDotIndex = newIndex;
	mCurrentSaveState = mSaveStates[mDotIndex];
	mDotsIndicator.setSelectedIndex(mDotIndex);
	reloadBackgroundImage();
}

void GuiSaveStatePillView::reloadBackgroundImage()
{
	std::string path = mCurrentSaveState != nullptr ? mCurrentSaveState->getScreenShot() : "";
	if (path.empty())
		path = mGame->getImagePath();

	mBackgroundImage.setImage(path);
}

void GuiSaveStatePillView::runAction(SaveStatePillActionType action)
{
	switch (action)
	{
	case SaveStatePillActionType::Continue:
		mRunCallback(mCurrentSaveState);
		delete this;
		break;
	case SaveStatePillActionType::Start:
		mRunCallback(mRepository->getDefaultNewGameSaveState());
		delete this;
		break;
	case SaveStatePillActionType::StartAutoSave:
		mRunCallback(mRepository->getDefaultAutoSaveSaveState());
		delete this;
		break;
	case SaveStatePillActionType::Delete:
		confirmDelete();
		break;
	}
}

void GuiSaveStatePillView::confirmDelete()
{
	if (mCurrentSaveState == nullptr)
		return;

	SaveState* toDelete = mCurrentSaveState;
	auto conf = toDelete->config;

	mWindow->pushGui(new GuiMsgBox(mWindow, _("ARE YOU SURE YOU WANT TO DELETE THIS ITEM?"), _("YES"),
		[this, toDelete, conf]
		{
			toDelete->remove();
			SaveStateRepository::renumberSlots(mGame, conf);
			mRepository->refresh();

			rebuildDotsAndSaveStates();
			rebuildActionList();
		},
		_("NO"), nullptr));
}

bool GuiSaveStatePillView::input(InputConfig* config, Input input)
{
	if (input.value != 0 && (config->isMappedTo(BUTTON_BACK, input) || config->isMappedTo("l3", input)))
	{
		delete this;
		return true;
	}

	if (input.value != 0 && config->isMappedTo(BUTTON_OK, input))
	{
		if (mActionList.size() > 0)
			runAction(mActionList.getSelected());
		return true;
	}

	if (input.value != 0 && config->isMappedLike("left", input))
	{
		onDotIndexChanged(Math::max(0, mDotIndex - 1));
		return true;
	}

	if (input.value != 0 && config->isMappedLike("right", input))
	{
		onDotIndexChanged(Math::min((int)mSaveStates.size() - 1, mDotIndex + 1));
		return true;
	}

	return mActionList.input(config, input);
}

void GuiSaveStatePillView::update(int deltaTime)
{
	GuiComponent::update(deltaTime);
	mHelpBar.update(deltaTime);
}

void GuiSaveStatePillView::render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = parentTrans * getTransform();

	mBackgroundImage.render(trans);

	Renderer::setMatrix(trans);
	Renderer::drawRect(0.0f, 0.0f, mSize.x(), mSize.y(), kOverlayColor);

	mNamePill.render(trans);
	mActionList.render(trans);
	mDotsIndicator.render(trans);
	mHelpBar.render(trans);
}

std::vector<HelpPrompt> GuiSaveStatePillView::getHelpPrompts()
{
	// The corner pills are drawn directly by mHelpBar instead of the standard help bar.
	return std::vector<HelpPrompt>();
}
