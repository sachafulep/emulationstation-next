#pragma once
#ifndef ES_APP_COMPONENTS_TEXT_LIST_COMPONENT_H
#define ES_APP_COMPONENTS_TEXT_LIST_COMPONENT_H

#include "components/IList.h"
#include "math/Misc.h"
#include "utils/StringUtil.h"
#include "Log.h"
#include "Sound.h"
#include <memory>
#include "components/ScrollbarComponent.h"
#include "components/RectangleComponent.h"
#include "Settings.h"
#include "Window.h"
#include "InputManager.h"
#include "utils/Delegate.h"
#include "CarouselComponent.h"
#include "BindingManager.h"

#define HOLD_TIME 1000

class TextCache;

struct TextListData
{
	unsigned int colorId;
	std::shared_ptr<TextCache> textCache;

	// Pill mode's ellipsis-truncated version of textCache, when the full text overflows the
	// list's width. Rebuilt only when the available width changes.
	std::shared_ptr<TextCache> truncatedCache;
	float truncatedCacheWidth = -1.0f;

	std::shared_ptr<GuiComponent> itemTemplate;
};

template <typename T>
struct is_textlist_bindable : std::is_base_of<IBindable, std::remove_pointer_t<T>> {};

//A graphical list. Supports multiple colors for rows and scrolling.
template <typename T>
class TextListComponent : public IList<TextListData, T>
{
protected:
	using IList<TextListData, T>::mEntries;
	using IList<TextListData, T>::listUpdate;
	using IList<TextListData, T>::listInput;
	using IList<TextListData, T>::listRenderTitleOverlay;
	using IList<TextListData, T>::getTransform;
	using IList<TextListData, T>::mSize;
	using IList<TextListData, T>::mCursor;
	using IList<TextListData, T>::mIsMouseOver;
	using IList<TextListData, T>::mScrollVelocity;
	using IList<TextListData, T>::mWindow;
	using IList<TextListData, T>::Entry;

public:
	using IList<TextListData, T>::size;
	using IList<TextListData, T>::getSize;
	using IList<TextListData, T>::getChild;
	using IList<TextListData, T>::getChildCount;
	using IList<TextListData, T>::isScrolling;
	using IList<TextListData, T>::stopScrolling;
	using IList<TextListData, T>::setOpacity;
	using IList<TextListData, T>::getOpacity;
	using IList<TextListData, T>::onShow;
	using IList<TextListData, T>::onHide;
	using IList<TextListData, T>::isShowing;
	
	TextListComponent(Window* window);

	bool input(InputConfig* config, Input input) override;
	void update(int deltaTime) override;
	void render(const Transform4x4f& parentTrans) override;
	void applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, unsigned int properties) override;
	
	void onShow() override;
	void onHide() override;
	void setOpacity(unsigned char opacity) override;

	void add(const std::string& name, const T& obj, unsigned int colorId);

	enum Alignment
	{
		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT
	};

	std::string getThemeTypeName() override { return "textlist"; }

	inline void setAlignment(Alignment align) { mAlignment = align; }

	inline void setCursorChangedCallback(const std::function<void(CursorState state)>& func) { mCursorChangedCallback = func; }

	inline void setFont(const std::shared_ptr<Font>& font)
	{
		mFont = font;
		for (auto it = mEntries.begin(); it != mEntries.end(); it++)
		{
			it->data.textCache.reset();
			it->data.truncatedCache.reset();
		}
	}

	inline void setUppercase(bool uppercase)
	{
		mUppercase = uppercase;
		for (auto it = mEntries.begin(); it != mEntries.end(); it++)
		{
			it->data.textCache.reset();
			it->data.truncatedCache.reset();
		}
	}

	inline void setSelectorHeight(float selectorScale) { mSelectorHeight = selectorScale; }
	inline void setSelectorOffsetY(float selectorOffsetY) { mSelectorOffsetY = selectorOffsetY; }
	inline void setSelectorColor(unsigned int color) { mSelectorColor = color; }
	inline void setSelectorColorEnd(unsigned int color) { mSelectorColorEnd = color; }
	inline void setSelectorColorGradientHorizontal(bool horizontal) { mSelectorColorGradientHorizontal = horizontal; }
	inline void setSelectedColor(unsigned int color) { mSelectedColor = color; }
	inline void setColor(unsigned int id, unsigned int color) { mColors[id] = color; }
	inline void setLineSpacing(float lineSpacing) { mLineSpacing = lineSpacing; }
	inline void setBonusTextColor(unsigned int color) { mBonusColor = color; mHasBonusColor = true; }
	inline void setBonusSelectedTextColor(unsigned int color) { mBonusSelectedColor = color; mHasBonusSelectedColor = true; }
	inline void setHorizontalMargin(float margin) { mHorizontalMargin = margin; }

	// Draws the cursor row's selector as a fully-rounded pill sized to fit the row's text
	// (radius = half the pill height), instead of a full-width bar.
	inline void setSelectorPillMode(bool enabled, unsigned int pillColor = 0xFFFFFFFF)
	{
		mSelectorPillMode = enabled;
		mSelectorPillColor = pillColor;
	}

	// Overrides the selector pill's auto-fit-to-text height with a fixed one (e.g. to match
	// other pills elsewhere on screen). Pass a value < 0 to go back to auto-fitting.
	inline void setSelectorPillHeight(float height) { mSelectorPillHeight = height; }

	// In pill mode, an overflowing selected entry normally marquee-scrolls, starting
	// immediately (no delay) rather than after the usual pause. Pass true to instead cut it
	// off with an ellipsis and leave it static, same as the rest of the (non-selected) rows.
	inline void setSelectorPillEllipsisMode(bool enabled) { mSelectorPillEllipsisMode = enabled; }

	// Debug aid: draws an outline around every row's bounds so spacing/sizing can be tuned.
	inline void setDebugShowRowBounds(bool enabled, unsigned int color = 0xFF0000FF)
	{
		mDebugShowRowBounds = enabled;
		mDebugRowBoundsColor = color;
	}

	void onSizeChanged() override;

	void resetLastCursor() { mLastCursor = -1; }
	int getLastCursor() { return mLastCursor; }

	virtual bool onMouseClick(int button, bool pressed, int x, int y) override;
	virtual void onMouseMove(int x, int y) override;
	virtual bool onMouseWheel(int delta) override;
	virtual bool hitTest(int x, int y, Transform4x4f& parentTransform, std::vector<GuiComponent*>* pResult = nullptr) override;

	Delegate<ILongMouseClickEvent> longMouseClick;

