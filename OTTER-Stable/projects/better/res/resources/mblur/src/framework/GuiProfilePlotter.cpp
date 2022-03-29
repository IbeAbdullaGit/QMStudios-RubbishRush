/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <vector>

#include "GuiProfilePlotter.h"
#include "common.h"
#include "AppBase.h"

namespace frm {

/*******************************************************************************
	IGuiComponent implementation:
*******************************************************************************/

//	PUBLIC:

//	SERVICES:

/*----------------------------------------------------------------------------*/
void GuiProfilePlotter::render(AppBase *pApp) {
	assert(shaderSprite_);
	assert(modelSAQuad);
	assert(font_);
	assert(dataFont_);

	assert(pApp);
	int viewportX = pApp->getWindowSize().x();
	int viewportY = pApp->getWindowSize().y();

	OOGL_CALL(glEnable(GL_BLEND));
	OOGL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	vec2 viewportSize((float)viewportX, (float)viewportY);
	vec2 realSize = vec2((float)size_.x(), (float)size_.y()) / viewportSize;
	vec2 realPosition = vec2((float)absPosition_.x(), (float)absPosition_.y()) / viewportSize;

	OOGL_CALL(glMatrixMode(GL_PROJECTION));
	OOGL_CALL(glLoadIdentity());
	OOGL_CALL(glMatrixMode(GL_MODELVIEW));
	OOGL_CALL(glLoadIdentity());
	OOGL_CALL(glPushMatrix());
	OOGL_CALL(glTranslatef(-1.0f, 1.0f, 0.0f));
	OOGL_CALL(glScalef(2.0f, -2.0f, 1.0f));

//	render backgrounds:
	OOGL_CALL(glColor4f(0.2f, 0.2f, 0.2f, 0.8f));

	for (int i = 0; i < Profiler::getNProfilers(); ++i) {
		vec2 offset((float)(i * (size_.x() + font_->getCharWidth())), 0.0f);
		offset /= viewportSize;

		vec2 topLeft = realPosition + offset;
		vec2 bottomRight = topLeft + realSize;
		OOGL_CALL(glRectfv(topLeft, bottomRight));
	}

//	plot graph:
	OOGL_CALL(glColor4fv(color_));
	for (int i = 0; i < Profiler::getNProfilers(); ++i) {
		const Profiler& prf = Profiler::getProfiler(i);
		vec2 offset1((float)(i * (size_.x() + font_->getCharWidth())), (float)size_.y());

		OOGL_CALL(
			glBegin(GL_LINE_STRIP);
			// start at 1; don't render the current sample as it is invalid!
				for (int j = 1; j < Profiler::getBufferSize(); ++j) {
					float val = prf[j].duration / prf.getMaxDuration();
					val *= -(float)size_.y(); // now value is y offset in pixels
					vec2 offset2((float)j / (float)Profiler::getBufferSize() * (float)size_.x(), val);
					offset2 = (offset1 + offset2) / viewportSize;
					glVertex2fv(realPosition + offset2);
				}
			glEnd()
		);
	}
	OOGL_CALL(glPopMatrix());

//	render profiler text:
	OOGL_CALL(glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA));

	font_->enable(pApp);
	for (int i = 0; i < Profiler::getNProfilers(); ++i) {
		const Profiler& prf = Profiler::getProfiler(i);
		ivec2 offset(i * (size_.x() + font_->getCharWidth()), 0);
		offset += absPosition_;

		if (prf.getParentId() != Profiler::INVALID_ID) {
			const Profiler& parent = Profiler::getProfiler(prf.getParentId());
			float percent = prf.getAverageDuration() / parent.getAverageDuration() * 100.0f;
			font_->renderString(
				offset.x(), offset.y(),
				"%s\n(%3.1f%% of %s)\ncalls: %u\navg: %2.4fms",
				prf.getName(),
				percent,
				parent.getName(),
				prf[0].callCount,
				prf.getAverageDuration() * 1000.0f
			);
		} else {
			font_->renderString(
				offset.x(), offset.y(),
				"%s\n--\ncalls: %u\navg: %2.4fms",
				prf.getName(),
				prf[0].callCount,
				prf.getAverageDuration() * 1000.0f
			);
		}
	}
	font_->disable();

//	render data text:
	dataFont_->enable(pApp);
	for (int i = 0; i < Profiler::getNProfilers(); ++i) {
		const Profiler& prf = Profiler::getProfiler(i);
		int charWidth = dataFont_->getCharWidth() + dataFont_->getTracking();
		ivec2 offset(i * (size_.x() + dataFont_->getCharWidth()), 0);
		offset.x(offset.x() + size_.x() - charWidth * 6);
		offset += absPosition_;
		dataFont_->renderString(
			offset.x(), offset.y(),
			"%2.3f", prf.getMaxDuration() * 1000.0f
		);

		offset.y(offset.y() + size_.y() - dataFont_->getCharHeight());
		dataFont_->renderString(
			offset.x(), offset.y(),
			"%2.3f", prf.getMinDuration() * 1000.0f
		);
	}
	dataFont_->disable();

	OOGL_CALL(glDisable(GL_BLEND));
}

} // namespace frm


