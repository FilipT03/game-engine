#include "pch.h"
#include "Renderer3DInternal.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/RendererCommon.h"
#include "Core/Application.h"
#include "Math/Vector.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef FT_OPENGL_RENDERER
#include <glad/glad.h>

#define FT_GLSL_INCLUDE
#include "Platform/OpenGL/Shaders/BasicMesh.vert"
#include "Platform/OpenGL/Shaders/BasicMesh.frag"
#endif

namespace ft {
	Renderer3DInternal::Renderer3DInternal() {};
	Renderer3DInternal::~Renderer3DInternal() {};

	void Renderer3DInternal::Init()
	{
#ifdef FT_OPENGL_RENDERER

		m_Camera = std::make_unique<WorldCamera3D>();
		auto props = Application::Get().GetWindow().GetWindowProps();
		m_Camera->CalculateProjectionMatrix(props.width, props.height);
		m_Camera->RecalculateView();

		auto layout = BufferLayout({
			{ LayoutElementType::Float3, "inPosition" },
			{ LayoutElementType::Float3, "inNormal" }
			});

		m_VertexArray = std::unique_ptr<VertexArray>(VertexArray::Create(layout));

		m_VertexBuffer = std::shared_ptr<VertexBuffer>(VertexBuffer::Create(100000));
		m_IndexBuffer = std::shared_ptr<IndexBuffer>(IndexBuffer::Create(100000));

		m_VertexArray->SetVertexBuffer(m_VertexBuffer);
		m_VertexArray->SetIndexBuffer(m_IndexBuffer);

		m_BasicShader = std::unique_ptr<Shader>(Shader::Create(basicMeshVert, basicMeshFrag));

		m_LightSource = std::make_unique<LightSource>();
		m_LightSource->position = glm::vec3(2.0f, 2.0f, 0.0f);
		m_LightSource->color = glm::vec3(1.0f);
		m_LightSource->intensity = 1.0f;

		WorldCamera3D* camera = dynamic_cast<WorldCamera3D*>(m_Camera.get());
		camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
		camera->SetFront(glm::vec3(0.0f, 0.0f, -1.0f));
		//camera->SetFront(glm::vec3(0.0f, -0.5f, -1.0f));
		//Mesh mesh = Mesh::CreateCube(Transform3D(glm::vec3(0, -2, 0), Vector::One * 2.0f));
		//Mesh mesh = Mesh::CreateSphere(Transform3D(glm::vec3(0, -2, -2), Vector::One * 4.0f), 160, 180);
		Mesh mesh = Mesh::CreateCylinder(Transform3D(glm::vec3(0, -3, -2), Vector::One * 4.0f, Vector::Forward * 30.0f), 30);
		//Mesh mesh = Mesh::CreateCone(Transform3D(glm::vec3(0, -3, -2), Vector::One * 4.0f, Vector::Forward * 30.0f), 30);
		mesh.color = glm::vec4(0.2f, 0.8f, 0.8f, 1.0f);
		AddMesh(std::move(mesh));
#endif
	}

	void Renderer3DInternal::Shutdown()
	{
		#ifdef FT_OPENGL_RENDERER
		#endif
	}

	Mesh* Renderer3DInternal::AddMesh(Mesh&& mesh)
	{
		mesh.SetID(m_LastMeshId++);
		auto [it, success] = m_Meshes.emplace(mesh.GetID(), std::make_unique<Mesh>(std::move(mesh)));
		Mesh* meshPtr = it->second.get();
		RenderMesh* renderMesh = meshPtr->GetRenderMesh();
		

		uint32_t stride = m_VertexArray->GetBufferLayout().GetStride();
		uint32_t count = renderMesh->GetVertexCount();
		uint32_t size = count * stride;

		renderMesh->vertexByteOffset = m_LastVertexByteOffset;
		m_LastVertexByteOffset += size;

		renderMesh->vertexOffset = m_LastVertexVertexOffset;
		m_LastVertexVertexOffset += count;

		//std::vector<uint8_t> data;
		//RendererCommon::PackInterleaved(data, count, { renderMesh->vertices.data(), renderMesh->normals.data() }, m_VertexArray->GetBufferLayout());


		//if (data.size() != size) {
		//	FT_ENGINE_ERROR("Data packing mismatch. Expected {} bytes, got {}", size, data.size());
		//}
		m_VertexBuffer->SetData(renderMesh->vertexByteOffset, size, renderMesh->vertices.data());

		size = renderMesh->GetIndexByteSize();
		renderMesh->indexOffset = m_LastIndexOffset;
		m_LastIndexOffset += size;
		m_IndexBuffer->SetData(renderMesh->indexOffset, size, renderMesh->indices.data());

		return meshPtr;
	}



	void Renderer3DInternal::OnUpdate()
	{
		#ifdef FT_OPENGL_RENDERER

		glEnable(GL_DEPTH_TEST);

		m_BasicShader->Bind();
		m_BasicShader->SetUniformMatrix4fv("uViewProjection", m_Camera->GetViewProjection());
		m_BasicShader->SetUniform3f("uLightPosition", m_LightSource->position);
		m_BasicShader->SetUniform3f("uLightColor", m_LightSource->color);
		m_BasicShader->SetUniform1f("uLightIntensity", m_LightSource->intensity);
		m_BasicShader->SetUniform3f("uViewPosition", m_Camera->GetPosition());


		m_VertexArray->Bind();
		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind();

		for (auto& [id, mesh] : m_Meshes)
		{
			GLenum mode;

			mesh->transform.rotation.y += 1.0f;
			mesh->CalculateModelMatrix();

			m_BasicShader->Bind();
			m_BasicShader->SetUniform4f("uColor", mesh->color);
			m_BasicShader->SetUniformMatrix4fv("uModel", mesh->modelMatrix);

			mode = GL_TRIANGLES;

			RenderMesh* renderMesh = mesh->GetRenderMesh();
			if (mesh->IsDirty())
			{
				mesh->BakeToRenderMesh();
				m_VertexBuffer->SetData(renderMesh->vertexByteOffset, renderMesh->GetVertexCount() * m_VertexArray->GetBufferLayout().GetStride(), renderMesh->vertices.data());
				mesh->ResetDirty();
			}

			glDrawElementsBaseVertex(mode, renderMesh->indices.size(), GL_UNSIGNED_INT, (void*)(intptr_t)renderMesh->indexOffset, renderMesh->vertexOffset);
		}
		#endif
	}


	bool Renderer3DInternal::OnEvent(const Event& event)
	{
		if (event.type == EventType::WindowResize)
		{
			glm::vec2 size = Application::Get().GetWindow().GetFrameBufferSize();

			#ifdef FT_OPENGL_RENDERER
			glViewport(0, 0, size.x, size.y);
			#endif

			m_Camera->CalculateProjectionMatrix(size.x, size.y);
		}
		return false;
	}

	void Renderer3DInternal::RemoveMesh(uint32_t meshID)
	{
		m_Meshes.erase(meshID);
	}
}