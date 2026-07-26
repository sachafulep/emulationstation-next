#pragma once
#ifndef ES_CORE_COMPONENTS_DOTS_INDICATOR_COMPONENT_H
#define ES_CORE_COMPONENTS_DOTS_INDICATOR_COMPONENT_H

#include "GuiComponent.h"
#include "components/RectangleComponent.h"
#include <memory>
#include <vector>

// A centered horizontal row of small circular dots - one per item, with a single highlighted
// (active-colored) dot marking the current index. Generic and data-agnostic: it only knows a
// count and a selected index, so it isn't tied to any particular list of items.
class DotsIndicatorComponent : public GuiComponent
{
public:
	DotsIndicatorComponent(Window* window);

	// Rebuilds the row for this many dots. Safe to call with 0 (renders nothing).
	void setCount(int count);

	// Moves the highlighted dot. No-op if count is 0.
	void setSelectedIndex(int index);

	void setDotColors(unsigned int inactiveColor, unsigned int activeColor);
	void setDotSize(float diameter, float gap);

	// Centers the row horizontally on centerX, with its top edge at y.
	void setCenteredPosition(float centerX, float y);

	void render(const Transform4x4f& parentTrans) override;

private:
	void relayout();

	std::vector<std::unique_ptr<RectangleComponent>> mDots;

	int mSelectedIndex;
	float mDotSize;
	float mDotGap;
	unsigned int mInactiveColor;
	unsigned int mActiveColor;
	float mCenterX;
	float mTopY;
};

#endif // ES_CORE_COMPONENTS_DOTS_INDICATOR_COMPONENT_H