protected:
	virtual void onScroll(int /*amt*/) { if (!mScrollSound.empty()) Sound::get(mScrollSound)->play(); }
	virtual void onCursorChanged(const CursorState& state);

private:
	void  updateCameraOffset();
	float getRowHeight() const;
	float getTotalRowHeight() const;

	ThemeData::ThemeElement		mItemTemplate;

	int mMarqueeOffset;
	int mMarqueeOffset2;
	int mMarqueeTime;

	int mLineCount;

	int mLastCursor;
	CursorState mLastCursorState;

	Alignment mAlignment;
	float mHorizontalMargin;

	std::function<void(CursorState state)> mCursorChangedCallback;

	std::shared_ptr<Font> mFont;
	bool mUppercase;
	float mLineSpacing;
	float mSelectorHeight;
	float mSelectorOffsetY;
	unsigned int mSelectorColor;
	unsigned int mSelectorColorEnd;
	bool mSelectorColorGradientHorizontal = true;
	unsigned int mSelectedColor;

	unsigned int mBonusColor;
	bool mHasBonusColor = false;
	unsigned int mBonusSelectedColor;
	bool mHasBonusSelectedColor = false;

	std::string mScrollSound;
	static const unsigned int COLOR_ID_COUNT = 2;
	unsigned int mColors[COLOR_ID_COUNT];

	unsigned int mGlowColor;
	unsigned int mGlowSize;
	Vector2f	 mGlowOffset;

	ImageComponent mSelectorImage;

	bool mSelectorPillMode;
	unsigned int mSelectorPillColor;
	float mSelectorPillHeight; // < 0 = auto-fit to text height
	bool mSelectorPillEllipsisMode;
	RectangleComponent mSelectorPill;

	bool mDebugShowRowBounds;
	unsigned int mDebugRowBoundsColor;

	ScrollbarComponent mScrollbar;
	float mCameraOffset;

	// Pill mode's windowed scroll position (index of the topmost visible row). Persisted
	// across calls instead of derived from the cursor, so the list only scrolls once the
	// cursor would move outside the currently visible rows.
	int mPillTopEntry;

	int		  mHotRow;
	int		  mPressedRow;
	Vector2i  mPressedPoint;
	int		  mTimeHoldingButton;

	// Handle pointer types derived from IBindable
	template <typename U = T>
	typename std::enable_if<is_textlist_bindable<U>::value, IBindable*>::type
		getBindable(const typename IList<TextListData, T>::Entry& entry)
	{
		IBindable* bindingPtr = static_cast<IBindable*>(entry.object);
		return bindingPtr;
	}

	// Handle other types
	template <typename U = T>
	typename std::enable_if<!is_textlist_bindable<U>::value, IBindable*>::type
		getBindable(const typename IList<TextListData, T>::Entry& entry)
	{
		return nullptr;
	}

};

