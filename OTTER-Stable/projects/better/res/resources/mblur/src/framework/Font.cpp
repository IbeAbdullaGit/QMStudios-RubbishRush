/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <cstdarg> // va_start, va_end
#include <cstdio> // vsnprintf

#include <vector>

#include "Font.h"
#include "common.h"
#include "AppBase.h"
#include "Model.h"
#include "Shader.h"
#include "ShaderUnit.h"

namespace frm {

/*******************************************************************************
	Font implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
Font* Font::create(
	Texture2d *fontTex,
	int charWidth, int charHeight,
	int tracking, int leading,
	const StyleT &style,
	const vec4 &color
) {
	assert(fontTex);
	if (charWidth == 0)
		charWidth = fontTex->getWidth() / 16;
	if (charHeight == 0)
		charHeight = fontTex->getHeight() / 16;

	Font *result = new(std::nothrow) Font;
	assert(result);

	result->fontTex_ = fontTex;
	result->setCharSize(charWidth, charHeight);
	result->setBaseCharSize(fontTex->getHeight() / 16, fontTex->getWidth() / 16);
	result->setTracking(tracking);
	result->setLeading(leading);
	result->setStyle(style);
	result->setColor(color);

	return result;
}

/*----------------------------------------------------------------------------*/
void Font::destroy(Font **ppFont) {
	assert(ppFont != 0);
	assert(*ppFont != 0);
	delete *ppFont;
	*ppFont = 0;
}

//	ACCESSORS:

//	SERVICES:

/*----------------------------------------------------------------------------*/
void Font::enable(AppBase *pApp) {
	assert(fontProgram_);
	assert(fontTex_);
	assert(modelSAQuad);

	assert(pApp);
	int viewportX = pApp->getWindowSize().x();
	int viewportY = pApp->getWindowSize().y();

	viewportSize_ = ivec2(viewportX, viewportY);
	vec2 fCharSize = vec2((float)charSize_.x() / (float)viewportX, (float)charSize_.y() / (float)viewportY);

	fontProgram_->use();

	OOGL_CALL(glUniform2iv(ulBaseCharSize, 1, baseCharSize_));
	OOGL_CALL(glUniform2fv(ulCharSize, 1, fCharSize));
	OOGL_CALL(glUniform4fv(ulColor, 1, color_));
	OOGL_CALL(glUniform4fv(ulStyleMask, 1, styleMask_));

	OOGL_CALL(glActiveTexture(GL_TEXTURE0));
	fontTex_->bind();

	modelSAQuad->enable(fontProgram_, pApp);

	current_ = this;
}

/*----------------------------------------------------------------------------*/
void Font::disable() {
	modelSAQuad->disable();
	Shader::useNone();
	current_ = 0;
}

/*----------------------------------------------------------------------------*/
void Font::renderChar(int x, int y, char c) {
	assert(current_);
	float vx = (float)current_->viewportSize_.x();
	float vy = (float)current_->viewportSize_.y();
	renderChar((float)x / vx, (float)y / vy, c);
}
/*----------------------------------------------------------------------------*/
void Font::renderChar(float x, float y, char c) {
	assert(fontProgram_);
	OOGL_CALL(glUniform2f(ulCharPosition, x, y));
	OOGL_CALL(glUniform1i(ulChar, (GLint)c));
	modelSAQuad->render(GL_TRIANGLE_STRIP);
}

/*----------------------------------------------------------------------------*/
void Font::renderString(int x, int y, CStringT str, ...) {
	assert(current_ != 0);

	va_list args;
	va_start(args, str); // set start of args list
	char buff[1024];
	vsprintf(buff, str, args);
	va_end(args); // free the va_list

	int i = 0, thisX = x, thisY = y;
	while (buff[i] != 0) {
		switch (buff[i]) {
			case '\n':
				thisY += current_->getCharHeight() + current_->getLeading();
				thisX = x;
				break;
			/*case '\t':
				break;*/
			default:
				renderChar(thisX, thisY, buff[i]);
				thisX += current_->getCharWidth() + current_->getTracking();
				break;
		}
		++i;
	}
}
/*----------------------------------------------------------------------------*/
void Font::renderString(float x, float y, CStringT str, ...) {
	assert(current_ != 0);

	va_list args;
	va_start(args, str); // set start of args list
	char buff[1024];
	vsprintf(buff, str, args);
	va_end(args); // free the va_list

	renderString((int)(x * current_->viewportSize_.x()), (int)(y * current_->viewportSize_.y()), buff);
}


//	PRIVATE:

//	MEMBERS:

const Font *Font::current_ = 0;
Shader *Font::fontProgram_ = 0;
GLint Font::ulViewportSize = 0,
		Font::ulBaseCharSize = 0,
		Font::ulCharSize = 0,
		Font::ulColor = 0,
		Font::ulStyleMask = 0,
		Font::ulCharPosition = 0,
		Font::ulChar = 0;

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
Font::Font() {
	if (nInstances() == 1) {
	//	init static data:
		const char *vsSrc =
			"#version 420\n \
			uniform ivec2 uBaseCharSize; \
			uniform vec2 uCharSize; \
			uniform vec2 uCharPosition; \
			uniform int uChar; \
			layout(binding=0) uniform sampler2D uFontTex; \
			layout(location=0) in vec2 aPosition; \
			noperspective out vec2 vTexcoord; \
			void main() { \
				ivec2 coffset; \
				coffset.x = uChar % 16; \
				coffset.y = (uChar - coffset.x) / 16; \
				coffset *= uBaseCharSize; \
				vec2 texelSize = 1.0 / vec2(textureSize(uFontTex, 0)); \
				vec2 scale = vec2(uBaseCharSize) * texelSize; \
				vec2 bias = vec2(coffset) * texelSize; \
				vTexcoord = (aPosition * 0.5 + 0.5) * scale + bias; \
				bias = uCharPosition * 2.0 - 1.0 + uCharSize; \
				bias.y = -bias.y; \
				gl_Position = vec4((aPosition * uCharSize) + bias, 0.0, 1.0); \
			}\n";
		const char *fsSrc =
			"#version 420\n \
			uniform vec4 uColor = vec4(1.0); \
			uniform vec4 uStyleMask = vec4(0.0, 1.0, 0.0, 0.0); \
			layout(binding=0) uniform sampler2D uFontTex; \
			noperspective in vec2 vTexcoord; \
			layout(location=0) out vec4 fResult; \
			void main() { \
				vec4 val = texture(uFontTex, vTexcoord) * uStyleMask; \
				fResult = vec4(max(max(max(val.r, val.g), val.b), val.a)) * uColor; \
			}\n";
		fontProgram_ = Shader::create(
			2,
			ShaderUnit::create(GL_VERTEX_SHADER, vsSrc),
			ShaderUnit::create(GL_FRAGMENT_SHADER, fsSrc)
		);
		assert(fontProgram_);

		ulBaseCharSize = fontProgram_->getUniformLocation("uBaseCharSize");
		ulCharSize = fontProgram_->getUniformLocation("uCharSize");
		ulColor = fontProgram_->getUniformLocation("uColor");
		ulStyleMask = fontProgram_->getUniformLocation("uStyleMask");
		ulCharPosition = fontProgram_->getUniformLocation("uCharPosition");
		ulChar = fontProgram_->getUniformLocation("uChar");
	}
}

} // namespace frm
