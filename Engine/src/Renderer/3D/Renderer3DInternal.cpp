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
#include "Platform/OpenGL/Shaders/Basic.frag"
#include "Platform/OpenGL/Shaders/StudioLighting.frag"
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

		m_LitShader = std::unique_ptr<Shader>(Shader::Create(basicMeshVert, basicMeshFrag));
		m_StudioShader = std::unique_ptr<Shader>(Shader::Create(basicMeshVert, studioLightingFrag));
		m_UnlitShader = std::unique_ptr<Shader>(Shader::Create(basicMeshVert, basicFrag));

		m_LightSource = std::make_unique<LightSource>();

		m_ModelMatrixCalculationMesh = std::make_unique<Mesh>();
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
		auto [it, success] = m_Meshes.emplace(std::make_unique<Mesh>(std::move(mesh)));
		Mesh* meshPtr = it->get();
		RenderMesh* renderMesh = meshPtr->GetRenderMesh();
		if (!meshPtr->IsStatic()) {
			renderMesh->Upload();
			return meshPtr;
		}

		uint32_t stride = m_VertexArray->GetBufferLayout().GetStride();
		uint32_t count = renderMesh->GetVertexCount();
		uint32_t size = count * stride;

		renderMesh->vertexByteOffset = m_LastVertexByteOffset;
		m_LastVertexByteOffset += size;

		renderMesh->vertexOffset = m_LastVertexVertexOffset;
		m_LastVertexVertexOffset += count;
		m_VertexBuffer->SetData(renderMesh->vertexByteOffset, size, renderMesh->vertices.data());
		renderMesh->initialVertexSize = size;

		size = renderMesh->GetIndexByteSize();
		renderMesh->indexOffset = m_LastIndexOffset;
		m_LastIndexOffset += size;
		m_IndexBuffer->SetData(renderMesh->indexOffset, size, renderMesh->indices.data());
		renderMesh->initialIndexSize = size;

		return meshPtr;
	}



	void Renderer3DInternal::OnUpdate()
	{
		#ifdef FT_OPENGL_RENDERER

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);

		m_LitShader->Bind();
		m_LitShader->SetUniformMatrix4fv("uViewProjection", m_Camera->GetViewProjection());
		m_LitShader->SetUniform3f("uLightPosition", m_LightSource->position);
		m_LitShader->SetUniform3f("uLightColor", m_LightSource->color);
		m_LitShader->SetUniform1f("uLightIntensity", m_LightSource->intensity);
		m_LitShader->SetUniform1f("uAmbientIntensity", m_LightSource->ambientIntensity);
		m_LitShader->SetUniform3f("uViewPosition", m_Camera->GetPosition());

		m_StudioShader->Bind();
		m_StudioShader->SetUniformMatrix4fv("uViewProjection", m_Camera->GetViewProjection());
		m_StudioShader->SetUniform3f("uViewPosition", m_Camera->GetPosition());
		
		m_UnlitShader->Bind();
		m_UnlitShader->SetUniformMatrix4fv("uViewProjection", m_Camera->GetViewProjection());

		m_VertexArray->Bind();
		m_VertexBuffer->Bind();
		m_IndexBuffer->Bind();
		bool staticBuffersBound = true;

		for (auto& mesh : m_Meshes)
		{
			if (!mesh->ShouldRender())
				continue;

			if (!mesh->IsStatic()) {
				mesh->GetRenderMesh()->Bind();
				staticBuffersBound = false;
			}
			else if (!staticBuffersBound) {
				m_VertexArray->Bind();
				m_VertexBuffer->Bind();
				m_IndexBuffer->Bind();
			}

			if (mesh->outlined) {
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);
			}

			Shader* shader;
			if (mesh->GetRenderMode() == RenderMode::Overlay || mesh->GetRenderMode() == RenderMode::Wireframe)
				shader = m_UnlitShader.get();
			else if (m_UseStudioLighting)
				shader = m_StudioShader.get();
			else
				shader = m_LitShader.get();

			shader->Bind();
			shader->SetUniform4f("uColor", mesh->color);
			shader->SetUniformMatrix4fv("uModel", mesh->modelMatrix);

			GLenum mode;
			mode = GL_TRIANGLES;
			if (mesh->GetRenderMode() == RenderMode::Wireframe)
				mode = GL_LINES;

			if (mesh->GetRenderMode() == RenderMode::Overlay) {
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(-1.0f, -1.0f);
			}
			else
				glDisable(GL_POLYGON_OFFSET_FILL);


			RenderMesh* renderMesh = mesh->GetRenderMesh();
			if (mesh->IsDirty())
			{
				mesh->CalculateModelMatrix();
				mesh->BakeToRenderMesh();
				if (mesh->IsStatic()) {
					uint32_t vertexSize = renderMesh->GetVertexCount() * m_VertexArray->GetBufferLayout().GetStride();
					if (vertexSize > renderMesh->initialVertexSize) {
						FT_ENGINE_ERROR("Static mesh {} vertex data overflow. Initial size: {} bytes, new size: {} bytes.", mesh->GetID(), renderMesh->initialVertexSize, vertexSize);
						return;
					}
					uint32_t indexSize = renderMesh->GetIndexByteSize();
					if (indexSize > renderMesh->initialIndexSize) {
						FT_ENGINE_ERROR("Static mesh {} index data overflow. Initial size: {} bytes, new size: {} bytes.", mesh->GetID(), renderMesh->initialIndexSize, indexSize);
						return;
					}
					m_VertexBuffer->SetData(renderMesh->vertexByteOffset, vertexSize, renderMesh->vertices.data());
					m_IndexBuffer->SetData(renderMesh->indexOffset, indexSize, renderMesh->indices.data());
				} else
					renderMesh->Upload();
				mesh->ResetDirty();
			}

			glDrawElementsBaseVertex(mode, renderMesh->indices.size(), GL_UNSIGNED_INT, (void*)(intptr_t)renderMesh->indexOffset, renderMesh->vertexOffset);

			if (mesh->outlined) {
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);
				glDisable(GL_DEPTH_TEST);
				m_ModelMatrixCalculationMesh->transform = mesh->transform;
				m_ModelMatrixCalculationMesh->transform.scale *= 1.025f;
				m_ModelMatrixCalculationMesh->CalculateModelMatrix();
				m_UnlitShader->Bind();
				m_UnlitShader->SetUniformMatrix4fv("uModel", m_ModelMatrixCalculationMesh->modelMatrix);
				m_UnlitShader->SetUniform4f("uColor", mesh->outlineColor);
				glDrawElementsBaseVertex(mode, renderMesh->indices.size(), GL_UNSIGNED_INT, (void*)(intptr_t)renderMesh->indexOffset, renderMesh->vertexOffset);
				glStencilMask(0xFF);
				glStencilFunc(GL_ALWAYS, 0, 0xFF);
				glEnable(GL_DEPTH_TEST);
			}
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
		auto it = std::find_if(m_Meshes.begin(), m_Meshes.end(), [meshID](const auto& meshPtr) { return meshPtr->GetID() == meshID; });
		if (it != m_Meshes.end())
			m_Meshes.erase(it);
	}
}