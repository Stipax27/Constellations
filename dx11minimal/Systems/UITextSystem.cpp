#include "UITextSystem.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>

#include <Windows.h>
#include <wingdi.h>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#include "../utils.h"

namespace {

constexpr int kUITextVertexShader = 13;
constexpr int kUITextPixelShader = 25;

struct GlyphInfo
{
	float u0 = 0.0f;
	float v0 = 0.0f;
	float u1 = 0.0f;
	float v1 = 0.0f;

	int width = 0;
	int height = 0;
	int bearingX = 0;
	int bearingY = 0;
	int advance = 0;

	bool hasBitmap = false;
};

struct FontAtlas
{
	int textureId = -1;
	int atlasWidth = 1024;
	int atlasHeight = 1024;
	int ascent = 0;
	int lineHeight = 0;
	std::unordered_map<wchar_t, GlyphInfo> glyphs;
};

struct FontKey
{
	wstring fontName;
	wstring fallbackFontName;
	wstring fontFilePath;
	int fontSizePx = 32;
	int fontWeight = 400;

	bool operator==(const FontKey& other) const
	{
		return fontSizePx == other.fontSizePx &&
			fontWeight == other.fontWeight &&
			fontName == other.fontName &&
			fallbackFontName == other.fallbackFontName &&
			fontFilePath == other.fontFilePath;
	}
};

struct FontKeyHash
{
	size_t operator()(const FontKey& key) const
	{
		size_t h = std::hash<wstring>{}(key.fontName);
		h ^= std::hash<wstring>{}(key.fallbackFontName) + 0x9e3779b9 + (h << 6) + (h >> 2);
		h ^= std::hash<wstring>{}(key.fontFilePath) + 0x9e3779b9 + (h << 6) + (h >> 2);
		h ^= std::hash<int>{}(key.fontSizePx) + 0x9e3779b9 + (h << 6) + (h >> 2);
		h ^= std::hash<int>{}(key.fontWeight) + 0x9e3779b9 + (h << 6) + (h >> 2);
		return h;
	}
};

std::unordered_map<FontKey, FontAtlas, FontKeyHash> g_FontCache;
std::unordered_map<wstring, int> g_PrivateFontRefs;

wstring ResolveFallbackFontName(const TextLabel& textLabel)
{
	if (!textLabel.fallbackFontFamilyW.empty()) {
		return textLabel.fallbackFontFamilyW;
	}

	return DefaultUIFontFamilyW;
}

wstring ResolveFontName(const TextLabel& textLabel)
{
	if (!textLabel.fontFamilyW.empty()) {
		return textLabel.fontFamilyW;
	}

	return ResolveFallbackFontName(textLabel);
}

wstring ResolveFontPath(const TextLabel& textLabel)
{
	return textLabel.fontFilePathW;
}

wstring ResolveText(const TextLabel& textLabel)
{
	return textLabel.textW;
}

bool IsSupportedChar(wchar_t ch)
{
	if (ch == L'\n') {
		return true;
	}

	if (ch >= 32 && ch <= 126) {
		return true;
	}

	if (ch >= 0x0400 && ch <= 0x04FF) {
		return true;
	}

	if (ch == 0x2116) {
		return true;
	}

	return false;
}

wstring NormalizeToSupportedCharset(const wstring& source)
{
	wstring normalized;
	normalized.reserve(source.size());

	for (wchar_t ch : source) {
		normalized.push_back(IsSupportedChar(ch) ? ch : L'?');
	}

	return normalized;
}

std::vector<wchar_t> BuildGlyphList()
{
	std::vector<wchar_t> glyphs;
	glyphs.reserve(400);

	for (int ch = 32; ch <= 126; ch++) {
		glyphs.push_back((wchar_t)ch);
	}

	for (int ch = 0x0400; ch <= 0x04FF; ch++) {
		glyphs.push_back((wchar_t)ch);
	}

	glyphs.push_back((wchar_t)0x2116);
	glyphs.push_back(L'?');
	return glyphs;
}

bool EnsurePrivateFontLoaded(const wstring& fontFilePath)
{
	if (fontFilePath.empty()) {
		return true;
	}

	auto it = g_PrivateFontRefs.find(fontFilePath);
	if (it != g_PrivateFontRefs.end()) {
		it->second += 1;
		return true;
	}

	const int addedCount = AddFontResourceExW(fontFilePath.c_str(), FR_PRIVATE, nullptr);
	if (addedCount <= 0) {
		return false;
	}

	g_PrivateFontRefs[fontFilePath] = 1;
	return true;
}

void ReleasePrivateFonts()
{
	for (auto& pair : g_PrivateFontRefs) {
		const wstring& fontFilePath = pair.first;
		int refsCount = pair.second;
		for (int i = 0; i < refsCount; i++) {
			RemoveFontResourceExW(fontFilePath.c_str(), FR_PRIVATE, nullptr);
		}
	}

	g_PrivateFontRefs.clear();
}

void SetupUiTransformConstants(const Transform2D& transform2D)
{
	ConstBuf::global[0] = XMFLOAT4(transform2D.position.x, transform2D.position.y, transform2D.position.z, 0.0f);
	ConstBuf::global[1] = XMFLOAT4(transform2D.scale.x, transform2D.scale.y, 0.0f, 0.0f);
	ConstBuf::global[2] = XMFLOAT4(transform2D.anchorPoint.x, transform2D.anchorPoint.y, transform2D.rotation, 0.0f);

	switch (transform2D.ratio)
	{
	case ScreenAspectRatio::XY:
		ConstBuf::global[1].z = 1.0f;
		ConstBuf::global[1].w = 1.0f;
		break;
	case ScreenAspectRatio::YX:
		ConstBuf::global[1].z = ConstBuf::frame.aspect.x;
		ConstBuf::global[1].w = ConstBuf::frame.aspect.y;
		break;
	case ScreenAspectRatio::XX:
		ConstBuf::global[1].z = 1.0f;
		ConstBuf::global[1].w = ConstBuf::frame.aspect.y;
		break;
	case ScreenAspectRatio::YY:
		ConstBuf::global[1].z = ConstBuf::frame.aspect.x;
		ConstBuf::global[1].w = 1.0f;
		break;
	}

	ConstBuf::Update(5, ConstBuf::global);
	ConstBuf::ConstToVertex(5);
	ConstBuf::ConstToPixel(5);
}

bool BuildFontAtlas(const FontKey& key, FontAtlas& outAtlas)
{
	const bool hasExternalFontFile = !key.fontFilePath.empty();
	const bool isExternalFontLoaded = EnsurePrivateFontLoaded(key.fontFilePath);

	HDC hdc = CreateCompatibleDC(nullptr);
	if (hdc == nullptr) {
		return false;
	}

	const int fontSizePx = key.fontSizePx > 2 ? key.fontSizePx : 2;
	const int fontWeight = std::max(1, std::min(1000, key.fontWeight));
	const wchar_t* fontName = key.fontName.empty() ? L"Arial" : key.fontName.c_str();
	if (hasExternalFontFile && !isExternalFontLoaded) {
		fontName = key.fallbackFontName.empty() ? L"Arial" : key.fallbackFontName.c_str();
	}
	HFONT font = CreateFontW(
		-fontSizePx,
		0,
		0,
		0,
		fontWeight,
		FALSE,
		FALSE,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		fontName);

	if (font == nullptr) {
		DeleteDC(hdc);
		return false;
	}

	HGDIOBJ oldFont = SelectObject(hdc, font);
	SetBkMode(hdc, TRANSPARENT);

	TEXTMETRICW tm = {};
	GetTextMetricsW(hdc, &tm);

	outAtlas.ascent = tm.tmAscent;
	outAtlas.lineHeight = tm.tmHeight;
	outAtlas.atlasWidth = key.fontSizePx > 96 ? 2048 : 1024;
	outAtlas.atlasHeight = outAtlas.atlasWidth;

	const int atlasWidth = outAtlas.atlasWidth;
	const int atlasHeight = outAtlas.atlasHeight;
	std::vector<unsigned char> atlasPixels(atlasWidth * atlasHeight * 4, 0);

	MAT2 mat = {};
	mat.eM11.value = 1;
	mat.eM12.value = 0;
	mat.eM21.value = 0;
	mat.eM22.value = 1;

	const std::vector<wchar_t> glyphList = BuildGlyphList();

	int penX = 1;
	int penY = 1;
	int rowHeight = 0;

	for (wchar_t ch : glyphList) {
		GlyphInfo glyphInfo;

		GLYPHMETRICS glyphMetrics = {};
		DWORD bitmapSize = GetGlyphOutlineW(hdc, (UINT)ch, GGO_GRAY8_BITMAP, &glyphMetrics, 0, nullptr, &mat);
		if (bitmapSize == GDI_ERROR) {
			SIZE size = {};
			GetTextExtentPoint32W(hdc, &ch, 1, &size);
			glyphInfo.advance = size.cx;
			outAtlas.glyphs[ch] = glyphInfo;
			continue;
		}

		glyphInfo.width = (int)glyphMetrics.gmBlackBoxX;
		glyphInfo.height = (int)glyphMetrics.gmBlackBoxY;
		glyphInfo.bearingX = glyphMetrics.gmptGlyphOrigin.x;
		glyphInfo.bearingY = glyphMetrics.gmptGlyphOrigin.y;
		glyphInfo.advance = glyphMetrics.gmCellIncX;
		if (glyphInfo.advance <= 0) {
			glyphInfo.advance = glyphInfo.width;
		}

		if (glyphInfo.width > 0 && glyphInfo.height > 0) {
			if (penX + glyphInfo.width + 1 >= atlasWidth) {
				penX = 1;
				penY += rowHeight + 1;
				rowHeight = 0;
			}

			if (penY + glyphInfo.height + 1 >= atlasHeight) {
				SelectObject(hdc, oldFont);
				DeleteObject(font);
				DeleteDC(hdc);
				return false;
			}

			std::vector<unsigned char> glyphBitmap(bitmapSize, 0);
			if (bitmapSize > 0) {
				GetGlyphOutlineW(hdc, (UINT)ch, GGO_GRAY8_BITMAP, &glyphMetrics, bitmapSize, glyphBitmap.data(), &mat);
			}

			int pitch = ((glyphInfo.width + 3) / 4) * 4;
			if (pitch <= 0) {
				pitch = glyphInfo.width;
			}

			if (glyphInfo.height > 0) {
				const int maxPitchByBuffer = (int)glyphBitmap.size() / glyphInfo.height;
				if (maxPitchByBuffer > 0 && pitch > maxPitchByBuffer) {
					pitch = maxPitchByBuffer;
				}
			}

			for (int y = 0; y < glyphInfo.height; y++) {
				for (int x = 0; x < glyphInfo.width; x++) {
					const int srcIndex = y * pitch + x;
					unsigned char alpha = 0;
					if (srcIndex >= 0 && srcIndex < (int)glyphBitmap.size()) {
						alpha = glyphBitmap[srcIndex];
					}

					int alphaInt = (int)alpha * 4;
					if (alphaInt > 255) {
						alphaInt = 255;
					}
					alpha = (unsigned char)alphaInt;

					const int dstIndex = ((penY + y) * atlasWidth + (penX + x)) * 4;
					if (dstIndex >= 0 && dstIndex + 3 < (int)atlasPixels.size()) {
						atlasPixels[dstIndex + 0] = 255;
						atlasPixels[dstIndex + 1] = 255;
						atlasPixels[dstIndex + 2] = 255;
						atlasPixels[dstIndex + 3] = alpha;
					}
				}
			}

			glyphInfo.u0 = (float)penX / (float)atlasWidth;
			glyphInfo.v0 = (float)penY / (float)atlasHeight;
			glyphInfo.u1 = (float)(penX + glyphInfo.width) / (float)atlasWidth;
			glyphInfo.v1 = (float)(penY + glyphInfo.height) / (float)atlasHeight;
			glyphInfo.hasBitmap = true;

			penX += glyphInfo.width + 2;
			rowHeight = std::max(rowHeight, glyphInfo.height);
		}

		outAtlas.glyphs[ch] = glyphInfo;
	}

	SelectObject(hdc, oldFont);
	DeleteObject(font);
	DeleteDC(hdc);

	const int textureId = Textures::texturesCount;
	Textures::Create(textureId, Textures::tType::flat, Textures::tFormat::u8, XMFLOAT2((float)atlasWidth, (float)atlasHeight), true, false);
	context->UpdateSubresource(Textures::Texture[textureId].pTexture, 0, nullptr, atlasPixels.data(), atlasWidth * 4, 0);
	context->GenerateMips(Textures::Texture[textureId].TextureResView);

	outAtlas.textureId = textureId;
	return true;
}

const FontAtlas* GetOrCreateFontAtlas(const TextLabel& textLabel)
{
	FontKey key;
	key.fontName = ResolveFontName(textLabel);
	key.fallbackFontName = ResolveFallbackFontName(textLabel);
	key.fontFilePath = ResolveFontPath(textLabel);
	key.fontSizePx = std::max(textLabel.fontSizePx, 8);
	key.fontWeight = textLabel.fontWeight;

	auto it = g_FontCache.find(key);
	if (it != g_FontCache.end()) {
		return &it->second;
	}

	FontAtlas atlas;
	if (!BuildFontAtlas(key, atlas)) {
		return nullptr;
	}

	auto result = g_FontCache.emplace(key, std::move(atlas));
	return &result.first->second;
}

const GlyphInfo* FindGlyph(const FontAtlas& atlas, wchar_t ch)
{
	auto it = atlas.glyphs.find(ch);
	if (it != atlas.glyphs.end()) {
		return &it->second;
	}

	it = atlas.glyphs.find(L'?');
	if (it != atlas.glyphs.end()) {
		return &it->second;
	}

	return nullptr;
}

float MeasureLineWidthPx(const FontAtlas& atlas, const wstring& text, int begin, int end, float scale, float letterSpacingPx)
{
	float widthPx = 0.0f;
	for (int i = begin; i < end; i++) {
		const GlyphInfo* glyph = FindGlyph(atlas, text[i]);
		if (glyph != nullptr) {
			widthPx += glyph->advance * scale + letterSpacingPx;
		}
	}

	if (widthPx > 0.0f) {
		widthPx -= letterSpacingPx;
	}

	return widthPx;
}

void DrawGlyphQuad(
	const GlyphInfo& glyph,
	const Transform2D& originTransform,
	const TextLabel& textLabel,
	float leftPx,
	float topPx,
	float widthPx,
	float heightPx,
	float screenWidth,
	float screenHeight)
{
	if (widthPx <= 0.0f || heightPx <= 0.0f) {
		return;
	}

	Transform2D glyphTransform;
	glyphTransform.position = point3d(
		originTransform.position.x + ((leftPx + widthPx * 0.5f) * 2.0f / screenWidth),
		originTransform.position.y - ((topPx + heightPx * 0.5f) * 2.0f / screenHeight),
		originTransform.position.z);
	glyphTransform.anchorPoint = point3d();
	glyphTransform.scale = point3d(widthPx / screenWidth, heightPx / screenHeight, 0.0f);
	glyphTransform.rotation = 0.0f;
	glyphTransform.ratio = ScreenAspectRatio::XY;

	ConstBuf::global[3] = XMFLOAT4(
		textLabel.color.x,
		textLabel.color.y,
		textLabel.color.z,
		std::max(0.0f, std::min(1.0f, textLabel.opacity)));
	ConstBuf::global[4] = XMFLOAT4(glyph.u0, glyph.v0, glyph.u1, glyph.v1);

	SetupUiTransformConstants(glyphTransform);
	Draw::Drawer(1);
}

void RenderTextLabel(const Transform2D& transform, const TextLabel& textLabel, const FontAtlas& atlas, const wstring& text, float screenWidth, float screenHeight)
{
	if (text.empty()) {
		return;
	}

	float scale = textLabel.fontScale;
	if (textLabel.useTransformScale) {
		float transformScale = std::max(fabsf(transform.scale.x), fabsf(transform.scale.y));
		scale *= transformScale;
	}

	if (scale <= 0.0f || textLabel.maxCharacters == 0) {
		return;
	}

	wstring textToRender = text;
	if (textLabel.maxCharacters >= 0 && textLabel.maxCharacters < (int)textToRender.size()) {
		textToRender.resize(textLabel.maxCharacters);
	}

	Shaders::vShader(kUITextVertexShader);
	Shaders::pShader(kUITextPixelShader);
	InputAssembler::IA(InputAssembler::topology::triList);
	Textures::TextureToShader(atlas.textureId, 0, targetshader::pixel);
	Sampler::Sampler(targetshader::pixel, 0, Sampler::filter::linear, Sampler::addr::clamp, Sampler::addr::clamp);

	const float letterSpacingPx = textLabel.letterSpacingPx;
	const float lineSpacing = std::max(0.1f, textLabel.lineSpacing);
	const float lineHeightPx = atlas.lineHeight * scale * lineSpacing;

	float lineTopPx = 0.0f;
	int lineStart = 0;
	const int textLength = (int)textToRender.size();

	for (int i = 0; i <= textLength; i++) {
		const bool isLineBreak = i == textLength || (i < textLength && textToRender[i] == L'\n');
		if (!isLineBreak) {
			continue;
		}

		const int lineEnd = i;
		const float lineWidthPx = MeasureLineWidthPx(atlas, textToRender, lineStart, lineEnd, scale, letterSpacingPx);
		float penX = textLabel.centered ? -lineWidthPx * 0.5f : 0.0f;
		const float baselinePx = lineTopPx + atlas.ascent * scale;

		for (int j = lineStart; j < lineEnd; j++) {
			const GlyphInfo* glyph = FindGlyph(atlas, textToRender[j]);
			if (glyph != nullptr && glyph->hasBitmap) {
				const float glyphLeftPx = penX + glyph->bearingX * scale;
				const float glyphTopPx = baselinePx - glyph->bearingY * scale;
				const float glyphWidthPx = glyph->width * scale;
				const float glyphHeightPx = glyph->height * scale;
				DrawGlyphQuad(*glyph, transform, textLabel, glyphLeftPx, glyphTopPx, glyphWidthPx, glyphHeightPx, screenWidth, screenHeight);
			}

			if (glyph != nullptr) {
				penX += glyph->advance * scale + letterSpacingPx;
			}
		}

		lineTopPx += lineHeightPx;
		lineStart = i + 1;
	}
}

}

