#pragma once
#ifndef ES_CORE_COMPONENTS_PILL_LIST_STYLE_H
#define ES_CORE_COMPONENTS_PILL_LIST_STYLE_H

#include "components/TextListComponent.h"
#include "components/PillMetrics.h"

// The shared "pill list" visual treatment: top-left-aligned white text with a black selector
// pill, in the house pill font. Originally the system view's list style, factored out here so
// any other TextListComponent<T> (e.g. a per-system game list) can reuse it - it only calls
// generic TextListComponent<T> methods, so it works for any item type T.
namespace PillListStyle
{
	template <typename T>
	void configure(TextListComponent<T>* textList, const Vector2f& screenSize)
	{
		constexpr unsigned int kHighlightColor = 0xFFFFFFFF; // text-list highlight bg
		constexpr unsigned int kSelectedTextColor = 0x000000FF; // text-list selected text

		textList->setAlignment(TextListComponent<T>::ALIGN_LEFT);
		textList->setHorizontalMargin(16.0f);
		textList->setUppercase(false);
		textList->setColor(0, kHighlightColor);
		textList->setColor(1, kHighlightColor);
		textList->setSelectedColor(kSelectedTextColor);
		textList->setSelectorPillMode(true);
		textList->setSelectorPillHeight(PillMetrics::kOuterPillHeight);
		textList->setFont(Font::get((int)PillMetrics::kFontSizeLarge, PillMetrics::kFontPath));
		textList->setLineSpacing(1.85f);

		// Debug aid for tuning row spacing/sizing - flip to true to outline each row, false when done.
		textList->setDebugShowRowBounds(false);
	}
}

#endif // ES_CORE_COMPONENTS_PILL_LIST_STYLE_H
