#include "components/PillHelpBarComponent.h"

#include "components/PillMetrics.h"
#include "LocaleES.h"
#include "resources/ResourceManager.h"
#include "ImageIO.h"

namespace
{
	constexpr unsigned int kPillHighlightColor = 0xFFFFFFFF; // white icons
	constexpr unsigned int kPillLightTextColor = 0xFFFFFFFF; // text directly on the grey pill
	constexpr unsigned int kPillChipTextColor = 0x999999FF; // the POWER/A chips' text color

	constexpr const char* kNetworkIconPath = ":/network.svg";

	constexpr float kListPadding = 12.0f;

	// The battery icon component hides itself when no real battery is detected. Flip this to
	// true to force it visible with a placeholder icon, so it can be seen on hardware/VMs
	// without one. Leave false for real usage, otherwise a real charging icon will get stomped
	// by the placeholder every time the pill bar lays out.
	constexpr bool kForcePlaceholderIcons = false;
}

PillHelpBarComponent::PillHelpBarComponent(Window* window) :
	mPowerChip(window), mHelpSleepText(window), mPowerSleepRow(window),
	mBackButtonChip(window), mHelpBackText(window),
	mButtonChip(window), mHelpOpenText(window), mOpenRow(window),
	mBatteryRow(window, kBatteryRowEdgePadding, kBatteryRowEdgePadding), mNetworkIcon(window), mBatteryIcon(window)
{
}

void PillHelpBarComponent::layout(const Vector2f& screenSize, Vector2f& safeAreaPosition, Vector2f& safeAreaSize, float& topRowMaxRight, bool showBackButton, const std::string& openLabel)
{
	mScreenSize = screenSize;

	float pillTop = layoutPowerSleepPill(screenSize);
	float safeTop = kListPadding;
	float safeBottom = pillTop - kListPadding;

	layoutOpenPill(screenSize, pillTop, showBackButton, openLabel);
	layoutBatteryPill(screenSize);

	safeAreaPosition = Vector2f(kListPadding, safeTop);
	safeAreaSize = Vector2f(screenSize.x() - 2.0f * kListPadding, safeBottom - safeTop);

	// Only whatever sits at the same height as the top-right battery/network pill needs to
	// avoid it - everything below has the full width to itself.
	topRowMaxRight = mBatteryRow.getPosition().x() - kListPadding;
}

float PillHelpBarComponent::layoutPowerSleepPill(const Vector2f& screenSize)
{
	mPowerChip.setColors(kPillHighlightColor, kPillChipTextColor);
	mPowerChip.setTextOffset(Vector2f(0.0f, 0.5f));

	mPowerSleepRow.addChip(mPowerChip, _("POWER"));
	mPowerChip.setFont(Font::get((int)PillMetrics::kFontSizeSmall, PillMetrics::kFontPath)); // addChip() applies the row's default font first, so this must come after - and before layout(), so the row sees the final size
	mPowerSleepRow.addLabel(mHelpSleepText, kPillLightTextColor, _("SLEEP"));
	mPowerSleepRow.layout();

	float pillLeft = kListPadding;
	float pillBottom = screenSize.y() - kListPadding;
	float pillTop = pillBottom - mPowerSleepRow.getSize().y();

	mPowerSleepRow.setPosition(pillLeft, pillTop);

	return pillTop;
}

void PillHelpBarComponent::layoutOpenPill(const Vector2f& screenSize, float pillTop, bool showBackButton, const std::string& openLabel)
{
	auto buttonFont = Font::get((int)PillMetrics::kFontSizeDefault, PillMetrics::kFontPath);

	if (showBackButton)
	{
		mBackButtonChip.setFont(buttonFont);
		mBackButtonChip.setText("B");
		mBackButtonChip.setColors(kPillHighlightColor, kPillChipTextColor);
		mBackButtonChip.setFixedHeight(PillMetrics::kInnerPillHeight);
		mBackButtonChip.setForceCircle(true);
		mBackButtonChip.setTextOffset(Vector2f(-1.0f, -1.0f)); // compensate for the glyph's uneven side-bearing

		mOpenRow.addItem(mBackButtonChip);
		mOpenRow.addLabel(mHelpBackText, kPillLightTextColor, _("BACK"));
	}

	mButtonChip.setFont(buttonFont);
	mButtonChip.setText("A");
	mButtonChip.setColors(kPillHighlightColor, kPillChipTextColor);
	mButtonChip.setFixedHeight(PillMetrics::kInnerPillHeight);
	mButtonChip.setForceCircle(true);
	mButtonChip.setTextOffset(Vector2f(-1.0f, -1.0f)); // compensate for the glyph's uneven side-bearing

	mOpenRow.addItem(mButtonChip);
	mOpenRow.addLabel(mHelpOpenText, kPillLightTextColor, openLabel.empty() ? _("OPEN") : openLabel);
	mOpenRow.layout();
	mOpenRow.setRightEdge(screenSize.x() - kListPadding, pillTop); // same baseline as the power/sleep pill
}

void PillHelpBarComponent::layoutBatteryPill(const Vector2f& screenSize)
{
	float batteryIconSize = 28.0f;
	float networkIconSize = batteryIconSize;

	mNetworkIcon.setDefaultIcon(ResourceManager::getInstance()->getResourcePath(kNetworkIconPath));

	mBatteryRow.addIcon(mNetworkIcon, kPillHighlightColor, Vector2f(networkIconSize, networkIconSize));
	mBatteryRow.addIcon(mBatteryIcon, kPillHighlightColor, Vector2f(batteryIconSize, batteryIconSize));

	// Both icons only know their real visibility/size once update() has run at least once (each
	// starts out at whatever its own constructor left it at) - layout() has to see that settled
	// state, so it must run after these, not before.
	mNetworkIcon.update(0);
	mBatteryIcon.update(0);

	if (kForcePlaceholderIcons)
		applyTemporaryTestOverride(batteryIconSize);

	mBatteryRow.layout();
	mBatteryRow.setRightEdge(screenSize.x() - kListPadding, kListPadding);
}

void PillHelpBarComponent::applyTemporaryTestOverride(float batteryIconSize)
{
	mBatteryIcon.setVisible(true);
	mBatteryIcon.setImage(ResourceManager::getInstance()->getResourcePath(":/battery/75.svg"), false, MaxSizeInfo(batteryIconSize, batteryIconSize));

	// Re-assert once more since the line above swapped in a (possibly still loading) image.
	mBatteryIcon.setSize(batteryIconSize, batteryIconSize);
}

void PillHelpBarComponent::update(int deltaTime)
{
	mNetworkIcon.update(deltaTime);
	mBatteryIcon.update(deltaTime);

	// Re-flow every frame, not just at layout() time: either icon can flip visible/hidden well
	// after the initial layout (wifi connecting, a battery being detected asynchronously), and
	// layout() is the only thing that knows how to size/position around that.
	mBatteryRow.layout();
	mBatteryRow.setRightEdge(mScreenSize.x() - kListPadding, kListPadding);
}

void PillHelpBarComponent::render(const Transform4x4f& trans)
{
	mPowerSleepRow.render(trans);
	mOpenRow.render(trans);
	mBatteryRow.render(trans);
}
