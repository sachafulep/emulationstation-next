#include "components/DotsIndicatorComponent.h"
#include "components/PillMetrics.h"

DotsIndicatorComponent::DotsIndicatorComponent(Window* window) : GuiComponent(window),
	mSelectedIndex(0), mDotSize(8.0f), mDotGap(10.0f),
	mInactiveColor(PillMetrics::kRowBackgroundColor), mActiveColor(0xFFFFFFFF),
	mCenterX(0.0f), mTopY(0.0f)
{
}

void DotsIndicatorComponent::setCount(int count)
{
	mDots.clear();

	for (int i = 0; i < count; i++)
	{
		auto dot = std::make_unique<RectangleComponent>(mWindow);
		dot->setBorderSize(0.0f);
		dot->setRoundCorners(0.5f);
		mDots.push_back(std::move(dot));
	}

	if (mSelectedIndex >= count)
		mSelectedIndex = count - 1;

	relayout();
}

void DotsIndicatorComponent::setSelectedIndex(int index)
{
	if (mDots.empty())
		return;

	mSelectedIndex = index;
	relayout();
}

void DotsIndicatorComponent::setDotColors(unsigned int inactiveColor, unsigned int activeColor)
{
	mInactiveColor = inactiveColor;
	mActiveColor = activeColor;
	relayout();
}

void DotsIndicatorComponent::setDotSize(float diameter, float gap)
{
	mDotSize = diameter;
	mDotGap = gap;
	relayout();
}

void DotsIndicatorComponent::setCenteredPosition(float centerX, float y)
{
	mCenterX = centerX;
	mTopY = y;
	relayout();
}

void DotsIndicatorComponent::relayout()
{
	int n = (int)mDots.size();
	float totalWidth = n > 0 ? n * mDotSize + (n - 1) * mDotGap : 0.0f;
	float startX = mCenterX - totalWidth / 2.0f;

	for (int i = 0; i < n; i++)
	{
		mDots[i]->setSize(mDotSize, mDotSize);
		mDots[i]->setPosition(startX + i * (mDotSize + mDotGap), mTopY);
		mDots[i]->setColor(i == mSelectedIndex ? mActiveColor : mInactiveColor);
	}

	setSize(totalWidth, mDotSize);
	setPosition(startX, mTopY);
}

void DotsIndicatorComponent::render(const Transform4x4f& parentTrans)
{
	for (auto& dot : mDots)
		dot->render(parentTrans);
}
