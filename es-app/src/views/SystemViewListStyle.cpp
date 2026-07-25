#include "views/SystemViewListStyle.h"

#include "components/PillListStyle.h"
#include "SystemData.h"

SystemViewListStyle::SystemViewListStyle(Window* window) : mHelpBar(window)
{
}

void SystemViewListStyle::configure(TextListComponent<SystemData*>* textList, const Vector2f& screenSize, Vector2f& listPosition, Vector2f& listSize)
{
	PillListStyle::configure(textList, screenSize);
	mHelpBar.layout(screenSize, listPosition, listSize);
}

void SystemViewListStyle::update(int deltaTime)
{
	mHelpBar.update(deltaTime);
}

void SystemViewListStyle::renderBackground(const Transform4x4f& trans, const Vector2f& screenSize)
{
	Renderer::setMatrix(trans);
	Renderer::drawRect(0.0f, 0.0f, screenSize.x(), screenSize.y(), 0x000000FF);
}

void SystemViewListStyle::renderChrome(const Transform4x4f& trans)
{
	mHelpBar.render(trans);
}
