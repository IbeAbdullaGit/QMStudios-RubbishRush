/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_GuiSlider_H_
#define FRAMEWORK_GuiSlider_H_

#include "common.h"
#include "IGuiComponent.h"
#include "Font.h"
#include "Texture2d.h"

namespace frm {

/*	*/
class GuiSlider: public IGuiComponent {
public:

//	CTORS/DTORS:
	static GuiSlider* create(
		float *value,
		const ivec2 &minMax,
		Texture2d *backgroundTexture,
		Texture2d *knobTexture,
		Font *font,
		const CStringT &name = ""
	);

//	ACCESSORS:
	Font* getFont() const;
	void setFont(Font*);

	Texture2d* getKnobTexture() const;
	void setKnobTexture(Texture2d*);

	Texture2d* getBackgroundTexture() const;
	void setBackgroundTexture(Texture2d*);

	float getMin() const;
	void setMin(float);
	float getMax() const;
	void setMax(float);
	void setMinMax(float min, float max);

//	SERVICES:
	virtual void render(AppBase *pApp);

	virtual bool mouseDown(const ivec2 &position, const MouseButtonT &button);
	virtual bool mouseUp(const ivec2 &position, const MouseButtonT &button);
	virtual bool mouseMove(const ivec2 &position);

protected:
//	MEMBERS:
	float *value_;

	Font *font_;

	Texture2d *knobTex_, *bgTex_;
	float min_, max_;

//	CTORS/DTORS:
	GuiSlider(
		float *value,
		const ivec2 &minMax,
		Texture2d *backgroundTexture,
		Texture2d *knobTexture,
		Font *font,
		const CStringT &name
	);

//	SERVICES:
	/*	Compute & cache the absolute size/position in render coordinates. The
		height of the slider is absolute and equals the height of the font +
		spacing + height of the knob texture. */
	virtual void computeAbsSizePosition();

}; // class GuiSlider

/*******************************************************************************
	GuiSlider inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline Font* GuiSlider::getFont() const {
	return font_;
}
/*----------------------------------------------------------------------------*/
inline void GuiSlider::setFont(Font *font) {
	font_ = font;
	computeAbsSizePosition();
}

/*----------------------------------------------------------------------------*/
inline Texture2d* GuiSlider::getKnobTexture() const {
	return knobTex_;
}
/*----------------------------------------------------------------------------*/
inline void GuiSlider::setKnobTexture(Texture2d *tex) {
	knobTex_ = tex;
	computeAbsSizePosition();
}

/*----------------------------------------------------------------------------*/
inline Texture2d* GuiSlider::getBackgroundTexture() const {
	return bgTex_;
}
/*----------------------------------------------------------------------------*/
inline void GuiSlider::setBackgroundTexture(Texture2d *tex) {
	bgTex_ = tex;
	computeAbsSizePosition();
}

/*----------------------------------------------------------------------------*/
inline float GuiSlider::getMin() const {
	return min_;
}
/*----------------------------------------------------------------------------*/
inline void GuiSlider::setMin(float min) {
	min_ = min;
}
/*----------------------------------------------------------------------------*/
inline float GuiSlider::getMax() const {
	return max_;
}
/*----------------------------------------------------------------------------*/
inline void GuiSlider::setMax(float max) {
	max_ = max;
}
/*----------------------------------------------------------------------------*/
inline void GuiSlider::setMinMax(float min, float max) {
	min_ = min;
	max_ = max;
}


//	PROTECTED:

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline void GuiSlider::computeAbsSizePosition() {
	assert(font_);
	assert(knobTex_);
	IGuiComponent::computeAbsSizePosition(); // call base
	absSize_.y(font_->getCharHeight() + font_->getLeading() + size_.y());
}

} // namespace frm

#endif // FRAMEWORK_GuiSlider_H_
