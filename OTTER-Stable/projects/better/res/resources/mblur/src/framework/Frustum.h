/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_FRUSTUM_H_
#define FRAMEWORK_FRUSTUM_H_

#include "common.h"
#include "Plane.h"

namespace frm {

/*	Bounding volume of 6 planes. */
class Frustum {
public:
//	TYPEDEFS/ENUMS
	enum SideT {
		FRONT		= 0,
		BACK		= 1,
		TOP		= 2,
		RIGHT		= 3,
		BOTTOM	= 4,
		LEFT		= 5,
		NPLANES
	};

// CTORS/DTORS:
	Frustum();

	Frustum(
		float aspect,
		float fov,
		float clipNear,
		float clipFar,
		bool isOrthographic	= false
	);

//	ACCESSORS:
	const Plane& getFront() const;
	const Plane& getBack() const;
	const Plane& getTop() const;
	const Plane& getRight() const;
	const Plane& getBottom() const;
	const Plane& getLeft() const;

	/*	Direct access to vertex array (contains frustum corners). */
	const vec3* getVertices() const;

//	SERVICES:
	/*	Return whether the given point is inside the frustum. */
	bool isInside(const vec3 &point) const;

	/*	Return whether the given sphere is inside the frustum. */
	bool isInside(const vec3 &origin, float radius) const;

	/*	Transform each plane by m. */
	void transform(const mat4 &m);

private:
//	MEMBERS:
	Plane planes_[NPLANES];
	vec3 vertices_[8]; // frustum vertices

//	SERVICES:
	/*	Construct planes from vertices. */
	void initPlanes();

}; // class Frustum

/*******************************************************************************
	Frustum inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Frustum::Frustum() {
}

/*----------------------------------------------------------------------------*/
inline Frustum::Frustum(
	float aspect,
	float fov,
	float clipNear,
	float clipFar,
	bool isOrthographic
) {
	float tanHalfFov = sml::tan(fov * 0.5f);
	if (isOrthographic) {
		float right = tanHalfFov * clipFar * aspect;
		float up = tanHalfFov * clipFar;

	//	front plane vertices:
		vertices_[0] = vec3(right, up, clipNear);
		vertices_[1] = vec3(-right, up, clipNear);
		vertices_[2] = vec3(-right, -up, clipNear);
		vertices_[3] = vec3(right, -up, clipNear);
	//	back plane vertices:
		vertices_[4] = vec3(right, up, clipFar);
		vertices_[5] = vec3(-right, up, clipFar);
		vertices_[6] = vec3(-right, -up, clipFar);
		vertices_[7] = vec3(right, -up, clipFar);
	} else {
	//	front plane vertices:
		float fy = tanHalfFov * clipNear;
		float fx = aspect * fy;
		vertices_[0] = vec3(fx, fy, clipNear);
		vertices_[1] = vec3(-fx, fy, clipNear);
		vertices_[2] = vec3(-fx, -fy, clipNear);
		vertices_[3] = vec3(fx, -fy, clipNear);
	//	back plane vertices:
		float by = tanHalfFov * clipFar;
		float bx = aspect * by;
		vertices_[4] = vec3(bx, by, clipFar);
		vertices_[5] = vec3(-bx, by, clipFar);
		vertices_[6] = vec3(-bx, -by, clipFar);
		vertices_[7] = vec3(bx, -by, clipFar);
	}

	initPlanes();
}

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline const Plane& Frustum::getFront() const {
	return planes_[FRONT];
}
/*----------------------------------------------------------------------------*/
inline const Plane& Frustum::getBack() const {
	return planes_[BACK];
}
/*----------------------------------------------------------------------------*/
inline const Plane& Frustum::getTop() const {
	return planes_[TOP];
}
/*----------------------------------------------------------------------------*/
inline const Plane& Frustum::getRight() const {
	return planes_[RIGHT];
}
/*----------------------------------------------------------------------------*/
inline const Plane& Frustum::getBottom() const {
	return planes_[BOTTOM];
}
/*----------------------------------------------------------------------------*/
inline const Plane& Frustum::getLeft() const {
	return planes_[LEFT];
}

/*----------------------------------------------------------------------------*/
inline const vec3* Frustum::getVertices() const {
	return vertices_;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline bool Frustum::isInside(const vec3 &point) const {
//	if the point is outside any of the planes, it is outside the frustum:
	for (int i = 0; i < NPLANES; ++i)
		if (planes_[i].distance(point) < 0)
			return false;
	return true;
}

/*----------------------------------------------------------------------------*/
inline bool Frustum::isInside(const vec3 &origin, float radius) const {
	for (int i = 0; i < NPLANES; ++i)
		if (planes_[i].distance(origin) < -radius)
			return false;
	return true;
}

/*----------------------------------------------------------------------------*/
inline void Frustum::transform(const mat4 &m) {
	for (int i = 0; i < NPLANES; ++i)
		planes_[i].transform(m);
}


//	PRIVATE:

//	SERVICES:

/*----------------------------------------------------------------------------*/
inline void Frustum::initPlanes() {
	planes_[FRONT]		= Plane(vertices_[0], vertices_[1], vertices_[2]);
	planes_[BACK]		= Plane(vertices_[6], vertices_[5], vertices_[4]);
	planes_[TOP]		= Plane(vertices_[0], vertices_[4], vertices_[5]);
	planes_[RIGHT]		= Plane(vertices_[1], vertices_[5], vertices_[6]);
	planes_[BOTTOM]	= Plane(vertices_[2], vertices_[6], vertices_[7]);
	planes_[LEFT]		= Plane(vertices_[3], vertices_[7], vertices_[4]);
}

//	SERVICES:

} // namespace frm

#endif // FRAMEWORK_FRUSTUM_H_
