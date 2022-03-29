/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <vector>

#include "IGuiComponent.h"
#include "common.h"
#include "Shader.h"
#include "ShaderUnit.h"

namespace frm {

/*******************************************************************************
	IGuiComponent implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
IGuiComponent* IGuiComponent::create(
	const CStringT &name,
	const ivec2 &position,
	AlignT alignX, AlignT alignY,
	ReferenceT posRefX, ReferenceT posRefY,
	const ivec2 &size,
	ReferenceT sizeRefX, ReferenceT sizeRefY,
	const vec4 &color
) {
	if (nInstances() == 0) {
	//	init static data:
		const char *vsSrc =
			"#version 420 \n \
			uniform vec2 uSize; \
			uniform vec2 uPosition; \
			layout(location=0) in vec2 aPosition; \
			noperspective out vec2 vTexcoord; \
			void main() { \
				vTexcoord = (aPosition * 0.5 + 0.5); \
				vec2 scale = uSize; \
				vec2 bias = uPosition * 2.0 - 1.0; \
				bias += scale; \
				bias.y = -bias.y; \
				gl_Position = vec4(aPosition * scale + bias, 0.0, 1.0); \
			}\n";
		const char *fsSrc =
			"#version 420 \n \
			uniform vec2 uSize; \
			uniform vec4 uColor = vec4(1.0); \
			layout(binding=0) uniform sampler2D uBackgroundTex; \
			noperspective in vec2 vTexcoord; \
			layout(location=0) out vec4 fResult; \
			void main() { \
				vec4 background = texture(uBackgroundTex, vTexcoord); \
				fResult = background * uColor; \
			}\n";
		shaderSprite_ = Shader::create(
			2,
			ShaderUnit::create(GL_VERTEX_SHADER, vsSrc),
			ShaderUnit::create(GL_FRAGMENT_SHADER, fsSrc)
		);
		assert(shaderSprite_);

		ulShaderSpriteSize = shaderSprite_->getUniformLocation("uSize");
		ulShaderSpritePosition = shaderSprite_->getUniformLocation("uPosition");
		ulShaderSpriteColor = shaderSprite_->getUniformLocation("uColor");
	}

	IGuiComponent *result = new(std::nothrow) IGuiComponent(
		name,
		position,
		alignX, alignY,
		posRefX, posRefY,
		size,
		sizeRefX, sizeRefY,
		color
	);
	assert(result);
	return result;
}

/*----------------------------------------------------------------------------*/
void IGuiComponent::destroy(IGuiComponent **ppComponent) {
	assert(ppComponent != 0);
	assert(*ppComponent != 0);
	delete *ppComponent;
	*ppComponent = 0;

	if (nInstances() == 0) {
	//	don't use nInstances() to destroy component-specific static data, as
	//	nInstance counts the total number of IGuiComponent instances!
	}
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void IGuiComponent::addChild(IGuiComponent *child) {
	assert(child != 0);
	if (child->parent_)
		child->parent_->removeChild(child);
	child->parent_ = this;
	children_.push_back(child);

	computeAbsSizePosition();
}
/*----------------------------------------------------------------------------*/
void IGuiComponent::removeChild(IGuiComponent *child) {
	assert(child);
	for (size_t i = 0; i < children_.size(); ++i)
		if (children_[i] == child)
			children_.erase(children_.begin() + i);
	child->setParent(0);
}

/*----------------------------------------------------------------------------*/
void IGuiComponent::render(AppBase *pApp) {
	/*vec2 topLeft = vec2((float)absPosition_.x() / (float)viewportX, (float)absPosition_.y() / (float)viewportY);
	vec2 bottomRight = topLeft + vec2((float)absSize_.x() / (float)viewportX, (float)absSize_.y() / (float)viewportY);

	OOGL_CALL(glColor4fv(color_));
	OOGL_CALL(glRectfv(topLeft, bottomRight));
	*/

	for (size_t i = 0; i < children_.size(); ++i)
		children_[i]->render(pApp);
}



/*----------------------------------------------------------------------------*/
bool IGuiComponent::mouseDown(const ivec2 &position, const MouseButtonT &button) {
	if (!(visible_ && enabled_))
		return false;

//	pass to focus first:
	if (focusChild_)
		if (focusChild_->mouseDown(position, button))
			return true;

//	now check if is inside:
	if (!isInside(position))
		return false;

//	pass to children (in z order):
	for (int i = children_.size() - 1; i >= 0; --i)
		if (children_[i]->mouseDown(position, button))
			return true;

	return false; // not handled
}

/*----------------------------------------------------------------------------*/
bool IGuiComponent::mouseUp(const ivec2 &position, const MouseButtonT &button) {
	if (!(visible_ && enabled_))
		return false;

//	pass to focus first:
	if (focusChild_) {
		if (focusChild_->mouseUp(position, button))
			return true;
	}

//	now check if is inside:
	if (!isInside(position))
		return false;

//	pass to children (in z order):
	for (int i = children_.size() - 1; i >= 0; --i)
		if (children_[i]->mouseUp(position, button))
			return true;

	return false; // not handled
}

/*----------------------------------------------------------------------------*/
bool IGuiComponent::mouseMove(const ivec2 &position) {
	if (!(visible_ && enabled_))
		return false;

//	pass to focus first:
	if (focusChild_)
		if (focusChild_->mouseMove(position))
			return true;

//	now check if is inside:
	if (!isInside(position))
		return false;

//	pass to children (in z order):
	for (int i = children_.size() - 1; i >= 0; --i)
		if (children_[i]->mouseMove(position))
			return true;

	return false; // not handled
}

//	PROTECTED:

//	MEMBERS:

Shader *IGuiComponent::shaderSprite_ = 0;
GLint IGuiComponent::ulShaderSpriteSize = 0,
		IGuiComponent::ulShaderSpritePosition = 0,
		IGuiComponent::ulShaderSpriteColor = 0;

//	SERVICES:

/*----------------------------------------------------------------------------*/
void IGuiComponent::computeAbsSizePosition() {
	if (!parent_) {
		absSize_ = size_;
		absPosition_ = position_;
	}

	for (int j = 0; j < children_.size(); ++j) {
		IGuiComponent &thisChild = *children_[j];

		thisChild.absSize_ = thisChild.size_;
		thisChild.absPosition_ = thisChild.position_;

		for (int i = 0; i < 2; ++i) {
			if (thisChild.sizeRef_[i] == RELATIVE)
				thisChild.absSize_[i] = absSize_[i] * thisChild.size_[i] / 100;

			if (thisChild.positionRef_[i] == RELATIVE)
				thisChild.absPosition_[i] = absSize_[i] * thisChild.position_[i] / 100;

			switch (thisChild.alignment_[i]) {
				case RIGHT: // or BOTTOM:
					thisChild.absPosition_[i] = absPosition_[i] + absSize_[i] - thisChild.position_[i] - thisChild.absSize_[i];
					break;
				case CENTER:
					thisChild.absPosition_[i] = absPosition_[i] + absSize_[i] / 2 + thisChild.position_[i] - thisChild.absSize_[i] / 2;
					break;
				case LEFT: // or TOP:
				default:
					thisChild.absPosition_[i] = absPosition_[i] + thisChild.position_[i];
					break;
			};
		}

		thisChild.computeAbsSizePosition();
	}
}

} // namespace frm

