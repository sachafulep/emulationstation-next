#pragma once
#ifndef ES_CORE_COMPONENTS_PILL_METRICS_H
#define ES_CORE_COMPONENTS_PILL_METRICS_H

// Shared height for every "outer" pill (the text-list selector pill, and the three help-bar
// pills). Fixed in pixels so they're all the same physical size on screen regardless of
// resolution. Pills nested inside another pill (the POWER sub-pill, the A button circle) are
// sized from their own content instead and don't use this.
//
// Font sizes are fixed in pixels for the same reason. kFontSizeLarge is the text list itself;
// kFontSizeDefault is every text-based pill (the help bar's SLEEP/A/OPEN labels); kFontSizeSmall
// is for pills that need to read smaller, like the POWER sub-pill.
namespace PillMetrics
{
	constexpr float kOuterPillHeight = 40.0f;
	constexpr float kFontSizeLarge = 18.0f;
	constexpr float kFontSizeDefault = 12.0f;
	constexpr float kFontSizeSmall = 10.0f;

	// Every pill nested inside another pill (the POWER sub-pill, the A button) shares this height.
	constexpr float kInnerPillHeight = kOuterPillHeight - 12.0f;

	// PillRowComponent's own baked-in defaults: house font, background color and edge padding.
	constexpr const char* kFontPath = ":/ZenMaruGothic-Black.ttf";
	constexpr unsigned int kRowBackgroundColor = 0x202420FF;
	constexpr float kRowEdgePaddingLeft = 6.0f;
	constexpr float kRowEdgePaddingRight = 10.0f;
}

#endif // ES_CORE_COMPONENTS_PILL_METRICS_H
