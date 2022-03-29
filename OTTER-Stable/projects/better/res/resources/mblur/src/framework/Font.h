/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_FONT_H_
#define FRAMEWORK_FONT_H_

#include "common.h"
#include "IResource.h"
#include "Texture2d.h"

namespace frm {

/*	Manages resources for a texture-based font (texture, shader, geometry) +
	provides services for rendering text strings. */
class Font: public IResource<Font> {
public:
	enum StyleT {
		REGULAR,
		BOLD,
		ITALIC,
		BOLD_ITALIC
	};

//	CTORS/DTORS:
	static Font* create(
		Texture2d *fontTex,
		int charWidth = 0, int charHeight = 0,
		int tracking = 0, int leading = 0,
		const StyleT &style = REGULAR,
		const vec4 &color = vec4(1.0f)
	);

	static void destroy(Font **ppFont);

//	ACCESSORS:
	Texture2d* getTexture();
	void setTexture(Texture2d*);

	int getBaseCharWidth() const;
	void setBaseCharWidth(int);
	int getBaseCharHeight() const;
	void setBaseCharHeight(int);
	void setBaseCharSize(int width, int height);

	int getCharWidth() const;
	void setCharWidth(int);
	int getCharHeight() const;
	void setCharHeight(int);
	void setCharSize(int width, int height);

	int getTracking() const;
	void setTracking(int);

	int getLeading() const;
	void setLeading(int);

	const vec4& getColor() const;
	void setColor(const vec4&);
	void setColor(float r, float g, float b, float a = 1.0f);

	const StyleT& getStyle() const;
	void setStyle(const StyleT&);

//	SERVICES:
	/*	Enables shaders, bind textures, pass uniforms. */
	void enable(AppBase *pApp);
	/* Undo anything done in enable(). */
	void disable();

	/*	Render character at x,y with the currently enabled font. */
	static void renderChar(int x, int y, char c);
	/*	x/y are in [0, 1]. */
	static void renderChar(float x, float y, char c);

	/*	Render a string at x,y with the currently enabled font. Use printf-style
		formatting. '\n' causes a newline (y + height + leading, reset x), '\t'
		causes a tab (x + 4 * (width + tracking)). */
	static void renderString(int x, int y, CStringT str, ...);
	/*	x/y are in [0, 1]. */
	static void renderString(float x, float y, CStringT str, ...);

private:
//	MEMBERS:
	Texture2d *fontTex_; // font texture
	ivec2 baseCharSize_; // internal char size

	ivec2 charSize_; // render char size (pixels)
	ivec2 spacing_; // x = tracking (char spacing), y = leading (line spacing)
	vec4 color_; // current render color
	StyleT style_;
	vec4 styleMask_; // R/G/B/A = regular/bold/italic/bold-italic

	ivec2 viewportSize_; // current viewport size (set by enable())

	static const Font *current_; // currently enabled font
	static Shader *fontProgram_;
	static GLint ulViewportSize, ulBaseCharSize, ulCharSize, ulColor,
		ulStyleMask, ulCharPosition, ulChar;

//	CTORS/DTORS:
	Font();

}; // class Font

/*******************************************************************************
	Font inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline Texture2d* Font::getTexture() {
	return fontTex_;
}
/*----------------------------------------------------------------------------*/
inline void Font::setTexture(Texture2d *texture) {
	assert(texture);
	fontTex_ = texture;
	setBaseCharSize(fontTex_->getWidth() / 16, fontTex_->getHeight() / 16);
}

/*----------------------------------------------------------------------------*/
inline int Font::getBaseCharWidth() const {
	return baseCharSize_.x();
}
/*----------------------------------------------------------------------------*/
inline void Font::setBaseCharWidth(int x) {
	baseCharSize_.x(x);
}
/*----------------------------------------------------------------------------*/
inline int Font::getBaseCharHeight() const {
	return baseCharSize_.y();
}
/*----------------------------------------------------------------------------*/
inline void Font::setBaseCharHeight(int y) {
	baseCharSize_.y(y);
}
/*----------------------------------------------------------------------------*/
inline void Font::setBaseCharSize(int x, int y) {
	baseCharSize_ = sml::vec2i(x, y);
}

/*----------------------------------------------------------------------------*/
inline int Font::getCharWidth() const {
	return charSize_.x();
}
/*----------------------------------------------------------------------------*/
inline void Font::setCharWidth(int x) {
	charSize_.x(x);
}
/*----------------------------------------------------------------------------*/
inline int Font::getCharHeight() const {
	return charSize_.y();
}
/*----------------------------------------------------------------------------*/
inline void Font::setCharHeight(int y) {
	charSize_.y(y);
}
/*----------------------------------------------------------------------------*/
inline void Font::setCharSize(int x, int y) {
	charSize_ = sml::vec2i(x, y);
}

/*----------------------------------------------------------------------------*/
inline int Font::getTracking() const {
	return spacing_.x();
}
/*----------------------------------------------------------------------------*/
inline void Font::setTracking(int x) {
	spacing_.x(x);
}

/*----------------------------------------------------------------------------*/
inline int Font::getLeading() const {
	return spacing_.y();
}
/*----------------------------------------------------------------------------*/
inline void Font::setLeading(int y) {
	spacing_.y(y);
}

/*----------------------------------------------------------------------------*/
inline const vec4& Font::getColor() const {
	return color_;
}
/*----------------------------------------------------------------------------*/
inline void Font::setColor(const vec4 &color) {
	color_ = color;
}
/*----------------------------------------------------------------------------*/
inline void Font::setColor(float r, float g, float b, float a) {
	color_ = vec4(r, g, b, a);
}

/*----------------------------------------------------------------------------*/
inline const Font::StyleT& Font::getStyle() const {
	return style_;
}
/*----------------------------------------------------------------------------*/
inline void Font::setStyle(const StyleT &style) {
	style_ = style;
	switch (style_) {
		case BOLD_ITALIC:
			styleMask_ = vec4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		case ITALIC:
			styleMask_ = vec4(0.0f, 0.0f, 1.0f, 0.0f);
			break;
		case BOLD:
			styleMask_ = vec4(0.0f, 1.0f, 0.0f, 0.0f);
			break;
		case REGULAR:
		default:
			styleMask_ = vec4(1.0f, 0.0f, 0.0f, 0.0f);
			break;
	};
}

} // namespace frm

#endif // FRAMEWORK_FONT_H_
