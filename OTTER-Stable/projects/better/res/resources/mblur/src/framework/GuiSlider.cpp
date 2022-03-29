/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "GuiSlider.h"
#include "common.h"
#include "AppBase.h"
#include "Font.h"
#include "Texture2d.h"
#include "Model.h"
#include "Shader.h"

namespace {

inline float getKnobPos(float value, float min, float max) {
	return (value - min) / (max - min);
}

} // namespace


namespace frm {

/*******************************************************************************
	GuiSlider implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
GuiSlider* GuiSlider::create(
	float *value,
	const ivec2 &minMax,
	Texture2d *backgroundTexture,
	Texture2d *knobTexture,
	Font *font,
	const CStringT &name
) {
	GuiSlider *result = new(std::nothrow) GuiSlider(
		value,
		minMax,
		backgroundTexture,
		knobTexture,
		font,
		name
	);
	assert(result);
	return result;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void GuiSlider::render(AppBase *pApp) {
	assert(shaderSprite_);
	assert(font_);
	assert(bgTex_);
	assert(knobTex_);
	assert(modelSAQuad);

	assert(pApp);
	int viewportX = pApp->getWindowSize().x();
	int viewportY = pApp->getWindowSize().y();

	OOGL_CALL(glEnable(GL_BLEND));

//	render text:
	OOGL_CALL(glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA));

	font_->enable(pApp);
	font_->renderString(absPosition_.x(), absPosition_.y(), name_.c_str(), *value_);
	font_->disable();

//	render slider:
	OOGL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	shaderSprite_->use();

	vec2 size((float)size_.x() / (float)viewportX, (float)size_.y() / (float)viewportY);
	float yOffset = ((float)font_->getCharHeight() + (float)font_->getLeading()) / (float)viewportY;
	vec2 position((float)absPosition_.x() / (float)viewportX, (float)absPosition_.y() / (float)viewportY + yOffset);
	OOGL_CALL(glUniform2fv(ulShaderSpriteSize, 1, size));
	OOGL_CALL(glUniform2fv(ulShaderSpritePosition, 1, position));
	OOGL_CALL(glUniform4fv(ulShaderSpriteColor, 1, color_));

	OOGL_CALL(glActiveTexture(GL_TEXTURE0));
	bgTex_->bind();

	modelSAQuad->enable(shaderSprite_, pApp);
	modelSAQuad->render(GL_TRIANGLE_STRIP);

//	render knob:
	// limit knob pos so the knob doesn't go outside the edges!
	float ks = (float)knobTex_->getWidth() * 0.5f / (float)absSize_.x();
	float kn = sml::clamp(getKnobPos(*value_, min_, max_), ks, 1.0f - ks);

	knobTex_->bind();

	position.x(position.x() + kn * size.x());
	size = vec2((float)knobTex_->getWidth() / (float)viewportX, (float)knobTex_->getHeight() / (float)viewportY);
	position.x(position.x() - size.x() * 0.5f);
	glUniform2fv(ulShaderSpriteSize, 1, size);
	glUniform2fv(ulShaderSpritePosition, 1, position);


	modelSAQuad->render(GL_TRIANGLE_STRIP);
	modelSAQuad->disable();

	Shader::useNone();

	OOGL_CALL(glDisable(GL_BLEND));
}

/*----------------------------------------------------------------------------*/
bool GuiSlider::mouseDown(const ivec2 &position, const MouseButtonT &button) {
	if (button != LBUTTON)
		return false;
	if (!(visible_ && enabled_))
		return false;
	if (!isInside(position))
		return false;

	focus();
	*value_ = (float)(position.x() - absPosition_.x()) / (float)absSize_.x();
	*value_ = *value_ * (max_ - min_) + min_;
	*value_ = sml::clamp(*value_, min_, max_);

	return true;
}

/*----------------------------------------------------------------------------*/
bool GuiSlider::mouseUp(const ivec2 &position, const MouseButtonT &button) {
	if (button != LBUTTON)
		return false;
	if (parent_->getFocus() == this) {
		unfocus();
		return true;
	}

	return false;
}

/*----------------------------------------------------------------------------*/
bool GuiSlider::mouseMove(const ivec2 &position) {
	if (!(visible_ && enabled_))
		return false;

	if (parent_->getFocus() == this) {
		*value_ = (float)(position.x() - absPosition_.x()) / (float)absSize_.x();
		*value_ = *value_ * (max_ - min_) + min_;
		*value_ = sml::clamp(*value_, min_, max_);
		return true;
	}

	return false; // not handled
}


//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
GuiSlider::GuiSlider(
	float *value,
	const ivec2 &minMax,
	Texture2d *backgroundTexture,
	Texture2d *knobTexture,
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
	),
	value_(value),
	min_(minMax.x()), max_(minMax.y()) {

	assert(value);

	assert(backgroundTexture);
	bgTex_ = backgroundTexture;
	size_ = ivec2(bgTex_->getWidth(), bgTex_->getHeight());

	assert(knobTexture);
	knobTex_ = knobTexture;

	assert(font);
	font_ = font;
}

} // namespace frm
