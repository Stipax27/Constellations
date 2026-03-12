#ifndef _TEXTLABEL_H_
#define _TEXTLABEL_H_

#include "component.h"
#include "point3d.h"

constexpr wchar_t DefaultUIFontFamilyW[] = L"Arial";

struct TextLabel : Component
{
	wstring textW = L"TextLabel";

	wstring fontFamilyW;
	wstring fallbackFontFamilyW = DefaultUIFontFamilyW;

	wstring fontFilePathW;

	int fontSizePx = 32;
	int fontWeight = 400;

	float fontScale = 1.0f;

	float letterSpacingPx = 0.0f;
	float lineSpacing = 1.0f;

	point3d color = point3d(1, 1, 1);
	float opacity = 1.0f;

	bool centered = false;

	int maxCharacters = -1;

	bool useTransformScale = false;
};

#endif