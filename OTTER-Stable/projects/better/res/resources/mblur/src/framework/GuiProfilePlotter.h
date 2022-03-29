/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_GuiProfilePlotter_H_
#define FRAMEWORK_GuiProfilePlotter_H_

#include "common.h"
#include "IGuiComponent.h"
#include "Font.h"
#include "Profiler.h"

namespace frm {

/*	*/
class GuiProfilePlotter: public IGuiComponent {
public:

//	CTORS/DTORS:
	static GuiProfilePlotter* create(
		const ivec2 &size = ivec2(200, 70), // size per plot
		AlignT alignX = LEFT, AlignT alignY = BOTTOM
	);

//	SERVICES:
	virtual void render(AppBase *pApp);

protected:
//	MEMBERS:
	Font *font_;
	Font *dataFont_; // for min/max/avg, etc. Copied from font.

//	CTORS/DTORS:
	GuiProfilePlotter(const ivec2 &size, AlignT alignX, AlignT alignY);

}; // class GuiProfilePlotter

/*******************************************************************************
	GuiProfilePlotter inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:
/*----------------------------------------------------------------------------*/
inline GuiProfilePlotter* GuiProfilePlotter::create(
	const ivec2 &size, AlignT alignX, AlignT alignY
) {
	GuiProfilePlotter *result = new(std::nothrow) GuiProfilePlotter(
		size, alignX, alignY
	);
	assert(result);
	return result;
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline GuiProfilePlotter::GuiProfilePlotter(
	const ivec2 &size, AlignT alignX, AlignT alignY
): IGuiComponent(
		"",
		ivec2(0),
		alignX, alignY,
		ABSOLUTE, ABSOLUTE,
		size,
		ABSOLUTE, ABSOLUTE,
		vec4(1.0f)
	) {

	font_ = Font::create(
		Texture2d::loadFromFile("media/gui/default16.png"),
		10, 13,
		-3, 3,
		Font::REGULAR,
		vec4(0.9f)
	);

	dataFont_ = Font::create(
		Texture2d::loadFromFile("media/gui/default8.png"),
		8, 10,
		-3, 0,
		Font::REGULAR,
		vec4(0.2f)
	);
}

} // namespace frm

#endif // FRAMEWORK_GuiProfilePlotter_H_
