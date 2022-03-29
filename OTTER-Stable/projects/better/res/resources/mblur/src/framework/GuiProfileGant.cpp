/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <vector>

#include "GuiProfileGant.h"
#include "common.h"
#include "AppBase.h"
#include "Profiler.h"
#include "Font.h"

namespace {

/*----------------------------------------------------------------------------*/
void renderGantRectangles(
	frm::Profiler::IdT i,
	const frm::vec2 &size,
	const frm::vec2 &position
) {
	using frm::vec2;
	using frm::Profiler;

	float width = 1.0f; // fraction of size
	float offset = 0.0f; // offset from position

	const Profiler &prf = Profiler::getProfiler(i);
	if (prf.getParentId() != Profiler::INVALID_ID) {
		const Profiler &parent = Profiler::getProfiler(prf.getParentId());
		int s = Profiler::getBufferSize() - 1;
		width = prf[s].duration / parent[s].duration;
		offset = (prf[s].start - parent[s].start) / parent[s].duration * (float)size.x();
	}

	vec2 topLeft = position + vec2(offset, 0.0f);
	vec2 bottomRight = topLeft + size * vec2(width, 1.0f);

	OOGL_CALL(glRectfv(topLeft, bottomRight));

//	render all children:
	for (int j = 0; j < Profiler::getNProfilers(); ++j)
		if (Profiler::getProfiler(j).getParentId() == prf.getId())
			renderGantRectangles(j, bottomRight - topLeft, vec2(topLeft.x(), topLeft.y() + size.y()));
}

/*----------------------------------------------------------------------------*/
void renderGantText(
	frm::Profiler::IdT i,
	const frm::ivec2 &size,
	const frm::ivec2 &position
) {
	using frm::ivec2;
	using frm::Profiler;
	using frm::Font;

	int width = 1;
	int offset = 0;

	const Profiler &prf = Profiler::getProfiler(i);
	if (prf.getParentId() != Profiler::INVALID_ID) {
		const Profiler &parent = Profiler::getProfiler(prf.getParentId());
		int s = Profiler::getBufferSize() - 1;
		width = (int)(prf[s].duration / parent[s].duration * size.x());
		offset = (int)((prf[s].start - parent[s].start) / parent[s].duration * (float)size.x());
	}

	Font::renderString(position.x() + offset, position.y(), "%s", prf.getName());

//	render all children:
	for (int j = 0; j < Profiler::getNProfilers(); ++j)
		if (Profiler::getProfiler(j).getParentId() == prf.getId())
			renderGantText(
				j,
				ivec2(size.x() * width, size.y()),
				ivec2(position.x() + offset, position.y() + size.y())
			);
}

}

namespace frm {

/*******************************************************************************
	IGuiComponent implementation:
*******************************************************************************/

//	PUBLIC:

//	SERVICES:

/*----------------------------------------------------------------------------*/
void GuiProfileGant::render(AppBase *pApp) {
	assert(font_);

	assert(pApp);
	int viewportX = pApp->getWindowSize().x();
	int viewportY = pApp->getWindowSize().y();

	vec2 viewportSize((float)viewportX, (float)viewportY);

	OOGL_CALL(glMatrixMode(GL_PROJECTION));
	OOGL_CALL(glLoadIdentity());
	OOGL_CALL(glMatrixMode(GL_MODELVIEW));
	OOGL_CALL(glLoadIdentity());
	OOGL_CALL(glPushMatrix());
	OOGL_CALL(glTranslatef(-1.0f, 1.0f, 0.0f));
	OOGL_CALL(glScalef(2.0f, -2.0f, 1.0f));

	OOGL_CALL(glColor4f(0.0f, 0.0f, 0.0f, 0.5f));

	OOGL_CALL(glEnable(GL_BLEND));

	OOGL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	renderGantRectangles(
		0,
		vec2((float)absSize_.x(), (float)absSize_.y()) / viewportSize,
		vec2((float)absPosition_.x(), (float)absPosition_.y()) / viewportSize
	);

	OOGL_CALL(glPopMatrix());

	OOGL_CALL(glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA));

	font_->enable(pApp);
	renderGantText(
		0,
		absSize_,
		absPosition_
	);
	font_->disable();

	OOGL_CALL(glDisable(GL_BLEND));
}

} // namespace frm

