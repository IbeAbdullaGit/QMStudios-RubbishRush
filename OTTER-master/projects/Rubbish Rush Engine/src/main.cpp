#include "NOU/App.h"
#include "NOU/Input.h"
#include "NOU/Entity.h"
#include "NOU/CCamera.h"
#include "NOU/CMeshRenderer.h"
#include "CPathAnimator.h"
#include "Tools/PathUtility.h"
#include "NOU/Shader.h"
#include "NOU/GLTFLoader.h"

#include "imgui.h"
#include <iostream>

#include <memory>

using namespace nou;


std::unique_ptr<ShaderProgram> prog_texLit, prog_lit, prog_unlit;
std::unique_ptr<Mesh> boxMesh, recMesh, trashyMesh, bgMesh;
std::unique_ptr<Texture2D> TrashyTex, recTex, bgTex;
std::unique_ptr<Material> unselectedMat, selectedMat, lineMat, recMat, trashyMat, bgMat;



float timeLoop = 6.f;


int main()
{

	bool isPressed = false;
	bool listPanel = true;

	bool timerDone = false;

	App::Init("Rubbish Rush", 1920, 1080);
	App::SetClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); // Reminder: SetClearColor uses RGB values of 0-1 instead of 0-255

	App::Tick();

	App::InitImgui();

	LoadDefaultResources();


	// Create and set up camera
	Entity camEntity = Entity::Create();
	auto& cam = camEntity.Add<CCamera>(camEntity);
	cam.Perspective(60.0f, 1.0f, 0.1f, 100.0f);
	camEntity.transform.m_pos = glm::vec3(0.0f, 0.0f, 4.0f);

	Entity RectangleE = Entity::Create();
	RectangleE.Add<CMeshRenderer>(RectangleE, *recMesh, *recMat);
	RectangleE.Add<CPathAnimator>(RectangleE);
	RectangleE.transform.m_pos = glm::vec3(0.0f, 2.0f, 0.0f);
	RectangleE.transform.m_rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	RectangleE.transform.m_scale = glm::vec3(5.0f, 0.2f, 4.0f);

	Entity TrashyE = Entity::Create();
	TrashyE.Add<CMeshRenderer>(TrashyE, *trashyMesh, *trashyMat);
	TrashyE.Add<CPathAnimator>(TrashyE);
	TrashyE.transform.m_pos = glm::vec3(-3.0f, 0.0f, 10.f);
	TrashyE.transform.m_rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	TrashyE.transform.m_scale = glm::vec3(1.f, 1.f, 1.f);

	Entity bgE = Entity::Create();
	bgE.Add<CMeshRenderer>(bgE, *bgMesh, *bgMat);
	bgE.transform.m_rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	bgE.transform.m_pos = glm::vec3(0.f, 0.f, -1.f);
	bgE.transform.m_scale = glm::vec3(5.8f, 5.8f, 5.8f);

	//Setting up our container of waypoints.
	std::vector<std::unique_ptr<Entity>> points;
	points.push_back(Entity::Allocate());
	points.back()->Add<CMeshRenderer>(*points.back(), *boxMesh, *unselectedMat);
	points.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	points.back()->transform.m_pos = glm::vec3(0.0f, 4.1f, 0.0f);

	points.push_back(Entity::Allocate());
	points.back()->Add<CMeshRenderer>(*points.back(), *boxMesh, *unselectedMat);
	points.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	points.back()->transform.m_pos = glm::vec3(0.0f, -0.05f, 0.0f);

	std::vector<std::unique_ptr<Entity>> points2;
	//POINT 1
	points2.push_back(Entity::Allocate());
	points2.back()->Add<CMeshRenderer>(*points2.back(), *boxMesh, *unselectedMat);
	points2.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	points2.back()->transform.m_pos = glm::vec3(-3.0f, 0.0f, 1.25f);
	//POINT 2
	points2.push_back(Entity::Allocate());
	points2.back()->Add<CMeshRenderer>(*points2.back(), *boxMesh, *unselectedMat);
	points2.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	points2.back()->transform.m_pos = glm::vec3(-2.0f, 0.0f, 1.25f);
	//POINT 3
	points2.push_back(Entity::Allocate());
	points2.back()->Add<CMeshRenderer>(*points2.back(), *boxMesh, *unselectedMat);
	points2.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	points2.back()->transform.m_pos = glm::vec3(-1.5f, 0.0f, 1.25f);
	//POINT 4
	points2.push_back(Entity::Allocate());
	points2.back()->Add<CMeshRenderer>(*points2.back(), *boxMesh, *unselectedMat);
	points2.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	points2.back()->transform.m_pos = glm::vec3(-1.0f, 0.0f, 1.25f);
	//POINT 5
	points2.push_back(Entity::Allocate());
	points2.back()->Add<CMeshRenderer>(*points2.back(), *boxMesh, *unselectedMat);
	points2.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	points2.back()->transform.m_pos = glm::vec3(-0.6, 0.0f, 1.25f);

	//points for scaling
	std::vector<std::unique_ptr<Entity>> pointsS;
	pointsS.push_back(Entity::Allocate());
	pointsS.back()->Add<CMeshRenderer>(*pointsS.back(), *boxMesh, *unselectedMat);
	pointsS.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	pointsS.back()->transform.m_pos = glm::vec3(0.8f, 0.8f, 1.0f); //SCALING VARIABLE
	//pointsS.back()->transform.m_rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, -0.3f, 0.0f));
	pointsS.back()->transform.m_rotation = glm::vec3(glm::radians(90.0f), 0.0, glm::radians(30.f));

	pointsS.push_back(Entity::Allocate());
	pointsS.back()->Add<CMeshRenderer>(*pointsS.back(), *boxMesh, *unselectedMat);
	pointsS.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	pointsS.back()->transform.m_pos = glm::vec3(1.5f, 2.8f, 1.0f); //SCALING VARIABLE
	//pointsS.back()->transform.m_rotation = glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.3f, 0.0f));
	pointsS.back()->transform.m_rotation = glm::vec3(glm::radians(90.0f), 0.0, glm::radians(-20.f));

	std::vector<std::unique_ptr<Entity>> pointsS2;
	pointsS2.push_back(Entity::Allocate());
	pointsS2.back()->Add<CMeshRenderer>(*pointsS.back(), *boxMesh, *unselectedMat);
	pointsS2.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	pointsS2.back()->transform.m_pos = glm::vec3(1.5f, 1.5f, 1.0f); //SCALING VARIABLE
	pointsS2.back()->transform.m_rotation = glm::vec3(glm::radians(90.0f), 0.0f, 0.0f);
	pointsS2.push_back(Entity::Allocate());
	pointsS2.back()->Add<CMeshRenderer>(*pointsS.back(), *boxMesh, *unselectedMat);
	pointsS2.back()->transform.m_scale = glm::vec3(0.0f, 0.0f, 0.0f);
	pointsS2.back()->transform.m_pos = glm::vec3(0.5f, 0.2f, 1.0f); //SCALING VARIABLE
	pointsS2.back()->transform.m_rotation = glm::vec3(glm::radians(90.0f), 0.0f, 0.0f);

	PathSampler sampler = PathSampler();

	Entity pathDrawUtility = Entity::Create();
	pathDrawUtility.Add<CLineRenderer>(pathDrawUtility, sampler, *lineMat);

	// Update loop
	while (!App::IsClosing() && !Input::GetKeyDown(GLFW_KEY_ESCAPE))
	{
		//Start of the frame.
		App::FrameStart();
		float deltaTime = App::GetDeltaTime();


		//Updates the camera.
		camEntity.Get<CCamera>().Update();

		bgE.Get<CMeshRenderer>().Draw();
		bgE.transform.RecomputeGlobal();


		if (isPressed && !timerDone)

		{//Update our path animator.

			if (timeLoop > 0) {
				timerDone = false;
				timeLoop -= deltaTime;
			}
			else {
				timerDone = true;
			}



			if (RectangleE.transform.m_pos.y > 0.0f)
			{
				RectangleE.Get<CPathAnimator>().Update(points, deltaTime);
			}
			else
			{
				//idk

				TrashyE.transform.RecomputeGlobal();
				TrashyE.Get<CMeshRenderer>().Draw();

				if (TrashyE.transform.m_pos.x <= -0.5f) {
					TrashyE.Get<CPathAnimator>().UpdateCAT(points2, deltaTime);
				}
				else {
					//std::cout << "debug hit";
					TrashyE.Get<CPathAnimator>().UpdateScale(pointsS, deltaTime);

				}


			}
			//Update transformation matrices.
			for (size_t i = 0; i < points.size(); ++i)
			{
				points[i]->transform.RecomputeGlobal();
			}


			RectangleE.transform.RecomputeGlobal();

			//Draw everything.
			for (size_t i = 0; i < points.size(); ++i)
			{
				points[i]->Get<CMeshRenderer>().Draw();
			}


			RectangleE.Get<CMeshRenderer>().Draw();

			//Draw our path (for debugging/demo purposes).
			pathDrawUtility.Get<CLineRenderer>().Draw(points);
		}


		if (isPressed && timerDone)

		{//Update our path animator.

				//idk

			TrashyE.transform.RecomputeGlobal();


			if (TrashyE.transform.m_pos.x > -2.4) {
				TrashyE.Get<CPathAnimator>().UpdateCAT(points2, deltaTime);
				//TrashyE.transform.m_scale = glm::vec3(1.f, 1.f, 1.f);

				TrashyE.Get<CPathAnimator>().UpdateScale(pointsS2, deltaTime);
			}
			else if (TrashyE.transform.m_pos.x > -2.4) {
				//std::cout << "debug hit";
				//reconfig scale
				//TrashyE.transform.m_scale = glm::vec3(1.f, 1.f, 1.f);
				//TrashyE.transform.m_rotation = glm::vec3(glm::radians(90.0f), 0.0f, 0.0f);
				//TrashyE.Get<CPathAnimator>().UpdateScale(pointsS2, deltaTime);

			}
			else {

				if (RectangleE.transform.m_pos.y < 3.0f)
				{
					RectangleE.Get<CPathAnimator>().Update(points, deltaTime);
				}
			}
			TrashyE.Get<CMeshRenderer>().Draw();

			//Update transformation matrices.
			for (size_t i = 0; i < points.size(); ++i)
			{
				points[i]->transform.RecomputeGlobal();
			}


			RectangleE.transform.RecomputeGlobal();

			//Draw everything.
			for (size_t i = 0; i < points.size(); ++i)
			{
				points[i]->Get<CMeshRenderer>().Draw();
			}


			RectangleE.Get<CMeshRenderer>().Draw();

			//Draw our path (for debugging/demo purposes).
			pathDrawUtility.Get<CLineRenderer>().Draw(points);

			//if (timeLoopA > 0) {
			//	timeLoopA -= deltaTime;

			//}
			//else {
				//timeLoop = 12.f;
				//timeLoopA = 5.f;
			//}
		}



		//For Imgui...
		if (listPanel)
		{
			App::StartImgui();


			ImGui::Begin("Rubbish Rush", &listPanel, ImVec2(300, 200));
			//ImGui::

			if (!isPressed)
			{//Add a new waypoint!
				if (ImGui::Button("Start"))
				{
					isPressed = true;
					listPanel = false;
				}
			}

			ImGui::End();
			App::EndImgui();
		}

		//This sticks all the drawing we just did on the screen.
		App::SwapBuffers();
	}

	App::Cleanup();

	return 0;
}

