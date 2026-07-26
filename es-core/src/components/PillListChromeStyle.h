#pragma once
#ifndef ES_CORE_COMPONENTS_PILL_LIST_CHROME_STYLE_H
#define ES_CORE_COMPONENTS_PILL_LIST_CHROME_STYLE_H

#include "components/TextListComponent.h"
#include "components/PillHelpBarComponent.h"
#include "components/PillListStyle.h"

// Owns the visual chrome shared by every pill-styled TextListComponent<T> view (system view's
// "list" style and the game list's pill style): the plain top-left-aligned text list's
// colors/font/selector pill, the pure-black background, and a PillHelpBarComponent (the
// bottom-left POWER/SLEEP, bottom-right (B) BACK/(A) OPEN and top-right battery/network pills)
// that replaces the standard help bar in this mode.
template <typename T>
class PillListChromeStyle
{
public:
	// showBackButton controls whether the bottom-right row includes the "(B) BACK" pill -
	// true for the game list (there's something to navigate back out of), false for system view.
	PillListChromeStyle(Window* window, bool showBackButton = false) :
		mHelpBar(window), mShowBackButton(showBackButton)
	{
	}

	// Configures textList for the pill visual style and lays out the help bar for the given
	// screen size. Returns (via listPosition/listSize) the safe content box the caller should
	// size/position its list to, so it never renders behind the bar's pills.
	void configure(TextListComponent<T>* textList, const Vector2f& screenSize, Vector2f& listPosition, Vector2f& listSize)
	{
		PillListStyle::configure(textList, screenSize);

		float topRowMaxRight = 0.0f;
		mHelpBar.layout(screenSize, listPosition, listSize, topRowMaxRight, mShowBackButton);

		// The list is full width, but its topmost visible row sits at the same height as the
		// battery pill - cap just that row's content, in the list's own local coordinate space.
		textList->setFirstRowMaxRight(topRowMaxRight - listPosition.x());
	}

	void update(int deltaTime)
	{
		mHelpBar.update(deltaTime);
	}

	void renderBackground(const Transform4x4f& trans, const Vector2f& screenSize)
	{
		Renderer::setMatrix(trans);
		Renderer::drawRect(0.0f, 0.0f, screenSize.x(), screenSize.y(), 0x000000FF);
	}

	void renderChrome(const Transform4x4f& trans)
	{
		mHelpBar.render(trans);
	}

private:
	PillHelpBarComponent mHelpBar;
	bool mShowBackButton;
};

#endif // ES_CORE_COMPONENTS_PILL_LIST_CHROME_STYLE_H
