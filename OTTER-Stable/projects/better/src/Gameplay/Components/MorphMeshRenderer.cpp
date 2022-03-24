#include "Gameplay/Components/MorphMeshRenderer.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Utils/GlmBulletConversions.h"
#include "Gameplay/Components/IComponent.h"
#include <GLFW/glfw3.h>
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/ImGuiHelper.h"

MorphMeshRenderer::MorphMeshRenderer() :
	IComponent()
{ }

void MorphMeshRenderer::SetMorphMeshRenderer(Gameplay::MeshResource::Sptr baseMesh, Gameplay::Material::Sptr mat)
{
	m_mat = mat;
	m_vao = baseMesh->Mesh;

	UpdateData(baseMesh, baseMesh, 0.0f);
}

MorphMeshRenderer::~MorphMeshRenderer() = default;


void MorphMeshRenderer::UpdateData(Gameplay::MeshResource::Sptr frame0, Gameplay::MeshResource::Sptr frame1, float t)
{
	m_vao = frame0->Mesh;
	
	VertexBuffer::Sptr vbo = VertexBuffer::Create();
	VertexBuffer::Sptr vbo2 = VertexBuffer::Create();
	
	vbo = frame1->Mesh->GetBufferBinding(AttribUsage::Position)->GetBuffer();
	vbo2 = frame1->Mesh->GetBufferBinding(AttribUsage::Normal)->GetBuffer();
	VertexArrayObject::VertexDeclaration newvd = frame1->Mesh->GetVDecl();
	//std::cout << newvd[0].Usage << std::endl; //position
	//std::cout << newvd[1].Usage << std::endl; //color
	//std::cout << newvd[2].Usage << std::endl; //normal
	//std::cout << newvd[3].Usage << std::endl; //texture
	//std::cout << newvd.size() << std::endl; =4
	BufferAttribute ba1 = BufferAttribute(6, 3, AttributeType::Float, newvd[0].Stride, newvd[0].Offset, AttribUsage::Position);
	BufferAttribute ba2 = BufferAttribute(7, 3, AttributeType::Float, newvd[2].Stride, newvd[2].Offset, AttribUsage::Normal);
	m_vao->AddVertexBuffer(vbo, std::vector<BufferAttribute>{ba1});
	m_vao->AddVertexBuffer(vbo2, std::vector<BufferAttribute>{ba2});
	
	GetGameObject()->Get<RenderComponent>()->GetMeshResource()->Mesh = m_vao;
	//GetGameObject()->Get<RenderComponent>()->GetMesh()->AddVertexBuffer(vbo, std::vector<BufferAttribute>{ba1});
	//GetGameObject()->Get<RenderComponent>()->GetMesh()->AddVertexBuffer(vbo2, std::vector<BufferAttribute>{ba2});
	
	m_t = t;
	GetGameObject()->Get<RenderComponent>()->GetMaterial()->Set("t", m_t);
}

void MorphMeshRenderer::Draw()
{
	m_mat->Apply();
	//auto& transform = GetGameObject()->GetTransform();

	//m_mat->Set()
	//do we need to re-set shader stuff? idk

	//set new vao??
	//GetGameObject()->Get<RenderComponent>()->GetMeshResource()->Mesh = m_vao;
	//m_vao->Draw();

	//change in time
	//GetGameObject()->Get<RenderComponent>()->GetMaterial()->Set("t", m_t);
}

void MorphMeshRenderer::Update(float deltaTime)
{
}

void MorphMeshRenderer::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
}

void MorphMeshRenderer::OnTriggerVolumeLeaving(const std::shared_ptr<Gameplay::Physics::RigidBody>& body)
{
}

void MorphMeshRenderer::Awake()
{
}

void MorphMeshRenderer::RenderImGui()
{
}

nlohmann::json MorphMeshRenderer::ToJson() const {

	//return {
	//	{ "trash_collected", trash != nullptr ? trash->GUID.str() : "null" }
	//	//{ "exit_material", ExitMaterial != nullptr ? ExitMaterial->GetGUID().str() : "null" }
	//};
	return {};
}

MorphMeshRenderer::Sptr MorphMeshRenderer::FromJson(const nlohmann::json & blob) {
	MorphMeshRenderer::Sptr result = std::make_shared<MorphMeshRenderer>();
	////result->trash = ResourceManager::Get<GameObject::Sptr>(Guid(blob["trash_collected"]));
	//
	//result->trash = (GameObject::FromJson(Guid(blob["trash_collected"]), result->GetGameObject()->GetScene()));
	return result;
}