template <typename T>
TextListComponent<T>::TextListComponent(Window* window) :
	IList<TextListData, T>(window), mSelectorImage(window), mScrollbar(window), mSelectorPill(window)
{
	mSelectorPillMode = false;
	mSelectorPillColor = 0xFFFFFFFF;
	mSelectorPillHeight = -1.0f;
	mSelectorPillEllipsisMode = false;
	mDebugShowRowBounds = false;
	mDebugRowBoundsColor = 0xFF0000FF;
	mSelectorPill.setBorderSize(0.0f);
	mSelectorPill.setRoundCorners(0.5f);

	mCameraOffset = 0;
	mPillTopEntry = 0;
	mLineCount = -1;
	mMarqueeOffset = 0;
	mMarqueeOffset2 = 0;
	mMarqueeTime = 0;
	mLastCursor = -1;
	mLastCursorState = CursorState::CURSOR_STOPPED;

	mHorizontalMargin = 0;
	mAlignment = ALIGN_CENTER;

	mFont = Font::get(FONT_SIZE_MEDIUM);
	mUppercase = false;
	mLineSpacing = 1.5f;
	mSelectorHeight = mFont->getSize() * 1.5f;
	mSelectorOffsetY = 0;
	mSelectorColor = 0x000000FF;
	mSelectorColorEnd = 0x000000FF;
	mSelectorColorGradientHorizontal = true;
	mSelectedColor = 0;
	mColors[0] = 0x0000FFFF;
	mColors[1] = 0x00FF00FF;

	mGlowColor = 0;
	mGlowSize = 2;
	mGlowOffset = Vector2f::Zero();

	mTimeHoldingButton = -1;
	mHotRow = -1;
	mPressedRow = -1;
	mPressedPoint = Vector2i(-1, -1);
}

