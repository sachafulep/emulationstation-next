#include "components/PillChipComponent.h"

PillChipComponent::PillChipComponent(Window* window) : GuiComponent(window),
	mShape(window), mText(window),
	mFixedHeight(0.0f), mForceCircle(false), mTextOffset(Vector2f::Zero())
{
	mShape.setBorderSize(0.0f);
	mShape.setRoundCorners(0.5f);
	mShape.setColor(0xFFFFFFFF);

	mText.setColor(0x000000FF);
	mText.setSize(0.0f, 0.0f);
}

void PillChipComponent::setColors(unsigned int shapeColor, unsigned int textColor)
{
	mShape.setColor(shapeColor);
	mText.setColor(textColor);
}

void PillChipComponent::setFont(const std::shared_ptr<Font>& font)
{
	mText.setFont(font);
	relayout();
}

void PillChipComponent::setText(const std::string& text)
{
	mText.setText(text);
	relayout();
}

void PillChipComponent::setFixedHeight(float height)
{
	mFixedHeight = height;
	relayout();
}

void PillChipComponent::setForceCircle(bool enabled)
{
	mForceCircle = enabled;
	relayout();
}

void PillChipComponent::setTextOffset(const Vector2f& offset)
{
	mTextOffset = offset;
	relayout();
}

void PillChipComponent::relayout()
{
	auto font = mText.getFont();
	if (font == nullptr)
		return;

	float horizontalPadding = 8.0f;
	Vector2f textSize = mText.getSize();
	float width = mForceCircle ? mFixedHeight : textSize.x() + horizontalPadding * 2.0f;
	Vector2f shapeSize(width, mFixedHeight);

	mShape.setSize(shapeSize);
	mShape.setPosition(0.0f, 0.0f);

	mText.setPosition((shapeSize.x() - textSize.x()) / 2.0f + mTextOffset.x(), (shapeSize.y() - textSize.y()) / 2.0f + mTextOffset.y());

	setSize(shapeSize);
}

void PillChipComponent::render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = parentTrans * getTransform();
	mShape.render(trans);
	mText.render(trans);
}
