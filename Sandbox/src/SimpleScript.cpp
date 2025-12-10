#include "SimpleScript.h"
#include <API/Renderer2D.h>

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

	ft::Shape* tex = ft::Renderer2D::AddShape<ft::TextureQuad>("assets/test.bmp");
	tex->transform.scale.x = 40;
	tex->transform.scale.y = 40;
	tex->color = glm::vec4(0.7f, 0.3f, 0.4f, 1.0f);
	tex->UpdateWorldVertices();
	m_Shapes.push_back(tex);

	//AddShape(Line(glm::vec2{-0.4,0}, glm::vec2{ +0.4,0 }));
}

void SimpleScript::OnDelete()
{
	FT_TRACE("Removed SimpleScript");
}

void SimpleScript::OnUpdate()
{
	double fps = 1.0 / ft::Time::DeltaTime();
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
		}
		shape->UpdateWorldVertices();
	}

	if (m_Panning)
	{
		glm::vec2 delta = ft::Input::GetMouseDelta();
		glm::vec2 worldDelta = ft::Renderer2D::ScreenDeltaToWorld(delta);
		ft::Renderer2D::GetCamera()->Translate(-worldDelta);
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
		if (m_DrawingShape->GetType() == ft::ShapeType::Line)
		{
			ft::Line* line = dynamic_cast<ft::Line*>(m_DrawingShape);
			line->SetEnd(currentPos);
		}
		else
		{
			m_DrawingShape->transform.position = (m_StartPos + currentPos) / 2.0f;
			m_DrawingShape->transform.scale = abs(m_StartPos - currentPos);
		}
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
	{
		if (m_Ellipses)
		{
			m_Lines = true;
			m_Ellipses = false;
		}
		else if (m_Lines)
		{
			m_Lines = false;
			m_Ellipses = false;
		} 
		else {
			m_Lines = false;
			m_Ellipses = true;
		}
	}
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
				else if (m_Lines)
					m_DrawingShape = ft::Renderer2D::AddShape<ft::Line>(m_StartPos, m_StartPos + glm::vec2{0.0001,0.0001});
				else
					m_DrawingShape = ft::Renderer2D::AddShape<ft::TextureQuad>("assets/mushroom.png");
				m_DrawingShape->transform.position = m_StartPos;
				m_DrawingShape->transform.scale = glm::vec2(0.00001f);
			}
		if (pressEvent.Button == GLFW_MOUSE_BUTTON_2)
			if (m_Dragging)
			{
				m_Dragging = false;
				ft::Renderer2D::RemoveShape(m_DrawingShape->GetID());
			}
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
		m_LogZoom += scrollEvent.YDelta * m_ZoomSpeed;
		m_LogZoom = std::clamp(m_LogZoom, -4.0f, 2.0f);
		ft::Camera* camera = ft::Renderer2D::GetCamera();
		camera->SetZoom(std::exp(m_LogZoom));
		
		// Zooming towards the cursor, adjust position
		glm::vec2 newPos = ft::Renderer2D::ScreenToWorld(ft::Input::GetMousePosition());
		camera->Translate(previousPos - newPos);
	}
}
