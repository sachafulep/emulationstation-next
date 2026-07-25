#pragma once
#ifndef ES_APP_VIEWS_SYSTEM_VIEW_LIST_STYLE_H
#define ES_APP_VIEWS_SYSTEM_VIEW_LIST_STYLE_H

#include "components/TextListComponent.h"
#include "components/PillHelpBarComponent.h"

class SystemData;
class Window;

// Owns the visual chrome specific to the system view's "list" style (SystemViewStyle == "list"):
// the plain top-left-aligned text list's colors/font/selector pill, the pure-black background,
// and a PillHelpBarComponent (the bottom-left POWER/SLEEP, bottom-right A/OPEN and top-right
// battery/network pills) that replaces the standard help bar in this mode.
class SystemViewListStyle
{
public:
	SystemViewListStyle(Window* window);

	// Configures textList for the list-mode visual style and lays out the help bar for the
	// given screen size. Returns (via listPosition/listSize) the safe content box the caller
	// should size/position its list to, so it never renders behind the bar's pills.
	void configure(TextListComponent<SystemData*>* textList, const Vector2f& screenSize, Vector2f& listPosition, Vector2f& listSize);

	void update(int deltaTime);

	void renderBackground(const Transform4x4f& trans, const Vector2f& screenSize);
	void renderChrome(const Transform4x4f& trans);

private:
	PillHelpBarComponent mHelpBar;
};

#endif // ES_APP_VIEWS_SYSTEM_VIEW_LIST_STYLE_H