template <typename T>
void TextListComponent<T>::render(const Transform4x4f& parentTrans)
{
	Transform4x4f trans = parentTrans * getTransform();

	std::shared_ptr<Font>& font = mFont;

	if (size() == 0)
		return;

	auto rect = Renderer::getScreenRect(trans, mSize);
	if (!Renderer::isVisibleOnScreen(rect))
		return;

	if (Settings::DebugMouse() && mIsMouseOver)
	{
		Renderer::setMatrix(trans);
		Renderer::drawRect(0.0f, 0.0f, mSize.x(), mSize.y(), 0xFF000033, 0xFF000033);
	}

	float opacity = getOpacity() / 255.0;
	
	float entrySize = getRowHeight();
	int startEntry = mCameraOffset / entrySize;
	int screenCount = mLineCount > 0 ? mLineCount : (int)(mSize.y() / entrySize);
	int lastEntry = Math::min((int) mEntries.size(), startEntry + screenCount + 1);
	
	int listCutoff = startEntry + screenCount;
	if (listCutoff > size())
		listCutoff = size();

	int loopStart = startEntry;
	int loopEnd = lastEntry;

	if (!mItemTemplate.type.empty())
	{
		startEntry = Math::max(0, startEntry - 2);
		lastEntry = Math::min((int) mEntries.size(), lastEntry + 2);

		loopStart = 0;
		loopEnd = mEntries.size();
	}

	Renderer::pushClipRect(rect);

	float y = startEntry * entrySize - mCameraOffset;

	for (int i = loopStart; i < loopEnd; i++) // mEntries.size()
	{
		typename IList<TextListData, T>::Entry& entry = mEntries.at((unsigned int)i);

		if (i < startEntry || i >= lastEntry)
		{
			if (entry.data.itemTemplate)
			{
				if (entry.data.itemTemplate->isShowing())
					entry.data.itemTemplate->onHide();

				if (entry.data.itemTemplate->isVisible())
					entry.data.itemTemplate->setVisible(false);
			}

			continue;
		}

		if (!entry.data.itemTemplate && !mItemTemplate.type.empty())
		{
			auto bindable = getBindable(entry);
			if (bindable != nullptr)
			{
				CarouselItemTemplate* templ = new CarouselItemTemplate(entry.name, mWindow);
				templ->setScaleOrigin(0.0f);
				templ->setSize(mSize.x(), entrySize);
				templ->isShowing() = isShowing();
				templ->setAmbientOpacity(getOpacity());
				templ->loadTemplatedChildren(&mItemTemplate);
				templ->updateBindings(bindable);

				entry.data.itemTemplate = std::shared_ptr<GuiComponent>(templ);

				if (mCursor == i && (entry.data.itemTemplate->selectStoryboard("activate") || entry.data.itemTemplate->selectStoryboard()))
				{
					entry.data.itemTemplate->startStoryboard();
					entry.data.itemTemplate->update(0);
				}
			}
		}
		
		if (entry.data.itemTemplate)
		{
			if (!entry.data.itemTemplate->isVisible())
				entry.data.itemTemplate->setVisible(true);

			if (isShowing() && !entry.data.itemTemplate->isShowing())
				entry.data.itemTemplate->onShow();

			entry.data.itemTemplate->setPosition(0.0f, y);
			entry.data.itemTemplate->render(trans);
		}
		else
		{
			unsigned int color;
			if (mCursor == i && mSelectedColor)
				color = mSelectedColor;
			else
				color = mColors[entry.data.colorId];

			if (!entry.data.textCache)
				entry.data.textCache = std::unique_ptr<TextCache>(font->buildTextCache(mUppercase ? Utils::String::toUpper(entry.name) : entry.name, 0, 0, 0x000000FF));

			if (mCursor == i && mHasBonusSelectedColor)
				entry.data.textCache->setColors(color, mBonusSelectedColor);
			else if (mHasBonusColor)
				entry.data.textCache->setColors(color, mBonusColor);
			else
				entry.data.textCache->setColor(color);

			TextCache* activeCache = entry.data.textCache.get();

			// In pill mode, overflowing non-selected entries are always truncated with an
			// ellipsis (they can't marquee-scroll). The selected entry does the same only in
			// ellipsis mode; otherwise it marquee-scrolls instead (see update()).
			if (mSelectorPillMode && (i != mCursor || mSelectorPillEllipsisMode))
			{
				float availableWidth = mSize.x() - mHorizontalMargin * 2.0f;
				if (availableWidth > 0.0f && activeCache->metrics.size.x() > availableWidth)
				{
					if (!entry.data.truncatedCache || entry.data.truncatedCacheWidth != availableWidth)
					{
						std::string text = mUppercase ? Utils::String::toUpper(entry.name) : entry.name;
						const std::string abbrev = "...";
						Vector2f abbrevSize = font->sizeText(abbrev);

						std::string truncated;
						size_t cursorPos = 0;
						while (cursorPos < text.size())
						{
							size_t newCursor = Utils::String::nextCursor(text, cursorPos);
							if (cursorPos == newCursor)
								break;
							cursorPos = newCursor;

							std::string testText = text.substr(0, cursorPos);
							if (font->sizeText(testText).x() + abbrevSize.x() > availableWidth)
								break;

							truncated = testText;
						}
						truncated += abbrev;

						entry.data.truncatedCache = std::shared_ptr<TextCache>(font->buildTextCache(truncated, 0, 0, 0x000000FF));
						entry.data.truncatedCacheWidth = availableWidth;
					}

					if (mCursor == i && mHasBonusSelectedColor)
						entry.data.truncatedCache->setColors(color, mBonusSelectedColor);
					else if (mHasBonusColor)
						entry.data.truncatedCache->setColors(color, mBonusColor);
					else
						entry.data.truncatedCache->setColor(color);

					activeCache = entry.data.truncatedCache.get();
				}
			}

			Vector3f offset(0, y, 0);

			if (mLineCount > 0 || mSelectorPillMode) // Vertical center
				offset[1] += (int)((entrySize - activeCache->metrics.size.y()) / 2);

			switch (mAlignment)
			{
			case ALIGN_LEFT:
				offset[0] = mHorizontalMargin;
				break;
			case ALIGN_CENTER:
				offset[0] = (int)((mSize.x() - activeCache->metrics.size.x()) / 2);
				if (offset[0] < mHorizontalMargin)
					offset[0] = mHorizontalMargin;
				break;
			case ALIGN_RIGHT:
				offset[0] = (mSize.x() - activeCache->metrics.size.x());
				offset[0] -= mHorizontalMargin;
				if (offset[0] < mHorizontalMargin)
					offset[0] = mHorizontalMargin;
				break;
			}

			// render text
			Transform4x4f drawTrans = trans;

			// currently selected item text might be scrolling
			if ((mCursor == i) && (mMarqueeOffset > 0))
				drawTrans.translate(offset - Vector3f((float)mMarqueeOffset, 0, 0));
			else
				drawTrans.translate(offset);

			if (Settings::DebugText())
			{
				Renderer::setMatrix(drawTrans);
				auto sz = mFont->sizeText(mUppercase ? Utils::String::toUpper(entry.name) : entry.name);
				Renderer::drawRect(0.0f, 0.0f, sz.x(), sz.y(), 0xFF000033, 0xFF000033);
			}

			if (mCursor == i)
			{
				if (mSelectorPillMode)
				{
					float padH = mFont->getSize() * 0.6f;
					float padV = mFont->getSize() * 0.03f;

					float pillW = activeCache->metrics.size.x() + padH * 2.0f;
					float pillH = mSelectorPillHeight >= 0.0f ? mSelectorPillHeight : activeCache->metrics.size.y() + padV * 2.0f;

					float pillX = offset[0] - padH;
					float pillY = y + mSelectorOffsetY + (entrySize - pillH) / 2.0f;

					// Long entries shouldn't push the highlight past the list's own bounds
					// (e.g. behind the battery pill in the corner help bar). The list's own
					// edge is already the safe boundary, so clamp to it directly rather than
					// insetting further.
					float maxPillRight = mSize.x();
					if (pillX + pillW > maxPillRight)
						pillW = Math::max(0.0f, maxPillRight - pillX);

					mSelectorPill.setColor(mSelectorPillColor);
					mSelectorPill.setSize(pillW, pillH);
					mSelectorPill.setPosition(pillX, pillY);

					// The pill intentionally extends past the text bounds for padding, so it
					// shouldn't be clipped by the list's own content clip rect - suspend
					// clipping for just this one draw call.
					Renderer::popClipRect();
					mSelectorPill.render(trans);
					Renderer::pushClipRect(rect);
				}
				else if (mSelectorImage.hasImage())
				{
					//mSelectorImage.setPosition(0.f, y, 0.f);
					mSelectorImage.setPosition(0.f, y + mSelectorOffsetY, 0.f);
					mSelectorImage.render(trans);
				}
				else
				{
					Renderer::setMatrix(trans);
					Renderer::drawRect(0.0f, y + mSelectorOffsetY, mSize.x(), mSelectorHeight,
						mSelectorColor & 0xFFFFFF00 | (unsigned char)((mSelectorColor & 0xFF) * opacity),
						mSelectorColorEnd & 0xFFFFFF00 | (unsigned char)((mSelectorColorEnd & 0xFF) * opacity),
						mSelectorColorGradientHorizontal);
				}
			}
			else if (i == mHotRow)
			{
				Renderer::setMatrix(trans);

				float hotOpacity = 0.1f;
				Renderer::drawRect(0.0f, y + mSelectorOffsetY, mSize.x(), entrySize,
					mSelectorColor & 0xFFFFFF00 | (unsigned char)((mSelectorColor & 0xFF) * opacity * hotOpacity),
					mSelectorColorEnd & 0xFFFFFF00 | (unsigned char)((mSelectorColorEnd & 0xFF) * opacity * hotOpacity),
					mSelectorColorGradientHorizontal);
			}

			font->renderTextCacheEx(activeCache, drawTrans, mGlowSize, mGlowColor, mGlowOffset, getOpacity());

			// render currently selected item text again if
			// marquee is scrolled far enough for it to repeat
			if (mCursor == i && mMarqueeOffset2 < 0)
			{
				drawTrans = trans;
				drawTrans.translate(offset - Vector3f((float)mMarqueeOffset2, 0, 0));

				font->renderTextCacheEx(activeCache, drawTrans, mGlowSize, mGlowColor, mGlowOffset, getOpacity());
			}
		}

		if (mDebugShowRowBounds)
		{
			Renderer::setMatrix(trans);
			Renderer::drawSolidRectangle(0.0f, y, mSize.x(), entrySize, 0x00000000, mDebugRowBoundsColor, 1.0f, 0.0f);
		}

		y += entrySize;
		if (mItemTemplate.type.empty() && y > mSize.y())
			break;
	}

	Renderer::popClipRect();

	listRenderTitleOverlay(trans);

	GuiComponent::renderChildren(trans);

	if (mScrollbar.isEnabled())
	{
		mScrollbar.setContainerBounds(GuiComponent::getPosition(), GuiComponent::getSize());
		mScrollbar.setRange(0, entrySize * mEntries.size(), mSize.y());
		mScrollbar.setScrollPosition(startEntry * entrySize);
		mScrollbar.render(parentTrans);
	}
}

