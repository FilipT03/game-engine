#include "SimpleScript.h"
#include <Renderer/Renderer2D.h>

void SimpleScript::OnRegister()
{
	FT_TRACE("Registered SimpleScript");

	ft::Shape* shape1 = ft::Renderer2D::AddShape<ft::Polygon>(4);
	shape1->transform.position.x -= 30.0f;
	shape1->transform.position.y -= 30.0f;
	shape1->transform.scale = { 30.0f, 30.0f };
	shape1->UpdateWorldVertices();
	m_Shapes.push_back(shape1);

	ft::Shape* shape2 = ft::Renderer2D::AddShape<ft::Polygon>(3);
	shape2->transform.position.x += 30.0f;
	shape2->transform.position.y -= 30.0f;
	shape2->transform.scale = { 30.0f, 30.0f };
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
		//FT_INFO("Average FPS over last {} frames: {}", m_FpsCount, avgFps);
		m_FpsCount = 0;
		m_FpsSum = 0;
	}

	for (auto shape : m_Shapes)
	{
		//shape->transform.rotation += 2;
		shape->color.r = sin(ft::Time::TotalTime());
		shape->color.g = cos(ft::Time::TotalTime());
		shape->color.b = cos(ft::Time::TotalTime() + glm::pi<float>() / 2.0f);
		if (shape->GetType() == ft::ShapeType::Ellipse) {
			ft::Ellipse* ellipse = dynamic_cast<ft::Ellipse*>(shape);
			ellipse->thickness = std::abs(sin(ft::Time::TotalTime()));
		}
		else {
			//shape->transform.scale.x = sin(ft::Time::TotalTime()) * 20 + 30;
			//shape->transform.scale.y = sin(ft::Time::TotalTime()) * 20 + 30;
			shape->UpdateWorldVertices();
		}
		shape->UpdateWorldVertices();
	}

	if (m_Panning)
	{
		glm::vec2 delta = ft::Input::GetMouseDeltaNormalized();
		FT_INFO("Delta x {} y {}", delta.x, delta.y);
		delta.x *= -1;
		delta *= m_PanSpeed;
		ft::Renderer2D::GetCamera()->position += delta;
		ft::Renderer2D::RecalculateView();
	}
	if (m_Dragging)
	{
		glm::vec2 currentPos = ft::Renderer2D::ScreenToWorld(ft::Input::GetMousePosition());
		if (ft::Input::IsShiftDown())
		{
			glm::vec2 delta = abs(m_StartPos - currentPos);
			glm::vec2 deltaSign = sign(m_StartPos - currentPos);
			if (delta.x < delta.y)
				currentPos.y += deltaSign.y * (delta.y - delta.x);
			else
				currentPos.x += deltaSign.x * (delta.x - delta.y);
		}
		m_DrawingShape->transform.position = (m_StartPos + currentPos) / 2.0f;
		m_DrawingShape->transform.scale = abs(m_StartPos - currentPos);
		m_DrawingShape->UpdateWorldVertices();
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
	if (event.Type == ft::EventType::KeyPress && event.Key == GLFW_KEY_E)
		m_Ellipses = !m_Ellipses;
}

void SimpleScript::OnMouseEvent(const ft::MouseEvent& event)
{
	if (event.Type == ft::EventType::MousePress)
	{
		auto& pressEvent = ft::As<ft::MousePressEvent>(event);
		if (pressEvent.Button == GLFW_MOUSE_BUTTON_1)
			if (pressEvent.Mods & GLFW_MOD_CONTROL)
				m_Panning = true;
			else
			{
				m_Dragging = true;
				m_StartPos = ft::Renderer2D::ScreenToWorld(ft::Input::GetMousePosition());
				if (m_Ellipses)
					m_DrawingShape = ft::Renderer2D::AddShape<ft::Ellipse>();
				else
					m_DrawingShape = ft::Renderer2D::AddShape<ft::Rectangle>();
				m_DrawingShape->transform.position = m_StartPos;
				m_DrawingShape->transform.scale = glm::vec2(0.00001f);
			}
		//ft::Renderer2D::GetCamera()->position.x += pressEvent.Button == GLFW_MOUSE_BUTTON_1 ? 5 : -5;
		//ft::Renderer2D::RecalculateView();
	}
	if (event.Type == ft::EventType::MouseRelease)
	{
		auto& releaseEvent = ft::As<ft::MouseReleaseEvent>(event);
		if (releaseEvent.Button == GLFW_MOUSE_BUTTON_1)
		{
			m_Panning = false;
			if (m_Dragging)
			{
				m_Dragging = false;
				m_Shapes.push_back(m_DrawingShape);
			}
		}
	}
	if (event.Type == ft::EventType::MouseScroll && ft::Input::IsCtrlDown())
	{
		auto& scrollEvent = ft::As<ft::MouseScrollEvent>(event);
		glm::vec2 previousPos = ft::Renderer2D::ScreenToWorld(ft::Input::GetMousePosition());
		FT_TRACE("Previous {} {}", previousPos.x, previousPos.y);
		m_LogZoom += scrollEvent.YDelta * m_ZoomSpeed;
		m_LogZoom = std::clamp(m_LogZoom, -4.0f, 2.0f);
		ft::Camera2D* camera = ft::Renderer2D::GetCamera();
		camera->zoom = std::exp(m_LogZoom);
		ft::Renderer2D::RecalculateView();
		
		// Zooming towards the cursor, adjust position
		glm::vec2 newPos = ft::Renderer2D::ScreenToWorld(ft::Input::GetMousePosition());
		FT_TRACE("New {} {}", newPos.x, newPos.y);
		camera->position += (previousPos - newPos) * camera->zoom;
		ft::Renderer2D::RecalculateView();
	}
}
