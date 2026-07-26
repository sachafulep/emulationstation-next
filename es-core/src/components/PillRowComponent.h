#pragma once
#ifndef ES_CORE_COMPONENTS_PILL_ROW_COMPONENT_H
#define ES_CORE_COMPONENTS_PILL_ROW_COMPONENT_H

#include "GuiComponent.h"
#include "components/RectangleComponent.h"
#include "components/TextComponent.h"
#include "components/PillChipComponent.h"
#include "components/ImageComponent.h"
#include <vector>

// A rounded pill background containing a left-to-right row of items (any GuiComponent - a
// PillChipComponent, a plain TextComponent label, an icon, ...), laid out in the house pill
// style: fixed height, edge padding, background color and font all come from PillMetrics, with a
// fixed 4px gap between items. Items are not owned by this component - they're expected to
// outlive it, same as every other widget in this engine's composite components.
class PillRowComponent : public GuiComponent
{
public:
	// edgePaddingLeft/Right default to PillMetrics' house values (tuned for a chip-then-label
	// row, where the round chip sits closer to the pill's rounded corner than the text on the
	// far side needs to). Pass explicit equal values for rows with more uniform content (e.g.
	// icon-only rows), where that asymmetry would otherwise read as off-center.
	PillRowComponent(Window* window);
	PillRowComponent(Window* window, float edgePaddingLeft, float edgePaddingRight);

	// Appends an item to the row, left to right.
	void addItem(GuiComponent& item);

	// Convenience for the common case of a plain text item with no wrapper shape: configures
	// label (font/color/text, auto-sized) and appends it in one call. Uses the row's default
	// font unless an explicit one is given.
	void addLabel(TextComponent& label, unsigned int color, const std::string& text);
	void addLabel(TextComponent& label, const std::shared_ptr<Font>& font, unsigned int color, const std::string& text);

	// Convenience for a chip using the row's default font: sets its text and appends it. Chips
	// needing a different font (or a fixed size) should be configured directly and passed to
	// addItem() instead.
	void addChip(PillChipComponent& chip, const std::string& text);

	// Convenience for a plain icon item: sets its tint color and size, then appends it.
	void addIcon(ImageComponent& icon, unsigned int color, const Vector2f& size);

	// Recomputes the row's total size and every item's position. Call after all addItem() calls
	// and whenever an item's own size may have changed.
	void layout();

	// Positions the row so its right edge lands at rightX (its left edge follows from its own
	// width) - the mirror image of the plain setPosition() used for left-anchored placement.
	void setRightEdge(float rightX, float y);

	void render(const Transform4x4f& parentTrans) override;

private:
	RectangleComponent mBackground;

	std::vector<GuiComponent*> mItems;
	std::shared_ptr<Font> mDefaultFont;
	float mEdgePaddingLeft;
	float mEdgePaddingRight;
};

#endif // ES_CORE_COMPONENTS_PILL_ROW_COMPONENT_H