template <typename T>
bool TextListComponent<T>::input(InputConfig* config, Input input)
{
	if (size() > 0)
	{
		if (input.value != 0)
		{
			if (config->isMappedLike("down", input))
			{
				listInput(1);
				return true;
			}

			if (config->isMappedLike("up", input))
			{
				listInput(-1);
				return true;
			}
			if (config->isMappedTo("rightshoulder", input))
			{
				listInput(10);
				return true;
			}

			if (config->isMappedTo("leftshoulder", input))
			{
				listInput(-10);
				return true;
			}
		}
		else {
			if (config->isMappedLike("down", input) || config->isMappedLike("up", input) ||
				config->isMappedLike("rightshoulder", input) || config->isMappedLike("leftshoulder", input))
			{
				stopScrolling();
			}
		}
	}

	return GuiComponent::input(config, input);
}

template <typename T>
void TextListComponent<T>::update(int deltaTime)
{
	if (mTimeHoldingButton >= 0)
	{
		mTimeHoldingButton += deltaTime;

		if (mTimeHoldingButton >= HOLD_TIME)
		{
			mTimeHoldingButton = -1;
			mHotRow = -1;
			mPressedPoint = Vector2i(-1, -1);

			longMouseClick.invoke([&](ILongMouseClickEvent* c) { c->onLongMouseClick(this); });
		}
	}

	mScrollbar.update(deltaTime);

	if (!mItemTemplate.type.empty())
	{
		for (unsigned int i = 0; i < mEntries.size(); i++)
		{
			auto& entry = mEntries.at(i);
			if (entry.data.itemTemplate && entry.data.itemTemplate->isShowing())
				entry.data.itemTemplate->update(deltaTime);
		}
	}

	listUpdate(deltaTime);

	if (!isScrolling() && size() > 0 && !(mSelectorPillMode && mSelectorPillEllipsisMode))
	{
		// always reset the marquee offsets
		mMarqueeOffset = 0;
		mMarqueeOffset2 = 0;

		std::string name = mEntries.at((unsigned int)mCursor).name;
		if (mUppercase)
			name = Utils::String::toUpper(name);

		// if we're not scrolling and this object's text goes outside our size, marquee it!
		const float textLength = mFont->sizeText(name).x();
		const float limit = mSize.x() - mHorizontalMargin * 2;

		if (textLength > limit)
		{
			// loop
			// pixels per second ( based on nes-mini font at 1920x1080 to produce a speed of 200 )
			// Pill mode scrolls at half speed, since it also starts instantly rather than easing in.
			const float speed = (mFont->sizeText("ABCDEFGHIJKLMNOPQRSTUVWXYZ").x() * 0.247f) * (mSelectorPillMode ? 0.5f : 1.0f);
			// Pill mode starts scrolling the instant an item is highlighted, skipping the
			// usual pause before the first pass.
			const float delay = mSelectorPillMode ? 0.0f : 3000.0f;
			const float scrollLength = textLength;
			const float returnLength = speed * 1.5f;
			const float scrollTime = (scrollLength * 1000) / speed;
			const float returnTime = (returnLength * 1000) / speed;
			const int   maxTime = (int)(delay + scrollTime + returnTime);

			mMarqueeTime += deltaTime;
			while (mMarqueeTime > maxTime)
				mMarqueeTime -= maxTime;

			mMarqueeOffset = (int)(Math::Scroll::loop(delay, scrollTime + returnTime, (float)mMarqueeTime, scrollLength + returnLength));

			if (mMarqueeOffset > (scrollLength - (limit - returnLength)))
				mMarqueeOffset2 = (int)(mMarqueeOffset - (scrollLength + returnLength));
		}
	}

	GuiComponent::update(deltaTime);
}

