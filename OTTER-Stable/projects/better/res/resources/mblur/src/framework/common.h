/*******************************************************************************
	Copyright (C) 2013 John Chapman

	This software is distributed freely under the terms of the MIT License.
	See "license.txt" or "http://copyfree.org/licenses/mit/license.txt".
*******************************************************************************/
#ifndef FRAMEWORK_COMMON_H_
#define FRAMEWORK_COMMON_H_

#ifndef NDEBUG
	#include <iostream>
#endif
#include <assert.h>

//	OpenGL wrappers/utils:
#ifndef NDEBUG
	#define OOGL_DEBUG
#endif
#include <oogl/oogl.h>
#include <oogl/utils.h>

//	3d maths:
#ifndef NDEBUG
	#define SML_DEBUG
	//#define SML_STRICT_DEBUG
#endif
#define SML_NO_ALIGN
#include <sml/sml.h>
#include <sml/itpl.h>



#define FRAMEWORK_STORE_PREVIOUS_TRANSFORMS

namespace frm {

typedef char* StringT;
typedef const char* CStringT;
typedef float TimeT;
typedef sml::vec2i ivec2;
typedef sml::vec2f vec2;
typedef sml::vec3f vec3;
typedef sml::vec4f vec4;
typedef sml::quatf quat;
typedef sml::mat3f mat3;
typedef sml::mat4f mat4;

class AppBase;
	class App3d;
		class App3dDeferred;

class Clock;

class Profiler;

class multibuf;

class Frustum;
class Plane;

class Object;

template <class C>
class IResource;

class Buffer;

class Framebuffer;

class ITexture;
	class Texture1d;
	class Texture2d;
	class Texture3d;
	class TextureCube;

class Shader;
class ShaderUnit;

class Mesh;
class Model;

class Node;
class INodeModifier;
	class Camera;
	class FirstPersonCtrl;
	class LookAtCtrl;
	class PositionRotationScaleCtrl;
	template <template <typename> class> class PositionTargetCtrl;
	template <template <typename> class> class RotationTargetCtrl;
	class SpinCtrl;

class IMaterial;
	class EnvmapMaterial;
	class LightDeferredIbl;
	class GenericMaterialIbl;
		class GenericMaterialDeferred;

class IGuiComponent;
	class GuiCheckbox;
	class GuiListContainer;
	class GuiSlider;
class Font;

extern Font *fontDefault;
extern Model *modelSAQuad;
extern Texture2d *texBlack, *texWhite, *texNorm;

/*	Initialize framework-global resources (default font, etc.). */
bool init();
/*	Release any resources allocated by init(). */
void cleanup();

} // namespace frm

#endif // FRAMEWORK_COMMON_H_