void LoadDefaultResources()
{
	// Load vertex and fragment shaders
	std::unique_ptr vs_shader = std::make_unique<Shader>("shaders/texturedlit.vert", GL_VERTEX_SHADER);
	std::unique_ptr fs_shader = std::make_unique<Shader>("shaders/texturedlit.frag", GL_FRAGMENT_SHADER);

	std::vector<Shader*> texLit = { vs_shader.get(), fs_shader.get() };
	prog_texLit = std::make_unique<ShaderProgram>(texLit);

	auto v_lit = std::make_unique<Shader>("shaders/lit.vert", GL_VERTEX_SHADER);
	auto f_lit = std::make_unique<Shader>("shaders/lit.frag", GL_FRAGMENT_SHADER);

	std::vector<Shader*> lit = { v_lit.get(), f_lit.get() };
	prog_lit = std::make_unique<ShaderProgram>(lit);

	auto v_unlit = std::make_unique<Shader>("shaders/unlit.vert", GL_VERTEX_SHADER);
	auto f_unlit = std::make_unique<Shader>("shaders/unlit.frag", GL_FRAGMENT_SHADER);

	std::vector<Shader*> unlit = { v_unlit.get(), f_unlit.get() };
	prog_unlit = std::make_unique<ShaderProgram>(unlit);

	//Set up Box
	boxMesh = std::make_unique<Mesh>();
	GLTF::LoadMesh("box/Box.gltf", *boxMesh);

	//Set up Rec
	recMesh = std::make_unique<Mesh>();
	GLTF::LoadMesh("Rectangle/Rec1.gltf", *recMesh);
	recTex = std::make_unique<Texture2D>("Rectangle/Rec1.png");

	recMat = std::make_unique<Material>(*prog_texLit);
	recMat->AddTexture("albedo", *recTex);

	bgMesh = std::make_unique<Mesh>();
	GLTF::LoadMesh("backg/menuBackg.gltf", *bgMesh);
	bgTex = std::make_unique<Texture2D>("backg/menubg.png");

	bgMat = std::make_unique<Material>(*prog_texLit);
	bgMat->AddTexture("albedo", *bgTex);

	trashyMesh = std::make_unique<Mesh>();
	GLTF::LoadMesh("trashy/Trashy.gltf", *trashyMesh);
	TrashyTex = std::make_unique<Texture2D>("trashy/Trashy2.png");

	trashyMat = std::make_unique<Material>(*prog_texLit);

	trashyMat->AddTexture("albedo", *TrashyTex);

	unselectedMat = std::make_unique<Material>(*prog_lit);
	unselectedMat->m_color = glm::vec3(0.5f, 0.5f, 0.5f);

	selectedMat = std::make_unique<Material>(*prog_lit);
	selectedMat->m_color = glm::vec3(1.0f, 0.0f, 0.0f);

	lineMat = std::make_unique<Material>(*prog_unlit);
	lineMat->m_color = glm::vec3(1.0f, 1.0f, 1.0f);
}