//list management stuff
template <typename T>
void TextListComponent<T>::add(const std::string& name, const T& obj, unsigned int color)
{
	assert(color < COLOR_ID_COUNT);

	typename IList<TextListData, T>::Entry entry;
	entry.name = name;
	entry.object = obj;
	entry.data.colorId = color;
	static_cast<IList< TextListData, T >*>(this)->add(entry);
}

template <typename T>
void TextListComponent<T>::onSizeChanged()
{
	IList<TextListData, T>::onSizeChanged();
	updateCameraOffset();
}

template <typename T>
void TextListComponent<T>::updateCameraOffset()
{
	auto rowHeight = getRowHeight();
	auto totalHeight = rowHeight * mEntries.size();

	if (mSelectorPillMode)
	{
		// No centering - the list stays put while the cursor moves within the visible
		// rows, and only scrolls (by however many rows are needed, normally just one) once
		// the cursor would otherwise move off the top or bottom of the screen.
		if (totalHeight > mSize.y() && mCursor < mEntries.size())
		{
			// render() always draws one extra "peek" row beyond this count (see lastEntry in
			// render()), so that's the true number of rows visible on screen - match it here
			// too, or the window scrolls further than necessary once it does trigger.
			int visibleRows = (mLineCount > 0 ? mLineCount : (int)(mSize.y() / rowHeight)) + 1;
			int maxTopEntry = Math::max(0, (int)mEntries.size() - visibleRows);

			mPillTopEntry = Math::min(mPillTopEntry, maxTopEntry);
			mPillTopEntry = Math::max(mPillTopEntry, 0);

			if ((int)mCursor < mPillTopEntry)
				mPillTopEntry = (int)mCursor;
			else if ((int)mCursor >= mPillTopEntry + visibleRows)
				mPillTopEntry = (int)mCursor - visibleRows + 1;

			mCameraOffset = mPillTopEntry * rowHeight;
		}
		else
		{
			mPillTopEntry = 0;
			mCameraOffset = 0;
		}

		return;
	}

	// move the camera to scroll
	if (totalHeight > mSize.y() && mCursor < mEntries.size())
	{
		mCameraOffset = (rowHeight * mCursor) + ((rowHeight - mSize.y()) / 2.0f);

		if (mCameraOffset < 0)
			mCameraOffset = 0;
		else if (mCameraOffset + mSize.y() > totalHeight)
			mCameraOffset = totalHeight - mSize.y();
	}
	else
		mCameraOffset = 0;
}

template <typename T>
float TextListComponent<T>::getRowHeight() const
{
	return mLineCount > 0 ? (mSize.y() / (float) mLineCount) : (Math::max(mFont->getHeight(1.0), (float)mFont->getSize()) * mLineSpacing);
}

template <typename T>
float TextListComponent<T>::getTotalRowHeight() const
{
	return getRowHeight() * mEntries.size();
}

template <typename T>
void TextListComponent<T>::onCursorChanged(const CursorState& state)
{
	// onCursorChanged fires again with CURSOR_STOPPED once the key is released, even when the
	// cursor didn't move any further that time - only reset the marquee on an actual cursor
	// change, or a mid-scroll marquee snaps back to its start and appears to twitch.
	if (mLastCursor != mCursor)
	{
		mMarqueeOffset = 0;
		mMarqueeOffset2 = 0;
		mMarqueeTime = 0;
	}

	mScrollbar.onCursorChanged();

	updateCameraOffset();

	if (mLastCursor != mCursor || mLastCursorState != state)
	{
		if (mCursor >= 0 && mCursor < mEntries.size())
		{
			typename IList<TextListData, T>::Entry& entry = mEntries.at(mCursor);
			if (entry.data.itemTemplate)
			{
				if (entry.data.itemTemplate->selectStoryboard("activate") || entry.data.itemTemplate->selectStoryboard())
				{
					entry.data.itemTemplate->startStoryboard();
					entry.data.itemTemplate->update(0);
				}
			}
		}

		if (mLastCursor >= 0 && mLastCursor != mCursor && mLastCursor < mEntries.size())
		{
			typename IList<TextListData, T>::Entry& entry = mEntries.at(mLastCursor);
			if (entry.data.itemTemplate)
			{
				if (entry.data.itemTemplate->selectStoryboard("deactivate") || entry.data.itemTemplate->selectStoryboard())
				{
					entry.data.itemTemplate->startStoryboard();
					entry.data.itemTemplate->update(0);
				} 
			}
		}

		if (mCursorChangedCallback)
			mCursorChangedCallback(state);

		mLastCursor = mCursor;
		mLastCursorState = state;
	}
}

