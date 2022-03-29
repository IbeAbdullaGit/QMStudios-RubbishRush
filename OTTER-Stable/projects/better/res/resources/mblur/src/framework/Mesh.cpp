/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <cstring> // strlen, strcmp

#include <vector>

#include "Mesh.h"
#include "common.h"

namespace frm {

/*******************************************************************************
	Mesh implementation:
*******************************************************************************/

//	PUBLIC:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
Mesh* Mesh::create(IndexT nTriangles) {
	Mesh *result = new(std::nothrow) Mesh(nTriangles);
	assert(result);
	return result;
}

/*----------------------------------------------------------------------------*/
Mesh* Mesh::loadFromFile(const CStringT &fileName, unsigned mask) {
	assert(fileName);

	const char *ext = strrchr(fileName, '.');
	if (!ext) {
		oogl::logEvent("Error: invalid filename '%s' (no extension).", fileName);
		return 0;
	}

	if (strcmp(ext, ".obj") == 0 || strcmp(ext, ".OBJ") == 0)
		return loadFromObj(fileName, mask);

	oogl::logEvent("Error: unsupported file type (%s).", ext);
	return 0;
}

/*----------------------------------------------------------------------------*/
Mesh* Mesh::plane(const vec2 &scale, const ivec2 &segs, unsigned mask) {
	Mesh *result = new(std::nothrow) Mesh(0);
	assert(result);

	result->vertexMask_ = mask;

//	calculate how many verts/tris, reserve memory:
	IndexT nverts = (segs.x() + 1) * (segs.y() + 1);
	IndexT ntris = segs.x() * segs.y() * 2;
	result->vertices_.reserve(nverts);
	result->indices_.reserve(ntris);

//	generate vertices:
	for (IndexT x = 0; x <= segs.x(); ++x) {
		for (IndexT y = 0; y <= segs.y(); ++y) {
			VertexT v;
			v.position = vec3(
				scale.x() * -0.5f + (scale.x() / (float)segs.x()) * x,
				0.0f,
				scale.y() * -0.5f + (scale.y() / (float)segs.y()) * y
			);

			if (mask & NORMALS)
				v.normal = vec3(0.0f, 1.0f, 0.0f);
			if (mask & TANGENTS)
				v.tangent = vec3(1.0f, 0.0f, 0.0f);
			if (mask & TEXCOORDS)
				v.texcoord = vec2((float)x / (float)segs.x(), (float)y / (float)segs.y());

			result->addVertex(v);
		}
	}

//	generate indices:
	IndexT j = 0;
	for (IndexT i = 0; i < ntris; ++i, ++j) {
		TriangleT tri;
		tri.a = j + 1;
		tri.b = j;
		tri.c = j + segs.x() + 1;
		result->addTriangle(tri);

		tri.a = j + segs.x() + 2;
		tri.b = j + 1;
		tri.c = j + segs.x() + 1;
		result->addTriangle(tri);

		++i;
		if ((j + 2) % (segs.x() + 1) == 0)
			++j;
	}
	result->calculateExtents();
	return result;
}

/*----------------------------------------------------------------------------*/
Mesh* Mesh::sphere(float radius, const ivec2 &segs, unsigned mask) {
	Mesh *result = plane(vec2(sml::CONSTANTS<float>::twoPi, sml::CONSTANTS<float>::pi), segs, mask);
	for (IndexT i = 0; i < result->nVertices(); ++i) {
		Mesh::VertexT &v = result->vertex(i);
		float x = v.position.x();
		float z = v.position.z();
		float x1 = sml::sin(x) * sml::sin(z + sml::CONSTANTS<float>::halfPi);
		float y1 = sml::cos(x) * sml::sin(z + sml::CONSTANTS<float>::halfPi);
		float z1 = z;
		vec3 position(x1, y1, z1);
		position.normalize();
		if (mask & NORMALS)
			v.normal = position;
		v.position = position * radius;
	}
	if (mask & TANGENTS)
		result->generateTangents();
	result->calculateExtents();
	return result;
}

/*----------------------------------------------------------------------------*/
void Mesh::destroy(Mesh **ppMesh) {
	assert(ppMesh != 0);
	assert(*ppMesh != 0);
	delete *ppMesh;
	*ppMesh = 0;
}

//	SERVICES:

/*----------------------------------------------------------------------------*/
void Mesh::addVertex(const VertexT &newVertex) {
	vertices_.push_back(newVertex);
}

/*----------------------------------------------------------------------------*/
void Mesh::addTriangle(const TriangleT &newTriangle) {
	assert(newTriangle.a < nVertices());
	assert(newTriangle.b < nVertices());
	assert(newTriangle.c < nVertices());

	indices_.push_back(newTriangle);
}

/*----------------------------------------------------------------------------*/
void Mesh::generateNormals() {
	assert(nVertices() != 0);
	assert(nTriangles() != 0);

	OOGL_LOG_RESOURCE_EVENT("Generating mesh normals...");

//	initialize normals to zero:
	for (IndexT i = 0; i < nVertices(); ++i)
		vertices_[i].normal = vec3(0.0f);

//	sum normals per-triangle:
	for (IndexT i = 0; i < nTriangles(); ++i) {
		VertexT &a = vertices_[indices_[i].a];
		VertexT &b = vertices_[indices_[i].b];
		VertexT &c = vertices_[indices_[i].c];

		vec3 ab = a.position - b.position;
		vec3 ac = a.position - c.position;
		vec3 norm = sml::cross(ac, ab);

		a.normal += norm;
		b.normal += norm;
		c.normal += norm;
	}

//	normalize per-vertex:
	for (IndexT i = 0; i < nVertices(); ++i)
		vertices_[i].normal.normalize();

	vertexMask_ |= NORMALS;

	OOGL_LOG_RESOURCE_EVENT("...DONE.");
}

/*----------------------------------------------------------------------------*/
void Mesh::generateTangents() {
	assert(nVertices() != 0);
	assert(nTriangles() != 0);

	OOGL_LOG_RESOURCE_EVENT("Generating mesh tangents...");

//	initialize normals to zero:
	for (IndexT i = 0; i < nVertices(); ++i)
		vertices_[i].tangent = vec3(0.0f);

//	sum tangents per-triangle:
	for (IndexT i = 0; i < nTriangles(); ++i) {
		VertexT &a = vertices_[indices_[i].a];
		VertexT &b = vertices_[indices_[i].b];
		VertexT &c = vertices_[indices_[i].c];

		vec3 ba = b.position - a.position;
		vec3 ca = c.position - a.position;
		vec2 tba = b.texcoord - a.texcoord;
		vec2 tca = c.texcoord - a.texcoord;
		float r = 1.0f / (tba.x() * tca.y() - tba.y() * tca.x());
		vec3 tang(
			(tca.y() * ba.x() - tba.y() * ca.x()) * r,
			(tca.y() * ba.y() - tba.y() * ca.y()) * r,
			(tca.y() * ba.z() - tba.y() * ca.z()) * r
		);

		a.tangent += tang;
		b.tangent += tang;
		c.tangent += tang;
	}

//	normalize per-vertex:
	for (IndexT i = 0; i < nVertices(); ++i)
		vertices_[i].tangent.normalize();

	vertexMask_ |= TANGENTS;

	OOGL_LOG_RESOURCE_EVENT("...DONE.");
}

/*----------------------------------------------------------------------------*/
void Mesh::transform(const mat4 &m) {
	mat3 nm = sml::transpose(mat3(sml::inverse(m)));
	for (IndexT i = 0; i < nVertices(); ++i) {
		vertices_[i].position = m * vertices_[i].position;

		vertices_[i].normal = nm * vertices_[i].normal;
		vertices_[i].normal.normalize();

		vertices_[i].tangent = nm * vertices_[i].tangent;
		vertices_[i].tangent.normalize();
	}
}

/*----------------------------------------------------------------------------*/
void Mesh::transformTexcoords(const mat3 &m) {
	for (IndexT i = 0; i < nVertices(); ++i) {
		vertices_[i].texcoord = m * vertices_[i].texcoord;
	}
}

/*----------------------------------------------------------------------------*/
void Mesh::flipNormals() {
	assert(nVertices() != 0);
	assert(has(NORMALS));
	for (IndexT i = 0; i < nVertices(); ++i)
		vertices_[i].normal = -vertices_[i].normal;
}

/*----------------------------------------------------------------------------*/
void Mesh::flipWinding() {
	assert(nVertices() != 0);
	assert(nTriangles() != 0);
	for (IndexT i = 0; i < nTriangles(); ++i) {
		IndexT tmp = indices_[i].a;
		indices_[i].a = indices_[i].b;
		indices_[i].b = tmp;
	}
}

/*----------------------------------------------------------------------------*/
void Mesh::weld(float threshold) {
	assert(nVertices() != 0);

	for (IndexT i = 0; i < nVertices(); ++i) { // for every vertex
		VertexT &v1 = vertices_[i];

		for (IndexT j = i + 1; j < nVertices(); ++j) { // for every other vertex
			VertexT &v2 = vertices_[j];

			float dist = sml::length(v1.position - v2.position);
			if (dist < threshold) {

				for (IndexT k = 0; k < nTriangles(); ++k) { // for every triangle
				//	if any index poitns to j, point to i instead:
					for (IndexT m = 0; m < 3; ++m) {
						if (indices_[k][m] == j)
							indices_[k][m] = i;
					}
				}
			}
		}
	}

}

/*----------------------------------------------------------------------------*/
void Mesh::calculateExtents() {
	assert(nVertices() != 0);

	radius_ = 0.0f;
	extents_[0] = vec3(0.0f);
	extents_[1] = vec3(0.0f);

	for (IndexT i = 0; i < nVertices(); ++i) {
		extents_[0] = sml::min(extents_[0], vertices_[i].position);
		extents_[1] = sml::max(extents_[1], vertices_[i].position);
		radius_ = sml::max(vertices_[i].position.length(), radius_);
	}


}

//	PROTECTED:

//	CTORS/DTORS:

/*----------------------------------------------------------------------------*/
Mesh::Mesh(IndexT nTriangles)
:	vertexMask_(NONE) {
	vertices_.reserve(nTriangles * 3);
	indices_.reserve(nTriangles);
}

/*----------------------------------------------------------------------------*/
Mesh::~Mesh() {
}

} // namespace frm


