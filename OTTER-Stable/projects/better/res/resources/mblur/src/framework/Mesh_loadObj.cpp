/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include <cstring> // strlen, strcmp
#include <cstdio> // file io

#include <vector>

#include "Mesh.h"
#include "common.h"

namespace  {

/*------------------------------------------------------------------------------
	Advance i to the next line (after '\n') of buffer. Return false if size ==
	limit. */
bool nextLine(const char *buffer, size_t *i, size_t limit) {
	assert(buffer);

	do {
		if (*i == limit)
			return false;
		++(*i);
	} while (buffer[*i] != '\n');
	++(*i);

	return true;
}

} // namespace

namespace frm {

/*----------------------------------------------------------------------------*/
Mesh* Mesh::loadFromObj(const CStringT &fileName, unsigned mask) {
	assert(fileName);

	oogl::logEvent("Loading mesh from \"%s\"...", fileName);

//	load file into memory:
	FILE *pFile = fopen(fileName, "rb");
	if (!pFile) {
		oogl::logEvent("Error opening file");
		return 0;
	}
	fseek(pFile, 0, SEEK_END);
	size_t fileLength = ftell(pFile);
	rewind(pFile);

	char *fileBuffer = new(std::nothrow) char[fileLength];
	assert(fileBuffer);

	size_t bytesRead = fread(fileBuffer, 1, fileLength, pFile);
	fclose(pFile);
	if (bytesRead != fileLength) {
		oogl::logEvent("Error reading from file %u %u", bytesRead, fileLength);
		return 0;
	}

//	allocate buffers for vertices, texcoords, normals:
	vec3 *vertexBuffer = 0, *normalBuffer = 0;
	vec2 *texcoordBuffer = 0;
	size_t vertexBufferSize = 0, normalBufferSize = 0, texcoordBufferSize = 0;

	size_t i = 0;
	do {
		switch (fileBuffer[i]) {
			case 'V': // vertex data
			case 'v':
				switch (fileBuffer[i + 1]) {
					case ' ': // vertex position
						++vertexBufferSize;
						break;

					case 'N': // vertex normal
					case 'n':
						++normalBufferSize;
						break;

					case 'T': // vertex texcoord
					case 't':
						++texcoordBufferSize;
						break;

					default:
						break;
				};
				break;

			default:
				break;
		};
	} while (nextLine(fileBuffer, &i, fileLength));

	vertexBuffer = new(std::nothrow) vec3[vertexBufferSize];
	assert(vertexBuffer);
	normalBuffer = new(std::nothrow) vec3[normalBufferSize];
	assert(normalBuffer);
	texcoordBuffer = new(std::nothrow) vec2[texcoordBufferSize];

	OOGL_LOG_RESOURCE_EVENT("\t%u vertices, %u normals, %u texcoords", vertexBufferSize, normalBufferSize, texcoordBufferSize);

//	parse/process data:
	Mesh *result = new(std::nothrow) Mesh(0);
	assert(result);

	//	tmp list for expanding smoothing groups, used to prevent duplicate vertices
	//	within a smoothing group. realIndex is used to track the actual vertex
	//	index which may be different to what's in the OBJ file!
	struct TmpVertexT { size_t smoothGroup; IndexT vt; IndexT realIndex; };
	TmpVertexT *tmpVertexList = new(std::nothrow) TmpVertexT[vertexBufferSize];
	assert(tmpVertexList);
	for (size_t i = 0; i < vertexBufferSize; ++i) {
		tmpVertexList[i].smoothGroup = 0;
		tmpVertexList[i].vt = 0;
		tmpVertexList[i].realIndex = i;
	}

	i = 0; vertexBufferSize = 0; normalBufferSize = 0; texcoordBufferSize = 0;
	size_t thisSmoothGroup = 0;
	do {
		switch (fileBuffer[i]) {
			case 'V': // vertex data
			case 'v':
				switch (fileBuffer[i + 1]) {
					case ' ': { // vertex position
						float x, y, z;
						if (sscanf(&fileBuffer[i + 2], "%f %f %f", &x, &y, &z) != 3) {
						//	error! do something
							return 0;
						}
//	flip z, we want the model to be in the correct left-handed space
z = -z;
						vertexBuffer[vertexBufferSize] = vec3(x, y, z);
						++vertexBufferSize;
						break;
					}

					case 'N': // vertex normal
					case 'n': {
						float x, y, z;
						if (sscanf(&fileBuffer[i + 2], "%f %f %f", &x, &y, &z) != 3) {
						//	error! do something
							return 0;
						}
//	flip normal z, too!
z = -z;
						normalBuffer[normalBufferSize] = vec3(x, y, z);
						++normalBufferSize;
						break;
					}

					case 'T': // vertex texcoord
					case 't':{
						float u, v;
						if (sscanf(&fileBuffer[i + 2], "%f %f", &u, &v) != 2) {
						//	error! do something
							return 0;
						}

						texcoordBuffer[texcoordBufferSize] = vec2(u, v);
						++texcoordBufferSize;
						break;
					}

					default:
						break;
				};
				break;

			case 'S': // smoothing group
			case 's':
				++thisSmoothGroup;
				break;

			case 'F': // face
			case 'f': {
				assert(vertexBufferSize); // must at least have some vertices

			//	read in indices:
				struct OBJVertexT { size_t v, vt, vn; };
				OBJVertexT tmpTri[3];
				OBJVertexT &a = tmpTri[0];
				OBJVertexT &b = tmpTri[1];
				OBJVertexT &c = tmpTri[2];

				if (sscanf(&fileBuffer[i + 2], "%u/%u/%u %u/%u/%u %u/%u/%u", &a.v, &a.vt, &a.vn, &b.v, &b.vt, &b.vn, &c.v, &c.vt, &c.vn) != 9)
					if (sscanf(&fileBuffer[i + 2], "%u//%u %u//%u %u//%u", &a.v, &a.vn, &b.v, &b.vn, &c.v, &c.vn) != 6)
						if (sscanf(&fileBuffer[i + 2], "%u/%u %u/%u %u/%u", &a.v, &a.vt, &b.v, &b.vt, &c.v, &c.vt) != 6)
							if (sscanf(&fileBuffer[i + 2], "%u %u %u", &a.v, &b.v, &c.v) != 3) {
							//	error, unsupported polygon type
								return 0;
							}

			//	construct final triangle:
				TriangleT tri;
				for (size_t j = 0; j < 3; ++j) {
				//	correct indices:
					--tmpTri[j].v;
					--tmpTri[j].vt;
					--tmpTri[j].vn;

					if (tmpVertexList[tmpTri[j].v].smoothGroup == thisSmoothGroup
						&& tmpVertexList[tmpTri[j].v].vt == tmpTri[j].vt) {
						tri[j] = tmpVertexList[tmpTri[j].v].realIndex;
					} else {
						tmpVertexList[tmpTri[j].v].smoothGroup = thisSmoothGroup;
						tmpVertexList[tmpTri[j].v].vt = tmpTri[j].vt;
						tmpVertexList[tmpTri[j].v].realIndex = result->nVertices();
						tri[j] = result->nVertices();

						VertexT ver;
						ver.position = vertexBuffer[tmpTri[j].v];
						if (texcoordBufferSize)
							ver.texcoord = texcoordBuffer[tmpTri[j].vt];
						if (normalBufferSize)
							ver.normal = normalBuffer[tmpTri[j].vn];
						result->addVertex(ver);

					}
				}

				result->addTriangle(tri);

				break;
			}

			default:
				break;
		};
	} while (nextLine(fileBuffer, &i, fileLength));

	delete[] tmpVertexList;
	delete[] vertexBuffer;
	delete[] normalBuffer;
	delete[] texcoordBuffer;
	delete[] fileBuffer;


	result->vertexMask_ = mask;
	if (mask & TANGENTS)
		result->generateTangents();
	result->calculateExtents();

	OOGL_LOG_RESOURCE_EVENT("\tGenerated %u vertices, %u triangles", result->nVertices(), result->nTriangles());

	oogl::logEvent("...DONE");

	return result;
}

} // namespace frm
