/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <cstring> // strlen

#include "GuiCheckbox.h"
#include "common.h"
#include "AppBase.h"
#include "Font.h"
#include "Texture2d.h"
#include "Model.h"
#include "Shader.h"

namespace frm {

/*******************************************************************************
	GuiCheckbox implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
GuiCheckbox* GuiCheckbox::create(
	bool *value,
	Texture2d *offTexture,
	Texture2d *onTexture,
	Font *font,
	const CStringT &name
) {
	GuiCheckbox *result = new(std::nothrow) GuiCheckbox(
		value,
		offTexture,
		onTexture,
		font,
		name
	);
	assert(result);
	return result;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void GuiCheckbox::render(AppBase *pApp) {
	assert(shaderSprite_);
	assert(font_);
	assert(offTex_);
	assert(onTex_);
	assert(modelSAQuad);

	assert(pApp);
	int viewportX = pApp->getWindowSize().x();
	int viewportY = pApp->getWindowSize().y();

	OOGL_CALL(glEnable(GL_BLEND));

//	render text:
	int yOffset = (offTex_->getHeight() - font_->getCharHeight()) / 2;

	OOGL_CALL(glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA));

	font_->enable(pApp);
	font_->renderString(absPosition_.x() + offTex_->getWidth(), absPosition_.y()+ yOffset, name_.c_str());
	font_->disable();

//	render button:
	OOGL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	shaderSprite_->use();

	vec2 size((float)offTex_->getWidth() / (float)viewportX, (float)offTex_->getHeight() / (float)viewportY);
	vec2 position((float)absPosition_.x() / (float)viewportX, (float)absPosition_.y() / (float)viewportY);
	OOGL_CALL(glUniform2fv(ulShaderSpriteSize, 1, size));
	OOGL_CALL(glUniform2fv(ulShaderSpritePosition, 1, position));
	OOGL_CALL(glUniform4fv(ulShaderSpriteColor, 1, color_));

	OOGL_CALL(glActiveTexture(GL_TEXTURE0));
	if (*value_)
		onTex_->bind();
	else
		offTex_->bind();

	modelSAQuad->enable(shaderSprite_, pApp);
	modelSAQuad->render(GL_TRIANGLE_STRIP);
	modelSAQuad->disable();

	Shader::useNone();

	OOGL_CALL(glDisable(GL_BLEND));
}

/*----------------------------------------------------------------------------*/
bool GuiCheckbox::mouseDown(const ivec2 &position, const MouseButtonT &button) {
	if (button != LBUTTON)
		return false;
	if (!(visible_ && enabled_))
		return false;
	if (!isInside(position))
		return false;

	focus();

	return true;
}

/*----------------------------------------------------------------------------*/
bool GuiCheckbox::mouseUp(const ivec2 &position, const MouseButtonT &button) {
	if (button != LBUTTON)
		return false;
	if (!(visible_ && enabled_))
		return false;
	if (!isInside(position))
		return false;
	if (parent_->getFocus() == this) {
		*value_ = !(*value_);
		unfocus();
		return true;
	}

	return false;
}

/*----------------------------------------------------------------------------*/
bool GuiCheckbox::mouseMove(const ivec2 &position) {
	return false; // not handled
}


//	PROTECTED:

//	MEMBERS:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
GuiCheckbox::GuiCheckbox(
	bool *value,
	Texture2d *offTexture,
	Texture2d *onTexture,
	Font *font,
	const CStringT &name
):	IGuiComponent(
		name,
		ivec2(0),
		LEFT, TOP,
		ABSOLUTE, ABSOLUTE,
		ivec2(0), // get width from texture
		ABSOLUTE, ABSOLUTE,
		vec4(1.0f)
	) {

	assert(value);
	value_ = value;

	assert(offTexture);
	offTex_ = offTexture;
	size_ = ivec2(offTex_->getWidth(), offTex_->getHeight());

	assert(onTexture);
	onTex_ = onTexture;

	assert(font);
	font_ = font;

	size_.x(size_.x() + (font_->getCharWidth() + font_->getTracking()) * strlen(name));
}

} // namespace frm

