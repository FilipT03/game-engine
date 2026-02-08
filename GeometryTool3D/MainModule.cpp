#include "MainModule.h"

void MainModule::OnRegister()
{
	FT_TRACE("MainModule registered");

	ft::Renderer3D::SetClearColor(0.1, 0.1, 0.1, 0.1);
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
