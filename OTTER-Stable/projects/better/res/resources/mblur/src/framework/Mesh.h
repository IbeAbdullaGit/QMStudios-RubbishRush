/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_MESH_H_
#define FRAMEWORK_MESH_H_

#include <vector>

#include "common.h"
#include "IResource.h"

namespace frm {

/*	Represents a triangle mesh. Vertex format is fixed, in future this may need
	to be more flexible (named vertex 'channels' etc.). */
class Mesh: public IResource<Mesh> {
public:
//	TYPEDEFS/ENUMS:
	enum VertexMaskT {
		NONE			= 0,
		POSITIONS	= 1,
		NORMALS		= 2,
		TANGENTS		= 4,
		TEXCOORDS	= 8
	};

	typedef int IndexT;

	struct VertexT {
		vec3 position;
		vec3 normal;
		vec3 tangent;
		vec2 texcoord;
	};

	struct TriangleT {
		IndexT a, b, c;

		IndexT& operator[](IndexT i) {
			assert(i >= 0 && i < 3);
			return (&a)[i];
		}
	};

//	CTORS/DTORS:
	static Mesh* create(IndexT nTriangles = 0);
	static Mesh* loadFromFile(const CStringT &fileName, unsigned mask = POSITIONS | NORMALS | TANGENTS | TEXCOORDS);
	static Mesh* plane(const vec2 &scale, const ivec2 &segs, unsigned mask = POSITIONS | NORMALS | TANGENTS | TEXCOORDS);
	static Mesh* sphere(float radius, const ivec2 &segs, unsigned mask = POSITIONS | NORMALS | TANGENTS | TEXCOORDS);

	static void destroy(Mesh **ppMesh);

//	ACCESSORS:
	const VertexT& vertex(IndexT i) const;
	VertexT& vertex(IndexT i);
	IndexT nVertices() const;

	const TriangleT& triangle(IndexT i) const;
	TriangleT& triangle(IndexT i);
	IndexT nTriangles() const;

	/*	Return direct ptrs to internal data. Use when uploading to GPU. */
	const VertexT* getVertices() const;
	const TriangleT* getIndices() const;

	unsigned getVertexMask() const;
	void setVertexMask(VertexMaskT);
	bool has(VertexMaskT);

	/*	Need to call calculateExtents() in order for these to be valid. */
	float getRadius() const;
	const vec3& getMinExtents() const;
	const vec3& getMaxExtents() const;
	const vec3* getExtents() const;

//	SERVICES:
	void addVertex(const VertexT&);
	void addTriangle(const TriangleT&);

	/*	Generate normals (compute & average normals for converging triangles). */
	void generateNormals();

	/*	Generate tangents (such that x/y align with u/v texcoords). Assumes that
		valid texture coordinates exist. */
	void generateTangents();

	/*	Transform all vertices by the given matrix (affects normals/tangents). */
	void transform(const mat4&);

	/*	Transform all texcoords by the given matrix. */
	void transformTexcoords(const mat3&);

	/*	Reverse normals. */
	void flipNormals();

	/*	Swap winding order for triangles. */
	void flipWinding();

	/*	Merge vertices within a given threshold. KLUDGE: This doesn't actually
		delete vertices, doesn't do anything with normals/tagents/texcoords. */
	void weld(float threshold = 0.0001f);

	/*	(Re)calculate & cache mesh extents. */
	void calculateExtents();

protected:
//	MEMBERS:
	std::vector<VertexT> vertices_;
	std::vector<TriangleT> indices_;

	unsigned vertexMask_; // which channels are available and valid

	float radius_; // bounding radius
	vec3 extents_[2]; // min/max extents (bounding box)

//	CTORS/DTORS:
	Mesh(IndexT nTriangles);
	virtual ~Mesh();

	/*	Load from Wavefront OBJ file. */
	static Mesh* loadFromObj(const CStringT &fileName, unsigned mask);

}; // class Mesh

/*******************************************************************************
	Mesh inline implementation:
*******************************************************************************/

//	PUBLIC:

//	ACCESSORS:

/*----------------------------------------------------------------------------*/
inline const Mesh::VertexT& Mesh::vertex(IndexT i) const {
	assert(i < nVertices());
	return vertices_[i];
}
/*----------------------------------------------------------------------------*/
inline Mesh::VertexT& Mesh::vertex(IndexT i) {
	assert(i < nVertices());
	return vertices_[i];
}
/*----------------------------------------------------------------------------*/
inline Mesh::IndexT Mesh::nVertices() const {
	return vertices_.size();
}

/*----------------------------------------------------------------------------*/
inline const Mesh::TriangleT& Mesh::triangle(IndexT i) const {
	assert(i < nTriangles());
	return indices_[i];
}
/*----------------------------------------------------------------------------*/
inline Mesh::TriangleT& Mesh::triangle(IndexT i) {
	assert(i < nTriangles());
	return indices_[i];
}
/*----------------------------------------------------------------------------*/
inline Mesh::IndexT Mesh::nTriangles() const {
	return indices_.size();
}

/*----------------------------------------------------------------------------*/
inline const Mesh::VertexT* Mesh::getVertices() const {
	assert(nVertices() != 0);
	return &vertices_[0];
}
/*----------------------------------------------------------------------------*/
inline const Mesh::TriangleT* Mesh::getIndices() const {
	assert(nTriangles() != 0);
	return &indices_[0];
}

/*----------------------------------------------------------------------------*/
inline unsigned Mesh::getVertexMask() const {
	return vertexMask_;
}
/*----------------------------------------------------------------------------*/
inline void Mesh::setVertexMask(VertexMaskT mask) {
	vertexMask_ = mask;
}
/*----------------------------------------------------------------------------*/
inline bool Mesh::has(VertexMaskT check) {
	return check & vertexMask_;
}

/*----------------------------------------------------------------------------*/
inline float Mesh::getRadius() const {
	return radius_;
}
/*----------------------------------------------------------------------------*/
inline const vec3& Mesh::getMinExtents() const {
	return extents_[0];
}
/*----------------------------------------------------------------------------*/
inline const vec3& Mesh::getMaxExtents() const {
	return extents_[1];
}
/*----------------------------------------------------------------------------*/
inline const vec3* Mesh::getExtents() const {
	return extents_;
}


} // namespace frm

#endif // FRAMEWORK_MESH_H_


