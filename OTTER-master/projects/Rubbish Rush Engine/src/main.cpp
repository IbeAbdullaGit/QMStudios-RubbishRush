#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "NOU/Shader.h"
#include "NOU/GLTFLoader.h"

#include "Logging.h"

#include <memory>

using namespace nou;

int main() {
	// Initialization
	App::Init("Rubbish Rush", 1920, 1080);
	App::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); // Reminder: SetClearColor uses RGB values of 0-1 instead of 0-255

	App::Tick();

	// Load vertex and fragment shaders
	std::unique_ptr vs_shader = std::make_unique<Shader>("shaders/texturedlit.vert", GL_VERTEX_SHADER);
	std::unique_ptr fs_shader = std::make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);

	// Activate shader program
	ShaderProgram shaderP = ShaderProgram({ vs_shader.get(),fs_shader.get() });


	// Create and set up camera
	Entity ent_camera = Entity::Create();
	CCamera& cam = ent_camera.Add<CCamera>(ent_camera);
	cam.Perspective(60.0f, 1.0, 0.1f, 100.0f);
	ent_camera.transform.m_pos = glm::vec3(0.0f, 0.0f, 4.0f);

	// Update loop
	while (!App::IsClosing() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
	{
		App::FrameStart();

		ent_camera.Get<CCamera>().Update();

		App::SwapBuffers();
	}

	App::Cleanup();

	return 0;
}