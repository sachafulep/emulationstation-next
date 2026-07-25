#include "views/gamelist/GameListPillStyle.h"

#include "components/PillListStyle.h"
#include "FileData.h"

GameListPillStyle::GameListPillStyle(Window* window) : mHelpBar(window)
{
}

void GameListPillStyle::configure(TextListComponent<FileData*>* textList, const Vector2f& screenSize, Vector2f& listPosition, Vector2f& listSize)
{
	PillListStyle::configure(textList, screenSize);
	mHelpBar.layout(screenSize, listPosition, listSize, true);
}

void GameListPillStyle::update(int deltaTime)
{
	mHelpBar.update(deltaTime);
}

void GameListPillStyle::renderBackground(const Transform4x4f& trans, const Vector2f& screenSize)
{
	Renderer::setMatrix(trans);
	Renderer::drawRect(0.0f, 0.0f, screenSize.x(), screenSize.y(), 0x000000FF);
}

void GameListPillStyle::renderChrome(const Transform4x4f& trans)
{
	mHelpBar.render(trans);
}