template <typename T>
void TextListComponent<T>::applyTheme(const std::shared_ptr<ThemeData>& theme, const std::string& view, const std::string& element, unsigned int properties)
{
	GuiComponent::applyTheme(theme, view, element, properties);

	const ThemeData::ThemeElement* elem = theme->getElement(view, element, "textlist");
	if (!elem)
		return;

	mScrollbar.fromTheme(theme, view, element, "textlist");


	if (elem)
	{
		auto itemTemplate = std::find_if(elem->children.cbegin(), elem->children.cend(), [](const std::pair<std::string, ThemeData::ThemeElement> ss) { return ss.first == "itemTemplate"; });
		if (itemTemplate != elem->children.cend())
			mItemTemplate = itemTemplate->second;
	}


	using namespace ThemeFlags;
	if (properties & COLOR)
	{
		if (elem->has("selectorColor"))
		{
			setSelectorColor(elem->get<unsigned int>("selectorColor"));
			setSelectorColorEnd(elem->get<unsigned int>("selectorColor"));
		}
		if (elem->has("selectorColorEnd"))
			setSelectorColorEnd(elem->get<unsigned int>("selectorColorEnd"));
		if (elem->has("selectorGradientType"))
			setSelectorColorGradientHorizontal(elem->get<std::string>("selectorGradientType").compare("horizontal"));
		if (elem->has("selectedColor"))
			setSelectedColor(elem->get<unsigned int>("selectedColor"));
		if (elem->has("primaryColor"))
			setColor(0, elem->get<unsigned int>("primaryColor"));
		if (elem->has("secondaryColor"))
			setColor(1, elem->get<unsigned int>("secondaryColor"));
		if (elem->has("extraTextColor"))
			setBonusTextColor(elem->get<unsigned int>("extraTextColor"));
		if (elem->has("extraTextSelectedColor"))
			setBonusSelectedTextColor(elem->get<unsigned int>("extraTextSelectedColor"));

		if (elem->has("glowColor"))
			mGlowColor = elem->get<unsigned int>("glowColor");
		else
			mGlowColor = 0;

		if (elem->has("glowSize"))
			mGlowSize = (int)elem->get<float>("glowSize");

		if (elem->has("glowOffset"))
			mGlowOffset = elem->get<Vector2f>("glowOffset");
	}

	setFont(Font::getFromTheme(elem, properties, mFont));
	const float selectorHeight = Math::max(mFont->getHeight(1.0), (float)mFont->getSize()) * mLineSpacing;
	setSelectorHeight(selectorHeight);

	if (properties & SOUND && elem->has("scrollSound"))
		mScrollSound = elem->get<std::string>("scrollSound");

	if (properties & ALIGNMENT)
	{
		if (elem->has("alignment"))
		{
			const std::string& str = elem->get<std::string>("alignment");
			if (str == "left")
				setAlignment(ALIGN_LEFT);
			else if (str == "center")
				setAlignment(ALIGN_CENTER);
			else if (str == "right")
				setAlignment(ALIGN_RIGHT);
			else
				LOG(LogError) << "Unknown TextListComponent alignment \"" << str << "\"!";
		}

		if (elem->has("horizontalMargin"))
			mHorizontalMargin = elem->get<float>("horizontalMargin") * (this->mParent ? this->mParent->getSize().x() : (float)Renderer::getScreenWidth());
	}

	if (properties & FORCE_UPPERCASE && elem->has("forceUppercase"))
		setUppercase(elem->get<bool>("forceUppercase"));

	if (properties & LINE_SPACING)
	{
		if (elem->has("lines"))
		{
			mLineCount = (int)elem->get<float>("lines");
			if (mLineCount > 0)
				setSelectorHeight(mSize.y() / mLineCount);
		}
		else
			mLineCount = -1;

		if (elem->has("lineSpacing"))
			setLineSpacing(elem->get<float>("lineSpacing"));

		if (elem->has("selectorHeight"))
			setSelectorHeight(elem->get<float>("selectorHeight") * Renderer::getScreenHeight());

		if (elem->has("selectorOffsetY"))
		{
			float scale = this->mParent ? this->mParent->getSize().y() : (float)Renderer::getScreenHeight();
			setSelectorOffsetY(elem->get<float>("selectorOffsetY") * scale);
		}
		else
			setSelectorOffsetY(0.0);
	}

	if (elem->has("selectorImagePath"))
	{
		std::string path = elem->get<std::string>("selectorImagePath");
		bool tile = elem->has("selectorImageTile") && elem->get<bool>("selectorImageTile");
		mSelectorImage.setImage(path, tile);
		mSelectorImage.setSize(mSize.x(), mSelectorHeight);
		mSelectorImage.setColorShift(mSelectorColor);
		mSelectorImage.setColorShiftEnd(mSelectorColorEnd);
	}
	else {
		mSelectorImage.setImage("");
	}
}

