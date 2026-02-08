#include "MainModule.h"

void MainModule::OnRegister()
{
	FT_TRACE("MainModule registered");

	ft::Renderer3D::SetClearColor(0.1, 0.1, 0.1, 0.1);

	ft::LightSource *lightSource = ft::Renderer3D::GetLightSource();
	lightSource->position = glm::vec3(2.0f, 2.0f, 0.0f);
	lightSource->color = glm::vec3(1.0f);
	lightSource->intensity = 1.0f;

	ft::WorldCamera3D* camera = ft::Renderer3D::GetCamera();
	camera->SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
	camera->SetFront(glm::vec3(0.0f, 0.0f, -1.0f));
	//camera->SetFront(glm::vec3(0.0f, -0.5f, -1.0f));
	//Mesh mesh = Mesh::CreateCube(Transform3D(glm::vec3(0, -2, 0), Vector::One * 2.0f));
	//Mesh mesh = Mesh::CreateSphere(Transform3D(glm::vec3(0, -2, -2), Vector::One * 4.0f), 160, 180);
	//Mesh mesh = Mesh::CreateCone(Transform3D(glm::vec3(0, -3, -2), Vector::One * 4.0f, Vector::Forward * 30.0f), 30);
	ft::Mesh* mesh = ft::Renderer3D::AddMesh(
						ft::Mesh::CreateCylinder(
							ft::Transform3D(glm::vec3(0, -3, -2), 
							ft::Vector::One * 4.0f, 
							ft::Vector::Forward * 30.0f), 30));
	mesh->color = glm::vec4(0.2f, 0.8f, 0.8f, 1.0f);
	test_mesh = mesh;
}

void MainModule::OnUpdate()
{
	test_mesh->transform.rotation.y += 1.0f;
	test_mesh->CalculateModelMatrix();
}

bool MainModule::OnKeyEvent(const ft::KeyEvent& event)
{
	if (event.type == ft::EventType::KeyPress) {
		if (event.key == GLFW_KEY_ESCAPE)
			ft::Application::Get().Close();
		if (event.key == GLFW_KEY_F1)
			ft::Renderer3D::GetCamera()->SetProjectionMode(ft::ProjectionMode::Perspective);
		if (event.key == GLFW_KEY_F2)
			ft::Renderer3D::GetCamera()->SetProjectionMode(ft::ProjectionMode::Orthographic);
	}
	return false;
}
