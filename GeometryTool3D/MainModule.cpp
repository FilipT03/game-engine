#include "MainModule.h"
#include<Resources/MeshImporter.h>
#include<optional>
#include<portable-file-dialogs.h>

void MainModule::OnRegister()
{
	FT_TRACE("MainModule registered");

	ft::Renderer3D::SetClearColor(0.1, 0.1, 0.1, 0.1);

	SetDefaultLight();

	ft::WorldCamera3D* camera = ft::Renderer3D::GetCamera();
	camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
	camera->SetFront(glm::vec3(0.0f, 0.0f, -1.0f));
	//camera->SetFront(glm::vec3(0.0f, -0.5f, -1.0f));
	//Mesh mesh = Mesh::CreateCube(Transform3D(glm::vec3(0, -2, 0), Vector::One * 2.0f));
	//Mesh mesh = Mesh::CreateSphere(Transform3D(glm::vec3(0, -2, -2), Vector::One * 4.0f), 160, 180);
	//Mesh mesh = Mesh::CreateCone(Transform3D(glm::vec3(0, -3, -2), Vector::One * 4.0f, Vector::Forward * 30.0f), 30);
	//test_mesh = ft::Renderer3D::AddMesh(
	//	ft::Mesh::CreateCone(
	//		ft::Transform3D(glm::vec3(0, -3, -2),
	//			ft::Vector::One * 3.0f,
	//			ft::Vector::Forward * 30.0f),
	//		glm::vec4(0.2f, 0.8f, 0.8f, 1.0f), 50));
	ft::Mesh* newMesh = ft::Renderer3D::AddMesh(
		ft::Mesh::CreateSphere(
							ft::Transform3D(glm::vec3(0, -3, -2), 
							ft::Vector::One * 3.0f, 
							ft::Vector::Forward * 30.0f), 
			glm::vec4(0.2f, 0.8f, 0.8f, 1.0f), 50, 50));

	//ft::Mesh* newMesh = ft::Renderer3D::AddMesh(
	//					ft::Mesh::CreateCylinder(
	//						ft::Transform3D(glm::vec3(0, -3, -2), 
	//						ft::Vector::One * 3.0f, 
	//						ft::Vector::Forward * 30.0f), 
	//						glm::vec4(0.2f, 0.8f, 0.8f, 1.0f), 30));
	m_Meshes.emplace(newMesh->GetID(), newMesh);

	ft::Renderer3D::AddMesh(ft::Mesh::CreatePlane(ft::Transform3D(glm::vec3(0, -5.5, 0), ft::Vector::One * 10.0f), glm::vec4(1), true));
	m_Selection_mesh = ft::Renderer3D::AddMesh(ft::Mesh(ft::Transform3D(), glm::vec4(1.0f, 0.7f, 0.157f, 0.8f), false));
	m_Selection_mesh->SetRenderMode(ft::RenderMode::Overlay);
}

void MainModule::OnUpdate()
{
	for (auto& [id, mesh] : m_Meshes) {
		mesh->transform.rotation.y += 90.0f * ft::Time::DeltaTime();
		mesh->CalculateModelMatrix();
	}
	if (m_SelectedMesh != 0) {
		m_Selection_mesh->transform = m_Meshes[m_SelectedMesh]->transform;
		m_Selection_mesh->CalculateModelMatrix();
	}
}

