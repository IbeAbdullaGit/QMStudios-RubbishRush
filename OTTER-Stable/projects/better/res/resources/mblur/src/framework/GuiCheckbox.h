/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_GUI_CHECKBOX_H_
#define FRAMEWORK_GUI_CHECKBOX_H_

#include <vector>

#include "common.h"
#include "IGuiComponent.h"
#include "Font.h"
#include "Texture2d.h"

namespace frm {

/*	*/
class GuiCheckbox: public IGuiComponent {
public:

//	CTORS/DTORS:
	static GuiCheckbox* create(
		bool *value,
		Texture2d *offTexture,
		Texture2d *onTexture,
		Font *font,
		const CStringT &name = 0
	);

//	ACCESSORS:
	Font* getFont() const;
	void setFont(Font*);

	Texture2d* getOnTexture() const;
	void setOnTexture(Texture2d*);

	Texture2d* getOffTexture() const;
	void setOffTexture(Texture2d*);

//	SERVICES:
	virtual void render(AppBase *pApp);

	virtual bool mouseDown(const ivec2 &position, const MouseButtonT &button);
	virtual bool mouseUp(const ivec2 &position, const MouseButtonT &button);
	virtual bool mouseMove(const ivec2 &position);

protected:
//	MEMBERS:
	bool *value_;
	Font *font_;
	Texture2d *onTex_, *offTex_;

//	CTORS/DTORS:
	GuiCheckbox(
		bool *value,
		Texture2d *offTexture,
		Texture2d *onTexture,
		Font *font,
		const CStringT &name
	);

//	SERVICES:
	/*	Compute & cache the absolute size/position in render coordinates. The
		height of the slider is absolute and equals the height of the font +
		spacing + height of the knob texture. */
	virtual void computeAbsSizePosition();

}; // class GuiCheckbox

/*******************************************************************************
	GuiCheckbox inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline Font* GuiCheckbox::getFont() const {
	return font_;
}
/*----------------------------------------------------------------------------*/
inline void GuiCheckbox::setFont(Font *font) {
	font_ = font;
	computeAbsSizePosition();
}

/*----------------------------------------------------------------------------*/
inline Texture2d* GuiCheckbox::getOnTexture() const {
	return onTex_;
}
/*----------------------------------------------------------------------------*/
inline void GuiCheckbox::setOnTexture(Texture2d *tex) {
	onTex_ = tex;
	computeAbsSizePosition();
}

/*----------------------------------------------------------------------------*/
inline Texture2d* GuiCheckbox::getOffTexture() const {
	return offTex_;
}
/*----------------------------------------------------------------------------*/
inline void GuiCheckbox::setOffTexture(Texture2d *tex) {
	offTex_ = tex;
	computeAbsSizePosition();
}

//	PROTECTED:

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline void GuiCheckbox::computeAbsSizePosition() {
	assert(font_);
	assert(onTex_);
	IGuiComponent::computeAbsSizePosition(); // call base
	absSize_.y(onTex_->getHeight() + font_->getLeading());
}

} // namespace frm

#endif // FRAMEWORK_GUI_CHECKBOX_H_