template <typename T>
bool TextListComponent<T>::hitTest(int x, int y, Transform4x4f& parentTransform, std::vector<GuiComponent*>* pResult)
{
	Transform4x4f trans = parentTransform * getTransform();

	bool ret = false;

	mHotRow = -1;

	auto rect = Renderer::getScreenRect(trans, getSize(), true);
	if (x != -1 && y != -1 && rect.contains(x, y))
	{
		ret = true;

		Transform4x4f ti = trans;
		ti.translate(0, -mCameraOffset);

		float ry = 0;
		float rowHeight = getRowHeight();
		Vector2f itemSize(getSize().x(), rowHeight);

		for (unsigned int i = 0; i < mEntries.size(); i++)
		{
			auto& entry = mEntries.at(i);

			if (ry - mCameraOffset + rowHeight >= 0)
			{
				rect = Renderer::getScreenRect(ti, itemSize, true);
				if (rect.contains(x, y))
				{
					mHotRow = i;
					break;
				}
			}

			ti.translate(0, rowHeight);
			ry += rowHeight;
			if (ry - mCameraOffset > mSize.y())
				break;
		}

		if (pResult != nullptr)
			pResult->push_back(this);

		trans.translate(0, -mCameraOffset);

		for (int i = 0; i < getChildCount(); i++)
			ret |= getChild(i)->hitTest(x, y, trans, pResult);
	}

	return ret;
}

template <typename T>
bool TextListComponent<T>::onMouseClick(int button, bool pressed, int x, int y)
{
	if (button == 1)
	{
		mTimeHoldingButton = -1;

		if (pressed)
		{
			if (mHotRow == mCursor)
				mPressedRow = mHotRow;
			else
				mPressedRow = -1;

			if (mHotRow >= 0)
			{
				float camOffset = mCameraOffset;
				mCursor = mHotRow;
				onCursorChanged(CURSOR_STOPPED);
				mCameraOffset = camOffset;			

				mTimeHoldingButton = 0;
			}

			mPressedPoint = Vector2i(x, y);
			mWindow->setMouseCapture(this);
		}
		else if (mWindow->hasMouseCapture(this))
		{
			mWindow->releaseMouseCapture();

			auto row = mPressedRow;
			mPressedRow = -1;
			mPressedPoint = Vector2i(-1, -1);
			mTimeHoldingButton = -1;

			if (row == mHotRow && mCursor == row)
				InputManager::getInstance()->sendMouseClick(mWindow, 1);
		}

		return true;
	}

	return false;
}

template <typename T>
void TextListComponent<T>::onMouseMove(int x, int y)
{
	if (mPressedPoint.x() != -1 && mPressedPoint.y() != -1 && mWindow->hasMouseCapture(this))
	{
		mTimeHoldingButton = -1;
		mHotRow = -1;

		float yOffset = getRowHeight() * mEntries.size();
		yOffset -= getSize().y();
		if (yOffset <= 0)
			return;

		mCameraOffset += mPressedPoint.y() - y;

		if (mCameraOffset < 0)
			mCameraOffset = 0;

		if (mCameraOffset > yOffset)
			mCameraOffset = yOffset;

		mPressedPoint = Vector2i(x, y);
	}
}

template <typename T>
bool TextListComponent<T>::onMouseWheel(int delta)
{
	listInput(-delta);
	mScrollVelocity = 0;
	mTimeHoldingButton = -1;
	mHotRow = -1;
	return true;
}

template<typename T>
void TextListComponent<T>::setOpacity(unsigned char opacity)
{
	IList<TextListData, T>::setOpacity(opacity);

	if (!mItemTemplate.type.empty())
	{
		for (unsigned int i = 0; i < mEntries.size(); i++)
		{
			auto& entry = mEntries.at(i);
			if (entry.data.itemTemplate != nullptr)
				entry.data.itemTemplate->setOpacity(opacity);
		}
	}
}

template<typename T>
void TextListComponent<T>::onShow()
{
	IList<TextListData, T>::onShow();	

	if (!mItemTemplate.type.empty())
	{
		for (unsigned int i = 0; i < mEntries.size(); i++)
		{
			auto& entry = mEntries.at(i);
			if (entry.data.itemTemplate)
				entry.data.itemTemplate->onShow();
		}
	}

	mMarqueeOffset = 0;
	mMarqueeOffset2 = 0;
	mMarqueeTime = 0;

	mScrollbar.onCursorChanged();
}

template<typename T>
void TextListComponent<T>::onHide()
{
	IList<TextListData, T>::onHide();

	if (!mItemTemplate.type.empty())
	{
		for (unsigned int i = 0; i < mEntries.size(); i++)
		{
			auto& entry = mEntries.at(i);
			if (entry.data.itemTemplate)
				entry.data.itemTemplate->onHide();
		}
	}
}

#endif // ES_APP_COMPONENTS_TEXT_LIST_COMPONENT_H
