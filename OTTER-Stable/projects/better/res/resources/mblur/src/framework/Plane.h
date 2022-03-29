/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_PLANE_H_
#define FRAMEWORK_PLANE_H_

#include "common.h"

namespace frm {

/*	Represents a half-space. */
class Plane {
public:
// CTORS/DTORS:
	/*	Init as a plane at the origin, oriented along the +z axis */
	Plane();

	/* Init from coefficients of the generalized plane equation. */
	Plane(float a, float b, float c, float d);

	/*	Init from three coplanar points. */
	Plane(const vec3 &p0, const vec3 &p1, const vec3 &p2);

	/*	Init from origin/normal. */
	Plane(const vec3 &origin, const vec3 &normal);

//	ACCESSORS:
	const vec3& getNormal() const;
	void setNormal(const vec3&); // input assumed to be normalized

	float getOffset() const;
	void setOffset(float);

	vec3 getOrigin() const;

//	SERVICES:
	/*	Return distance of a point frin the plane (positive if in front, negative
		if behind. */
	float distance(const vec3 &point) const;

	/*	Return closest point on the plane to the given point. */
	vec3 closest(const vec3 &point) const;

	/*	Transform plane by the give matrix. */
	void transform(const mat4 &m);

private:
//	MEMBERS:
	vec3 normal_;
	float offset_;

}; // class Plane

/*******************************************************************************
	Plane inline implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
inline Plane::Plane()
:	normal_(vec3::zAxis()), offset_(0) {
}

/*----------------------------------------------------------------------------*/
inline Plane::Plane(float a, float b, float c, float d)
:	normal_(a, b, c) {
	offset_ = d / normal_.sqlength();
	normal_.normalize();
}

/*----------------------------------------------------------------------------*/
inline Plane::Plane(const vec3 &p0, const vec3 &p1, const vec3 &p2) {
	vec3 u(p1 - p0);
	vec3 v(p2 - p0);
	normal_ = normalize(sml::cross(u, v));
	offset_ = sml::dot(normal_, p0);
}

/*----------------------------------------------------------------------------*/
inline Plane::Plane(const vec3 &origin, const vec3 &normal)
:	normal_(normal) {
	offset_ = sml::dot(normal_, origin);
}


//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline const vec3& Plane::getNormal() const {
	return normal_;
}

/*----------------------------------------------------------------------------*/
inline void Plane::setNormal(const vec3 &n) {
	normal_ = n;
}

/*----------------------------------------------------------------------------*/
inline float Plane::getOffset() const {
	return offset_;
}
/*----------------------------------------------------------------------------*/
inline void Plane::setOffset(float o) {
	offset_ = o;
}

/*----------------------------------------------------------------------------*/
inline vec3 Plane::getOrigin() const {
	return normal_ * offset_;
}


//	SERVICES:
/*----------------------------------------------------------------------------*/
inline float Plane::distance(const vec3 &point) const {
	return sml::dot(normal_, point) - offset_;
}

/*----------------------------------------------------------------------------*/
inline vec3 Plane::closest(const vec3 &point) const {
	return point - normal_ * (sml::dot(normal_, point) - offset_);
}

/*----------------------------------------------------------------------------*/
inline void Plane::transform(const mat4 &m) {
	vec3 newNormal = mat3(m) * normal_;
	vec3 origin = m * getOrigin();
	normal_ = normalize(newNormal);
	offset_ = sml::dot(normal_, origin);
}

} // namespace frm

#endif // FRAMEWORK_PLANE_H_
