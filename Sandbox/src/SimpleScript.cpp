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
		ft::Renderer2D::GetCamera()->position.x += pressEvent.Button == GLFW_MOUSE_BUTTON_1 ? 5 : -5;
		ft::Renderer2D::RecalculateView();
	}
	if (event.Type == ft::EventType::MouseScroll && ft::Input::IsCtrlDown())
	{
		auto& scrollEvent = ft::As<ft::MouseScrollEvent>(event);
		FT_TRACE("Zooming {}", scrollEvent.XDelta);
		ft::Renderer2D::GetCamera()->zoom += scrollEvent.XDelta * m_ZoomSpeed;
		ft::Renderer2D::RecalculateView();
	}
}
