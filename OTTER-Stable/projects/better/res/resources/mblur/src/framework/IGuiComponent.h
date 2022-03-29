/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_IGuiComponent_H_
#define FRAMEWORK_IGuiComponent_H_

#include <vector>

#include "common.h"
#include "IResource.h"

namespace frm {

/*	Base class for GUI components. All absolute dimensions are specified in
	PIXELS, relative dimensions are expressed as a percentage. */
class IGuiComponent: public IResource<IGuiComponent> {
public:
//	TYPEDEFS/ENUMS:
	enum DimensionT {
		X						= 0,
		HORIZONTAL			= 0,
		Y						= 1,
		VERTICAL				= 1
	};

	enum ReferenceT {
		RELATIVE, // % of parent
		ABSOLUTE // absolute, relative to parent in screen units
	};

	enum AlignT {
		LEFT			= 0,
		TOP			= 0,
		CENTER		= 1,
		RIGHT			= 2,
		BOTTOM		= 2
	};

	enum MouseButtonT {
		LBUTTON		= 0,
		MBUTTON		= 1,
		RBUTTON		= 2
	};

//	CTORS/DTORS:
	static IGuiComponent* create(
		const CStringT &name = "",
		const ivec2 &position = ivec2(0),
		AlignT alignX = LEFT, AlignT alignY = TOP,
		ReferenceT posRefX = ABSOLUTE, ReferenceT posRefY = ABSOLUTE,
		const ivec2 &size = ivec2(100),
		ReferenceT sizeRefX = RELATIVE, ReferenceT sizeRefY = RELATIVE,
		const vec4 &color = vec4(1.0f)
	);

	static void destroy(IGuiComponent **ppComponent);

//	ACCESSORS:
	void show();
	void hide();
	void toggleVisibility();
	bool isVisible();

	void enable();
	void disable();
	void toggleEnabled();
	bool isEnabled();

	const ivec2& getAbsPosition() const;
	void setAbsPosition(const ivec2&);

	const ivec2& getAbsSize() const;
	void setAbsSize(const ivec2&);

	const ivec2& getPosition() const;
	void setPosition(const ivec2&);

	AlignT getAlignmentX() const;
	AlignT getAlignmentY() const;
	const AlignT* getAlignment() const;
	void setAlignment(AlignT x, AlignT y);

	ReferenceT getPositionRefX() const;
	ReferenceT getPositionRefY() const;
	const ReferenceT* getPositionRef() const;
	void setPositionRef(ReferenceT x, ReferenceT y);

	const ivec2& getSize() const;
	void setSize(const ivec2&);

	ReferenceT getSizeRefX() const;
	ReferenceT getSizeRefY() const;
	const ReferenceT* getSizeRef() const;
	void setSizeRef(ReferenceT x, ReferenceT y);

	const vec4& getColor() const;
	void setColor(const vec4&);

	IGuiComponent* getParent() const;
	void setParent(IGuiComponent*);

	IGuiComponent* getFocus() const;
	void setFocus(IGuiComponent*);



//	SERVICES:
	/*	Become parent's focusChild_. This cascades up the hierarchy. */
	void focus();
	/*	Stop being parent's focusChild_, optionally set a new focus. */
	void unfocus(IGuiComponent *newFocus = 0);

	/*	Determine whether a point is inside this component's bounding rectangle.
		point should be expressed in pixels, relative to the top left corner. */
	bool isInside(const ivec2 &point);

	/*	Add to this IGuiComponent's child list. Removes child from its parent node
		(if applicable). */
	void addChild(IGuiComponent *child);
	/*	Remove from this IGuiComponent's child list. */
	void removeChild(IGuiComponent *child);

	/*	Render this first, then call render() on children in ascending order. This
		means that the child order is the z-order. */
	virtual void render(AppBase *pApp);

	/*	Handle mouse events. Return true if the event was consumed. */
	virtual bool mouseDown(const ivec2 &position, const MouseButtonT &button);
	virtual bool mouseUp(const ivec2 &position, const MouseButtonT &button);
	virtual bool mouseMove(const ivec2 &position);

//protected: // KLUDGE
//	MEMBERS:
	bool enabled_, visible_;

	vec4 color_;

	ivec2 position_, size_;
	mutable ivec2 absPosition_, absSize_;
	ReferenceT positionRef_[2], sizeRef_[2];
	AlignT alignment_[2];

	IGuiComponent *parent_;
	IGuiComponent *focusChild_; // gets first chance to handle events (for dragging, etc.)
	std::vector<IGuiComponent*> children_; // render forwards, handle events backwards

