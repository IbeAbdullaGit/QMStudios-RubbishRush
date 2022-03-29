/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#include "common.h"
#include "Buffer.h"
#include "Font.h"
#include "Shader.h"
#include "ShaderUnit.h"
#include "ITexture.h"
	#include "Texture2d.h"
#include "IGuiComponent.h"
#include "Mesh.h"
#include "Model.h"
#include "Node.h"
#include "INodeModifier.h"
#include "Object.h"
#include "IMaterial.h"
#include "Framebuffer.h"

namespace frm {

Font *fontDefault = 0;
Model *modelSAQuad = 0;
Texture2d *texBlack = 0, *texWhite = 0, *texNorm = 0;

/*----------------------------------------------------------------------------*/
bool init() {
	Texture2d *fontDefaultTex = Texture2d::loadFromFile("media/gui/default16.png");
	fontDefaultTex->setMinFilter(GL_LINEAR);
	fontDefaultTex->setMagFilter(GL_LINEAR);

	fontDefault = Font::create(
		fontDefaultTex,
		12, 13,
		-3, 2
	);
	assert(fontDefault);

	modelSAQuad = Model::createSAQuad();
	assert(modelSAQuad);

	texBlack = Texture2d::create(GL_RGBA, 1, 1, GL_RGBA, GL_FLOAT, vec4(0.0f));
	assert(texBlack);

	texWhite = Texture2d::create(GL_RGBA, 1, 1, GL_RGBA, GL_FLOAT, vec4(1.0f));
	assert(texWhite);

	texNorm = Texture2d::create(GL_RGBA, 1, 1, GL_RGBA, GL_FLOAT, vec4(0.5f, 0.5f, 1.0f, 0.0f));
	assert(texNorm);

	return true;
}

/*----------------------------------------------------------------------------*/
void cleanup() {
	Buffer::destroyAll();
	IMaterial::destroyAll();
	Model::destroyAll();
	Mesh::destroyAll();
	IGuiComponent::destroyAll();
	Font::destroyAll();
	ITexture::destroyAll();
	Framebuffer::destroyAll();
	Shader::destroyAll();
	ShaderUnit::destroyAll();
	Object::destroyAll();
	Node::destroyAll();
	INodeModifier::destroyAll();
}


} // namespace frm
