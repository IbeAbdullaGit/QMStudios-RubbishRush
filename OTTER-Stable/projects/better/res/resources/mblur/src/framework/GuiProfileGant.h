/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_GuiProfileGant_H_
#define FRAMEWORK_GuiProfileGant_H_

#include "common.h"
#include "IGuiComponent.h"
#include "Font.h"
#include "Model.h"
#include "Shader.h"
#include "Profiler.h"

namespace frm {

/*	*/
class GuiProfileGant: public IGuiComponent {
public:

//	CTORS/DTORS:
	static GuiProfileGant* create();

//	SERVICES:
	virtual void render(AppBase *pApp);

protected:
//	MEMBERS:
	Font *font_;

//	CTORS/DTORS:
	GuiProfileGant();

}; // class GuiProfileGant

/*******************************************************************************
	GuiProfileGant inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:
/*----------------------------------------------------------------------------*/
inline GuiProfileGant* GuiProfileGant::create() {
	GuiProfileGant *result = new(std::nothrow) GuiProfileGant();
	assert(result);
	return result;
}

//	SERVICES:

/*
	gant's size =
		(this.avgDuration / parent.avgDuration) * parent.size

	gant's position =
		parent.pos + (this.start - parent.start) / parent.avgDuration * parent.Size

*//*
inline void GuiProfileGant::renderGant(const Profiler &prf, const vec2 &parentSize, const vec2 &origin, const vec2 &viewportSize) {
	float frac = 1.0f;
	float offset = origin.x();
	if (prf.getParentId() != Profiler::INVALID_ID) {
		const Profiler &parent = Profiler::getProfiler(prf.getParentId());
		//frac = prf.getAverageDuration() / parent.getAverageDuration();
		//offset += (prf[1].start - parent[1].start) / parent.getAverageDuration() * parentSize.x();

		int smp = Profiler::getBufferSize() - 1;
		frac = prf[smp].duration / parent[smp].duration;
		offset += (prf[smp].start - parent[smp].start) / parent[smp].duration * parentSize.x();

	}

//	compute size as a fraction of the parent size:
	vec2 realSize(parentSize.x() * frac, parentSize.y());
	glUniform2fv(ulShaderSpriteSize, 1, realSize);

//	compute offset along parent length:
	vec2 realPosition(offset, origin.y() + parentSize.y() + (float)font_->getLeading() / viewportSize.y());
	glUniform2fv(ulShaderSpritePosition, 1, realPosition);

	modelSAQuad->render(GL_TRIANGLE_STRIP);

//	render all children:
	for (int i = 0; i < Profiler::getNProfilers(); ++i)
		if (Profiler::getProfiler(i).getParentId() == prf.getId())
			renderGant(Profiler::getProfiler(i), realSize, realPosition, viewportSize);
}*/

/*----------------------------------------------------------------------------*/
/*inline void GuiProfileGant::render(int viewportX, int viewportY) {
	assert(shaderSprite_);
	assert(modelSAQuad);
	assert(font_);

	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_ONE, GL_ONE);

	vec2 viewportSize((float)viewportX, (float)viewportY);
	vec2 realSize = vec2((float)absSize_.x(), (float)absSize_.y()) / viewportSize;
	vec2 realPosition = vec2((float)absPosition_.x(), (float)absPosition_.y()) / viewportSize;

//	render backgrounds:
	shaderSprite_->use();
	glUniform4fv(ulShaderSpriteColor, 1, vec4(0.1f, 0.1f, 0.1f, 0.5f));
	glActiveTexture(GL_TEXTURE0);
	texWhite->bind(); // KLUDGE
	modelSAQuad->enable(shaderSprite_);
	renderGant(Profiler::getProfiler(0), realSize, realPosition, viewportSize);
	modelSAQuad->disable();
	Shader::useNone();

//	render text:

	glDisable(GL_BLEND);
}*/

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline GuiProfileGant::GuiProfileGant()
: 	IGuiComponent(
		"",
		ivec2(0),
		LEFT, TOP,
		ABSOLUTE, ABSOLUTE,
		ivec2(512, 32),
		ABSOLUTE, ABSOLUTE,
		vec4(1.0f)
	) {

	font_ = Font::create(
		Texture2d::loadFromFile("media/gui/default16.png"),
		10, 13,
		-3, 3,
		Font::REGULAR,
		vec4(1.0f)
	);
}

} // namespace frm

#endif // FRAMEWORK_GuiProfileGant_H_