	static Shader *shaderSprite_;
	static GLint ulShaderSpriteSize, ulShaderSpritePosition, ulShaderSpriteColor;

//	CTORS/DTORS:
	IGuiComponent(
		const CStringT &name,
		const ivec2 &position,
		AlignT alignX, AlignT alignY,
		ReferenceT posRefX, ReferenceT posRefY,
		const ivec2 &size,
		ReferenceT sizeRefX, ReferenceT sizeRefY,
		const vec4 &color
	);
	/*	Dtor needs to be virtual, as you'll be deleting all gui objects via an
		IGuiComponent ptr. */
	virtual ~IGuiComponent();

//	SERVICES:
	/*	Compute & cache the absolute size/position in render coordinates. */
	virtual void computeAbsSizePosition();

}; // class IGuiComponent

/*******************************************************************************
	IGuiComponent inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline void IGuiComponent::show() {
	visible_ = true;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::hide() {
	visible_ = false;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::toggleVisibility() {
	visible_ = !visible_;
}
/*----------------------------------------------------------------------------*/
inline bool IGuiComponent::isVisible() {
	return visible_;
}

/*----------------------------------------------------------------------------*/
inline void IGuiComponent::enable() {
	enabled_ = true;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::disable() {
	enabled_ = false;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::toggleEnabled() {
	enabled_ = !enabled_;
}
/*----------------------------------------------------------------------------*/
inline bool IGuiComponent::isEnabled() {
	return enabled_;
}

/*----------------------------------------------------------------------------*/
inline const ivec2& IGuiComponent::getAbsPosition() const {
	return absPosition_;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::setAbsPosition(const ivec2 &absPosition) {
	absPosition_ = absPosition;
}


/*----------------------------------------------------------------------------*/
inline const ivec2& IGuiComponent::getAbsSize() const {
	return absSize_;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::setAbsSize(const ivec2 &absSize) {
	absSize_ = absSize;
}

/*----------------------------------------------------------------------------*/
inline const ivec2& IGuiComponent::getPosition() const {
	return position_;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::setPosition(const ivec2 &position) {
	position_ = position;
	computeAbsSizePosition();
}

/*----------------------------------------------------------------------------*/
inline IGuiComponent::AlignT IGuiComponent::getAlignmentX() const {
	return alignment_[X];
}
/*----------------------------------------------------------------------------*/
inline IGuiComponent::AlignT IGuiComponent::getAlignmentY() const {
	return alignment_[Y];
}
/*----------------------------------------------------------------------------*/
inline const IGuiComponent::AlignT* IGuiComponent::getAlignment() const {
	return alignment_;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::setAlignment(AlignT x, AlignT y) {
	alignment_[X] = x; alignment_[Y] = y;
	computeAbsSizePosition();
}

/*----------------------------------------------------------------------------*/
inline IGuiComponent::ReferenceT IGuiComponent::getPositionRefX() const {
	return positionRef_[X];
}
/*----------------------------------------------------------------------------*/
inline IGuiComponent::ReferenceT IGuiComponent::getPositionRefY() const {
	return positionRef_[Y];
}
/*----------------------------------------------------------------------------*/
inline const IGuiComponent::ReferenceT* IGuiComponent::getPositionRef() const {
	return positionRef_;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::setPositionRef(ReferenceT x, ReferenceT y) {
	positionRef_[X] = x; positionRef_[Y] = y;
	computeAbsSizePosition();
}

/*----------------------------------------------------------------------------*/
inline const ivec2& IGuiComponent::getSize() const {
	return size_;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::setSize(const ivec2 &size) {
	size_ = size;
	computeAbsSizePosition();
}

/*----------------------------------------------------------------------------*/
inline IGuiComponent::ReferenceT IGuiComponent::getSizeRefX() const {
	return sizeRef_[X];
}
/*----------------------------------------------------------------------------*/
inline IGuiComponent::ReferenceT IGuiComponent::getSizeRefY() const {
	return sizeRef_[Y];
}
/*----------------------------------------------------------------------------*/
inline const IGuiComponent::ReferenceT* IGuiComponent::getSizeRef() const {
	return sizeRef_;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::setSizeRef(ReferenceT x, ReferenceT y) {
	sizeRef_[X] = x; sizeRef_[Y] = y;
	computeAbsSizePosition();
}

/*----------------------------------------------------------------------------*/
inline const vec4& IGuiComponent::getColor() const {
	return color_;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::setColor(const vec4 &color) {
	color_ = color;
}

/*----------------------------------------------------------------------------*/
inline IGuiComponent* IGuiComponent::getParent() const {
	return parent_;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::setParent(IGuiComponent *parent) {
	parent_ = parent;
}

/*----------------------------------------------------------------------------*/
inline IGuiComponent* IGuiComponent::getFocus() const {
	return focusChild_;
}
/*----------------------------------------------------------------------------*/
inline void IGuiComponent::setFocus(IGuiComponent *focusChild) {
	focusChild_ = focusChild;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline void IGuiComponent::focus() {
	if (parent_) {
		if (parent_->focusChild_)
			parent_->focusChild_->unfocus(this); // unfocus current, focus this
		else
			parent_->focusChild_ = this;

		parent_->focus();
	}
}

/*----------------------------------------------------------------------------*/
inline void IGuiComponent::unfocus(IGuiComponent *newFocus) {
	if (parent_) {
		//assert(parent_->focusChild_ == this); // not necessarily!
		parent_->focusChild_ = newFocus;
	}
}

/*----------------------------------------------------------------------------*/
inline bool IGuiComponent::isInside(const ivec2 &point) {
	ivec2 topLeft = getAbsPosition();
	ivec2 bottomRight = topLeft + getAbsSize();
	if (point.x() >= topLeft.x() && point.x() <= bottomRight.x())
		if (point.y() <= bottomRight.y() && point.y() >= topLeft.y())
			return true;
	return false;
}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline IGuiComponent::IGuiComponent(
	const CStringT &name,
	const ivec2 &position,
	AlignT alignX, AlignT alignY,
	ReferenceT posRefX, ReferenceT posRefY,
	const ivec2 &size,
	ReferenceT sizeRefX, ReferenceT sizeRefY,
	const vec4 &color
):	IResource(name),
	enabled_(true),
	visible_(true),
	color_(color),
	position_(position),
	size_(size),
	parent_(0),
	focusChild_(0) {

	positionRef_[0] = posRefX; positionRef_[1] = posRefY;
	sizeRef_[0] = sizeRefX; sizeRef_[1] = sizeRefY;
	alignment_[0] = alignX; alignment_[1] = alignY;

	computeAbsSizePosition();
}

inline IGuiComponent::~IGuiComponent() {
}

} // namespace frm

#endif // FRAMEWORK_IGuiComponent_H_