bool MainModule::OnKeyEvent(const ft::KeyEvent& event)
{
	if (event.type == ft::EventType::KeyPress) {
		if (event.key == GLFW_KEY_ESCAPE)
			ft::Application::Get().Close();
		if (event.key == GLFW_KEY_F1) {
			if (m_Perspective)
				ft::Renderer3D::GetCamera()->SetProjectionMode(ft::ProjectionMode::Orthographic);
			else
				ft::Renderer3D::GetCamera()->SetProjectionMode(ft::ProjectionMode::Perspective);
			m_Perspective = !m_Perspective;
		}
		if (event.key == GLFW_KEY_F2 && m_SelectedMesh != 0) {
			if (m_Solid)
				m_Meshes[m_SelectedMesh]->SetRenderMode(ft::RenderMode::Wireframe);
			else
				m_Meshes[m_SelectedMesh]->SetRenderMode(ft::RenderMode::Solid);
			m_Solid = !m_Solid;
		}
		if (event.key == GLFW_KEY_F3) {
			auto selection = pfd::open_file("Select a mesh file", ".", { "Wavefront .obj File", "*.obj" }).result();

			std::optional<ft::Mesh> mesh;
			if (selection.empty())
				FT_TRACE("No file selected.");
			else 
				mesh = ft::MeshImporter::ImportMesh(selection[0]);

			if (mesh) {
				mesh->transform.position = ft::Renderer3D::GetCamera()->GetPosition() + ft::Renderer3D::GetCamera()->GetFront() * 5.0f;
				mesh->CalculateModelMatrix();
				ft::Mesh* newMesh = (ft::Renderer3D::AddMesh(std::move(*mesh)));
				m_Meshes.emplace(newMesh->GetID(), newMesh);
			}
		}
		if (event.key == GLFW_KEY_F4) {
			if (m_UsingAmbientOnlyLight)
				SetDefaultLight();
			else
				SetAmbientOnlyLight();
		}
		if (m_SelectedMesh != 0) {
			bool move = false;
			glm::vec3 direction;
			if (event.key == GLFW_KEY_1) {
				direction = ft::Vector::Forward;
				move = true;
			}
			else if (event.key == GLFW_KEY_2) {
				direction = ft::Vector::Left;
				move = true;
	}
			else if (event.key == GLFW_KEY_3) {
				direction = ft::Vector::Down;
				move = true;
			}
			else if (event.key == GLFW_KEY_4) {
				direction = ft::Vector::Back;
				move = true;
			}
			else if (event.key == GLFW_KEY_5) {
				direction = ft::Vector::Right;
				move = true;
			}
			else if (event.key == GLFW_KEY_6) {
				direction = ft::Vector::Up;
				move = true;
			}
			if (move) {
				ft::WorldCamera3D* camera = ft::Renderer3D::GetCamera();
				float depth = glm::length(camera->GetPosition() - m_Meshes[m_SelectedMesh]->transform.position);
				camera->SetFront(direction);
				camera->SetPosition(m_Meshes[m_SelectedMesh]->transform.position - direction * depth);
			}

		}
	}
	return false;
}

bool MainModule::OnMouseEvent(const ft::MouseEvent& event)
{
	if (event.type == ft::EventType::MousePress) {
		ft::MousePressEvent pressEvent = ft::As<ft::MousePressEvent>(event);
		if (pressEvent.button == GLFW_MOUSE_BUTTON_LEFT) {
			ft::WorldCamera3D* camera = ft::Renderer3D::GetCamera();
			glm::vec2 mousePos = ft::Input::GetMousePosition();
			glm::vec3 rayOrigin, rayDir;
			camera->ScreenPointToRay(mousePos, rayOrigin, rayDir);

			int faceIndex = -1;
			uint32_t meshID = 0;
			float closestDistance = std::numeric_limits<float>::max();
			for (auto& [id, mesh] : m_Meshes) {
				ft::MeshRaycastHit result = mesh->GetFaceIndexFromRay(rayOrigin, rayDir);
				float distance = glm::length(rayOrigin - result.hitPoint);
				if (result.hit && distance < closestDistance) {
					faceIndex = result.faceIndex;
					meshID = id;
					closestDistance = distance;
				}
			}
			FT_TRACE("Face index: {}", faceIndex);

			ft::MeshData* data = m_Selection_mesh->GetData();
			data->Clear();
			if (faceIndex != -1) {
				uint32_t vertexStart = 0;
				ft::Mesh* mesh = m_Meshes[meshID];
				for (int f = 0; f < faceIndex; f++)
					vertexStart += mesh->GetData()->polygonSizes[f];
				uint32_t polygonSize = mesh->GetData()->polygonSizes[faceIndex];
				for (uint32_t i = 0; i < polygonSize; i++) {
					uint32_t vertexIndex = mesh->GetData()->indices[vertexStart + i];
					data->positions.push_back(mesh->GetData()->positions[vertexIndex]);
					data->indices.push_back(i);
				}
				data->faceNormals.push_back(mesh->GetData()->faceNormals[faceIndex]);
				data->polygonSizes.push_back(polygonSize);
				m_Selection_mesh->CalculateModelMatrix();
				m_Selection_mesh->BakeToRenderMesh();
				m_SelectedMesh = meshID;
			}
			else
				m_SelectedMesh = 0;
		}
	}
	return false;
}

void MainModule::SetDefaultLight()
{
	ft::LightSource* lightSource = ft::Renderer3D::GetLightSource();
	lightSource->position = glm::vec3(2.0f, 2.0f, 0.0f);
	lightSource->color = glm::vec3(1.0f);
	lightSource->intensity = 1.0f;
	lightSource->ambientIntensity = 0.2f;
	m_UsingAmbientOnlyLight = false;
}

void MainModule::SetAmbientOnlyLight()
{
	ft::LightSource* lightSource = ft::Renderer3D::GetLightSource();
	lightSource->position = glm::vec3(2.0f, 2.0f, 0.0f);
	lightSource->color = glm::vec3(1.0f);
	lightSource->intensity = 0.3f;
	lightSource->ambientIntensity = 0.7f;
	m_UsingAmbientOnlyLight = true;
}
