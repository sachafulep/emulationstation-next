#pragma once
#ifndef ES_CORE_COMPONENTS_PILL_CHIP_COMPONENT_H
#define ES_CORE_COMPONENTS_PILL_CHIP_COMPONENT_H

#include "GuiComponent.h"
#include "components/RectangleComponent.h"
#include "components/TextComponent.h"

// A small rounded shape (a pill, or a circle if its size ends up square) with a centered text
// label inside it - e.g. a "POWER" sub-pill, or a single-letter button glyph in a circle.
//
// The shape's width auto-sizes to fit the label's text plus a padding ratio derived from the
// label's own font size. Height is set explicitly via setFixedHeight() - the label stays
// centered within the shape either way.
class PillChipComponent : public GuiComponent
{
public:
	PillChipComponent(Window* window);

	void setColors(unsigned int shapeColor, unsigned int textColor);
	void setFont(const std::shared_ptr<Font>& font);
	void setText(const std::string& text);
	void setFixedHeight(float height);

	// Horizontal padding applied on each side of the text, between the text and the shape's
	// rounded edge. Defaults to 8px (the house chips' tuned value) - override to match a
	// different pill's own padding, e.g. a font-size-relative amount to match a pill list's
	// selector pill.
	void setHorizontalPadding(float padding);

	// Forces the shape to a perfect circle (width == height) instead of auto-fitting the text's
	// own width - for single-glyph chips like the "A" button, where a pill shape looks wrong.
	void setForceCircle(bool enabled);

	// Nudges the text away from its computed centered position. The text is centered using the
	// font's advance width, which isn't always the glyph's visual center (fonts can have uneven
	// left/right side-bearing) - this compensates by hand when that's visibly off.
	void setTextOffset(const Vector2f& offset);

	void render(const Transform4x4f& parentTrans) override;

private:
	void relayout();

	RectangleComponent mShape;
	TextComponent mText;

	float mFixedHeight;
	float mHorizontalPadding;
	bool mForceCircle;
	Vector2f mTextOffset;
};

#endif // ES_CORE_COMPONENTS_PILL_CHIP_COMPONENT_H
