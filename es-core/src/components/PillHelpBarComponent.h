#pragma once
#ifndef ES_CORE_COMPONENTS_PILL_HELP_BAR_COMPONENT_H
#define ES_CORE_COMPONENTS_PILL_HELP_BAR_COMPONENT_H

#include "components/TextComponent.h"
#include "components/PillChipComponent.h"
#include "components/PillRowComponent.h"
#include "components/BatteryIconComponent.h"
#include "components/NetworkIconComponent.h"

class Window;

// A view-agnostic replacement for the standard help bar: a bottom-left "POWER / SLEEP" pill, a
// bottom-right "(A) OPEN" pill, and a top-right battery/network status pill, all in the same
// grey-pill house style. Any view can own one and drop it in - it only needs a screen size to
// lay itself out and a transform to render.
class PillHelpBarComponent
{
public:
	PillHelpBarComponent(Window* window);

	// Lays out all three pills for the given screen size. Returns (via safeAreaPosition/
	// safeAreaSize) the full-width content box above the bottom pills, so the caller can keep
	// its own content from rendering behind them - this box is NOT narrowed for the top-right
	// battery/network pill, since only whatever content sits at that same height needs to avoid
	// it. topRowMaxRight (screen-space x) is that boundary, for the caller to apply to just the
	// content occupying the battery pill's row. showBackButton prepends a "(B) BACK" pill before
	// the "(A) OPEN" one in the bottom-right row - only the game list passes true for this, since
	// system view has nothing to navigate back out of.
	void layout(const Vector2f& screenSize, Vector2f& safeAreaPosition, Vector2f& safeAreaSize, float& topRowMaxRight, bool showBackButton = false);

	void update(int deltaTime);
	void render(const Transform4x4f& trans);

private:
	// Lays out the bottom-left pill; returns its top edge, needed to align the bottom-right pill
	// and to compute the safe area.
	float layoutPowerSleepPill(const Vector2f& screenSize);
	void layoutOpenPill(const Vector2f& screenSize, float pillTop, bool showBackButton);
	void layoutBatteryPill(const Vector2f& screenSize);

	// Forces the battery icon to a visible placeholder, ignoring real detected state. Only
	// called when kForcePlaceholderIcons (PillHelpBarComponent.cpp) is set to true.
	void applyTemporaryTestOverride(float batteryIconSize);

	// The battery/network row is icon-only (no chip/text), so unlike the other two rows it uses
	// symmetric edge padding - otherwise a single visible icon (with the other one hidden) reads
	// as off-center within the pill.
	static constexpr float kBatteryRowEdgePadding = 10.0f;

	// Bottom-left "POWER / SLEEP" pill.
	PillChipComponent mPowerChip;
	TextComponent mHelpSleepText;
	PillRowComponent mPowerSleepRow;

	// Bottom-right "(B) BACK (A) OPEN" pill - the B/BACK part only added when showBackButton.
	PillChipComponent mBackButtonChip;
	TextComponent mHelpBackText;
	PillChipComponent mButtonChip;
	TextComponent mHelpOpenText;
	PillRowComponent mOpenRow;

	// Top-right battery/network indicator pill.
	PillRowComponent mBatteryRow;
	NetworkIconComponent mNetworkIcon;
	BatteryIconComponent mBatteryIcon;

	// Remembered from the last layout() call so update() can re-flow mBatteryRow every frame -
	// its icons can flip visible/hidden well after layout() last ran (wifi connecting, a
	// battery being detected), and nothing else would ever reposition the row to notice.
	Vector2f mScreenSize;
};

#endif // ES_CORE_COMPONENTS_PILL_HELP_BAR_COMPONENT_H