UITextSystem::UITextSystem()
{
}


void UITextSystem::Initialize()
{
}


void UITextSystem::Shutdown()
{
	ReleasePrivateFonts();
	g_FontCache.clear();
}


void UITextSystem::Update(vector<Entity*>& entities, float)
{
	Blend::Blending(Blend::blendmode::alpha, Blend::blendop::add);
	Rasterizer::Cull(Rasterizer::cullmode::off);
	Depth::Depth(Depth::depthmode::off);
	InputAssembler::IA(InputAssembler::topology::triList);

	const float screenWidth = ConstBuf::frame.aspect.z;
	const float screenHeight = ConstBuf::frame.aspect.w;

	size_t size = entities.size();
	for (int i = 0; i < size; i++) {
		Entity* entity = entities[i];
		if (!IsEntityValid(entity)) {
			continue;
		}

		Transform2D* transform2D = entity->GetComponent<Transform2D>();
		TextLabel* textLabel = entity->GetComponent<TextLabel>();
		if (transform2D == nullptr || textLabel == nullptr || !textLabel->active) {
			continue;
		}

		wstring text = ResolveText(*textLabel);
		if (text.empty()) {
			continue;
		}
		text = NormalizeToSupportedCharset(text);

		const FontAtlas* atlas = GetOrCreateFontAtlas(*textLabel);
		if (atlas == nullptr || atlas->textureId < 0) {
			continue;
		}

		const Transform2D worldTransform = GetWorldTransform2D(entity);
		RenderTextLabel(worldTransform, *textLabel, *atlas, text, screenWidth, screenHeight);
	}
}
