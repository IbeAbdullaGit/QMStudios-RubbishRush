/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_GuiListContainer_H_
#define FRAMEWORK_GuiListContainer_H_

#include <vector>

#include "common.h"
#include "IGuiComponent.h"

namespace frm {

/*	List container; arranges child elements either horizontally or vertically.
	Controls the position of child elemens, but not size. */
class GuiListContainer: public IGuiComponent {
public:

//	CTORS/DTORS:
	static GuiListContainer* create(
		const CStringT &name = "",
		DimensionT direction = VERTICAL
	);

	static void destroy(GuiListContainer **ppComponent);

//	ACCESSORS:
	DimensionT getDirection() const;
	void setDirection(DimensionT);

//	SERVICES:
	/*virtual void render(AppBase *pApp) {
		vec2 topLeft = getAbsPosition();
		vec2 bottomRight = topLeft + getAbsSize();

		glColor4fv(color_);
		glRectfv(topLeft, bottomRight);

		for (size_t i = 0; i < children_.size(); ++i)
			children_[i]->render(viewportX, viewportY);
	}*/

protected:
//	MEMBERS:
	DimensionT direction_; // either horizontal or vertical

//	CTORS/DTORS:
	GuiListContainer(
		const CStringT &name,
		DimensionT direction
	);
	/*	*/
	virtual ~GuiListContainer();

//	SERVICES:
	/*	Compute & cache the absolute size/position in render coordinates. */
	virtual void computeAbsSizePosition();

}; // class GuiListContainer

/*******************************************************************************
	GuiListContainer inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:
/*----------------------------------------------------------------------------*/
inline GuiListContainer* GuiListContainer::create(
	const CStringT &name,
	DimensionT direction
) {
	GuiListContainer *result = new(std::nothrow) GuiListContainer(
		name,
		direction
	);
	assert(result);
	return result;
}

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline IGuiComponent::DimensionT GuiListContainer::getDirection() const {
	return direction_;
}

/*----------------------------------------------------------------------------*/
inline void GuiListContainer::setDirection(DimensionT direction) {
	direction_ = direction;
	computeAbsSizePosition();
}

//	SERVICES:

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline GuiListContainer::GuiListContainer(
	const CStringT &name,
	DimensionT direction
):	IGuiComponent(
		name,
		ivec2(0),
		LEFT, TOP,
		ABSOLUTE, ABSOLUTE,
		ivec2(0),
		ABSOLUTE, ABSOLUTE,
		vec4(1.0f)
	),
	direction_(direction) {
}

inline GuiListContainer::~GuiListContainer() {
}

/*----------------------------------------------------------------------------*/
inline void GuiListContainer::computeAbsSizePosition() {
	IGuiComponent::computeAbsSizePosition(); // call base implementation

	float sum = 0.0f; // distance sum
	for (size_t i = 0; i < children_.size(); ++i) {
		IGuiComponent &child = *children_[i];
		child.positionRef_[direction_] = ABSOLUTE;
		child.position_[direction_] = absPosition_[direction_] + sum;
		child.computeAbsSizePosition();
		sum += child.absSize_[direction_];

		//child.computeAbsSizePosition();
	}
	absSize_[direction_] = sum;
}

} // namespace frm

#endif // FRAMEWORK_GuiListContainer_H_



