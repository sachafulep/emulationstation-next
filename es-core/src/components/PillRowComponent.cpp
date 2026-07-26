#include "components/PillRowComponent.h"
#include "components/PillMetrics.h"

namespace
{
	constexpr float kItemGap = 8.0f;
}

PillRowComponent::PillRowComponent(Window* window) :
	PillRowComponent(window, PillMetrics::kRowEdgePaddingLeft, PillMetrics::kRowEdgePaddingRight)
{
}

PillRowComponent::PillRowComponent(Window* window, float edgePaddingLeft, float edgePaddingRight) : GuiComponent(window),
	mBackground(window), mEdgePaddingLeft(edgePaddingLeft), mEdgePaddingRight(edgePaddingRight)
{
	mBackground.setBorderSize(0.0f);
	mBackground.setRoundCorners(0.5f);
	mBackground.setColor(PillMetrics::kRowBackgroundColor);

	mDefaultFont = Font::get((int)PillMetrics::kFontSizeDefault, PillMetrics::kFontPath);
}

void PillRowComponent::addItem(GuiComponent& item)
{
	mItems.push_back(&item);
}

void PillRowComponent::addLabel(TextComponent& label, unsigned int color, const std::string& text)
{
	addLabel(label, mDefaultFont, color, text);
}

void PillRowComponent::addLabel(TextComponent& label, const std::shared_ptr<Font>& font, unsigned int color, const std::string& text)
{
	label.setFont(font);
	label.setColor(color);
	label.setText(text);
	label.setSize(0.0f, 0.0f);
	addItem(label);
}

void PillRowComponent::addChip(PillChipComponent& chip, const std::string& text)
{
	chip.setFont(mDefaultFont);
	chip.setFixedHeight(PillMetrics::kInnerPillHeight);
	chip.setText(text);
	addItem(chip);
}

void PillRowComponent::addIcon(ImageComponent& icon, unsigned int color, const Vector2f& size)
{
	icon.setColor(color);
	icon.setSize(size.x(), size.y());
	addItem(icon);
}

void PillRowComponent::layout()
{
	float height = PillMetrics::kOuterPillHeight;

	float x = 0.0f;
	bool addedVisibleItem = false;
	for (size_t i = 0; i < mItems.size(); i++)
	{
		GuiComponent* item = mItems[i];
		if (!item->isVisible())
			continue;

		if (!addedVisibleItem)
			x = mEdgePaddingLeft;
		else
			x += kItemGap;

		Vector2f size = item->getSize();
		item->setPosition(x, (height - size.y()) / 2.0f);

		x += size.x();
		addedVisibleItem = true;
	}
	if (addedVisibleItem)
		x += mEdgePaddingRight;

	mBackground.setSize(x, height);
	mBackground.setPosition(0.0f, 0.0f);

	setSize(x, height);
}

void PillRowComponent::setRightEdge(float rightX, float y)
{
	setPosition(rightX - getSize().x(), y);
}

void PillRowComponent::render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = parentTrans * getTransform();

	mBackground.render(trans);

	for (auto* item : mItems)
		item->render(trans);
}
