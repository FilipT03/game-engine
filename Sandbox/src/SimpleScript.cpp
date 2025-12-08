#include "SimpleScript.h"
#include <Renderer/Renderer2D.h>

void SimpleScript::OnRegister()
{
	FT_TRACE("Registered SimpleScript");

	ft::Shape* shape1 = ft::Renderer2D::AddShape<ft::Polygon>(4);
	shape1->transform.position.x -= 30.0f;
	shape1->transform.position.y -= 30.0f;
	shape1->UpdateWorldVertices();
	m_Shapes.push_back(shape1);

	ft::Shape* shape2 = ft::Renderer2D::AddShape<ft::Polygon>(3);
	shape2->transform.position.x += 30.0f;
	shape2->transform.position.y -= 30.0f;
	shape2->UpdateWorldVertices();
	m_Shapes.push_back(shape2);

	ft::Shape* shape3 = ft::Renderer2D::AddShape<ft::Ellipse>();
	shape3->transform.position.y += 30.0f;
	shape3->transform.scale.x = 50;
	shape3->transform.scale.y = 20;
	shape3->color = glm::vec4(0.7f, 0.3f, 0.4f, 1.0f);
	shape3->UpdateWorldVertices();
	m_Shapes.push_back(shape3);
	//AddShape(Line(glm::vec2{-0.4,0}, glm::vec2{ +0.4,0 }));
}

void SimpleScript::OnDelete()
{
	FT_TRACE("Removed SimpleScript");
}

void SimpleScript::OnUpdate()
{
	double fps = 1 / ft::Time::DeltaTime();
	m_FpsCount++;
	m_FpsSum += fps;
	if (m_FpsCount >= 60)
	{
		double avgFps = m_FpsSum / m_FpsCount;
		FT_INFO("Average FPS over last {} frames: {}", m_FpsCount, avgFps);
		m_FpsCount = 0;
		m_FpsSum = 0;
	}

	for (auto shape : m_Shapes)
	{
		shape->transform.rotation += 2;
		shape->color.r = sin(ft::Time::TotalTime());
		shape->color.g = cos(ft::Time::TotalTime());
		shape->color.b = cos(ft::Time::TotalTime() + glm::pi<float>() / 2.0f);
		if (shape->GetType() == ft::ShapeType::Ellipse) {
			ft::Ellipse* ellipse = dynamic_cast<ft::Ellipse*>(shape);
			ellipse->thickness = std::abs(sin(ft::Time::TotalTime()));
		}
		else {
			shape->transform.scale.x = sin(ft::Time::TotalTime()) * 20 + 30;
			shape->transform.scale.y = sin(ft::Time::TotalTime()) * 20 + 30;
			shape->UpdateWorldVertices();
		}
		shape->UpdateWorldVertices();
	}

	if (m_Panning)
	{
		FT_TRACE("X: {}, Y: {}", ft::Input::GetMouseDelta().x, ft::Input::GetMouseDelta().y);
		glm::vec2 delta = ft::Input::GetMouseDelta();
		delta.x *= -1;
		delta *= m_PanSpeed * (float)ft::Time::DeltaTime();
		ft::Renderer2D::GetCamera()->position += delta;
		ft::Renderer2D::RecalculateView();
	}
}

void SimpleScript::OnEvent(const ft::Event& event)
{
}

void SimpleScript::OnKeyEvent(const ft::KeyEvent& event)
{
	if (event.Type == ft::EventType::KeyPress && event.Key == GLFW_KEY_ESCAPE)
		ft::Application::Get().Close();
	if (event.Type == ft::EventType::KeyPress && event.Key == GLFW_KEY_G)
		ft::Application::Get().RemoveScriptComponent(GetId());
}

void SimpleScript::OnMouseEvent(const ft::MouseEvent& event)
{
	if (event.Type == ft::EventType::MousePress)
	{
		auto& pressEvent = ft::As<ft::MousePressEvent>(event);
		FT_TRACE("Mouse event: {}, mods: {}", pressEvent.Button, pressEvent.Mods);
		if ((pressEvent.Mods & GLFW_MOD_CONTROL) && pressEvent.Button == GLFW_MOUSE_BUTTON_1)
			m_Panning = true;
		//ft::Renderer2D::GetCamera()->position.x += pressEvent.Button == GLFW_MOUSE_BUTTON_1 ? 5 : -5;
		//ft::Renderer2D::RecalculateView();
	}
	if (event.Type == ft::EventType::MouseRelease)
	{
		auto& releaseEvent = ft::As<ft::MouseReleaseEvent>(event);
		if ((releaseEvent.Mods & GLFW_MOD_CONTROL) && releaseEvent.Button == GLFW_MOUSE_BUTTON_1)
			m_Panning = false;
	}
	if (event.Type == ft::EventType::MouseScroll && ft::Input::IsCtrlDown())
	{
		auto& scrollEvent = ft::As<ft::MouseScrollEvent>(event);
		FT_TRACE("Zooming {}", scrollEvent.YDelta);
		//float zoom = ft::Renderer2D::GetCamera()->zoom
		m_LogZoom += scrollEvent.YDelta * m_ZoomSpeed;
		m_LogZoom = std::clamp(m_LogZoom, -4.0f, 2.0f);
		ft::Renderer2D::GetCamera()->zoom = std::exp(m_LogZoom);
		//ft::Renderer2D::GetCamera()->zoom = exp(m_Zoom / 1.442695) - 1.0;
		ft::Renderer2D::RecalculateView();
	}
}
