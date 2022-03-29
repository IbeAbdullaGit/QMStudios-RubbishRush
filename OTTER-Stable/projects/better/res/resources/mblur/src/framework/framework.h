#ifndef FRAMEWORK_H_
#define FRAMEWORK_H_

#include "AppBase.h"
	#include "App3d.h"
		#include "App3dDeferred.h"

#include "Clock.h"

#include "Profiler.h"

#include "multibuf.h"

#include "Frustum.h"
#include "Plane.h"

#include "Object.h"

#include "IResource.h"

#include "Buffer.h"

#include "Framebuffer.h"

#include "ITexture.h"
	#include "Texture1d.h"
	#include "Texture2d.h"
	#include "Texture3d.h"
	#include "TextureCube.h"

#include "Shader.h"
#include "ShaderUnit.h"

#include "Mesh.h"
#include "Model.h"

#include "Node.h"
#include "INodeModifier.h"
	#include "Camera.h"
	#include "FirstPersonCtrl.h"
	#include "LookAtCtrl.h"
	#include "PositionRotationScaleCtrl.h"
	#include "PositionTargetCtrl.h"
	#include "RotationTargetCtrl.h"
	#include "SpinCtrl.h"


#include "IMaterial.h"
	#include "EnvmapMaterial.h"
	#include "LightDeferredIbl.h"
	#include "GenericMaterialIbl.h"
		#include "GenericMaterialDeferred.h"

#include "IGuiComponent.h"
	#include "GuiCheckbox.h"
	#include "GuiListContainer.h"
	#include "GuiProfileGant.h"
	#include "GuiProfilePlotter.h"
	#include "GuiSlider.h"
#include "Font.h"

#endif
