#include "SimpleScript.h"
#include <API/Renderer2D.h>
#include <API/UI.h>

void SimpleScript::OnRegister()
{
	FT_TRACE("Registered SimpleScript");

	ft::Panel* panel = ft::UI::AddElement<ft::Panel>("assets/sideBar.png", ft::Rect(0, 84, 300, 831));
	m_UIElements.push_back(panel);

	std::vector<ft::Button*> buttons;

	for (int i = 0; i < 6; i++)
	{
		ft::Button* button1 = ft::UI::AddElement<ft::Button>("assets/button.png", ft::Rect(33, 117 + i*133, 100, 100));
		m_UIElements.push_back(button1);
		buttons.push_back(button1);

		ft::Button* button2 = ft::UI::AddElement<ft::Button>("assets/button.png", ft::Rect(167, 117 + i*133, 100, 100));
		m_UIElements.push_back(button2);
		buttons.push_back(button2);
	}
	int j = 0;
	ft::Renderer2D::AddUIShape<ft::Rectangle>(ft::Transform({ 83, 167 + j*133 }, { 50, 50 }), glm::vec4(0, 0, 0, 1));
	ft::Renderer2D::AddUIShape<ft::Ellipse>(0, 0.01, ft::Transform({ 217, 167 + j*133 }, { 50, 50 }), glm::vec4(0, 0, 0, 1));
	j++;
	ft::Renderer2D::AddUIShape<ft::Polygon>(3, ft::Transform({ 83, 167 + j*133 }, { 50, 50 }, 180), glm::vec4(0, 0, 0, 1));
	ft::Renderer2D::AddUIShape<ft::Polygon>(5, ft::Transform({ 217, 167 + j*133 }, { 50, 50 }, 180), glm::vec4(0, 0, 0, 1));
	j++;
	ft::Renderer2D::AddUIShape<ft::Line>(glm::vec2{ 58, 142 + j * 133 }, glm::vec2{ 108, 192 + j * 133 }, glm::vec4(0, 0, 0, 1));
	ft::Renderer2D::AddUIShape<ft::TextureQuad>("assets/mushroom.png", ft::Transform({217, 167 + j * 133}, {60, 60}));
	j++;
	ft::Renderer2D::AddUIShape<ft::Polygon>(4, ft::Transform({ 83, 167 + j * 133 }, { 100, 100 }, 45), glm::vec4(0, 0, 0, 1));
	ft::Renderer2D::AddUIShape<ft::Polygon>(4, ft::Transform({ 217, 167 + j * 133 }, { 100, 100 }, 45), glm::vec4(0, 0, 0, 1))->isOutline = true;
	j++;

	buttons[0]->SetClickCallback([&tool = m_Tool, &modifier = m_ToolModifier]() {
		tool = Tool::Rectangle;
	});
	buttons[1]->SetClickCallback([&tool = m_Tool, &modifier = m_ToolModifier]() {
		tool = Tool::Ellipse;
	});
	buttons[2]->SetClickCallback([&tool = m_Tool, &modifier = m_ToolModifier]() {
		tool = Tool::Triangle;
	});
	buttons[3]->SetClickCallback([&tool = m_Tool, &modifier = m_ToolModifier]() {
		tool = Tool::Pentagon;
	});
	buttons[4]->SetClickCallback([&tool = m_Tool, &modifier = m_ToolModifier]() {
		tool = Tool::Line;
	});
	buttons[5]->SetClickCallback([&tool = m_Tool, &modifier = m_ToolModifier]() {
		tool = Tool::Texture;
	});
	buttons[6]->SetClickCallback([&tool = m_Tool, &modifier = m_ToolModifier]() {
		modifier = ToolModifier::Filled;
	});
	buttons[7]->SetClickCallback([&tool = m_Tool, &modifier = m_ToolModifier]() {
		modifier = ToolModifier::Outline;
	});
	buttons[8]->SetTexture("assets/rainbowMode.png");
	buttons[8]->SetClickCallback([&mode = m_SpecialMode]() {
		mode ^= SpecialMode::Rainbow;
	});
	buttons[9]->SetTexture("assets/rotatingMode.png");
	buttons[9]->SetClickCallback([&mode = m_SpecialMode]() {
		mode ^= SpecialMode::Rotating;
	});
	buttons[10]->SetTexture("assets/transformButton.png");
	buttons[10]->SetClickCallback([&mode = m_InteractionMode]() {
		if (mode == InteractionMode::Drawing)
			mode = InteractionMode::Transforming;
		else
			mode = InteractionMode::Drawing;
	});
	buttons[11]->SetTexture("assets/clearButton.png");
	buttons[11]->SetClickCallback([this]() {
		this->ClearShapes();
	});

	ft::Renderer2D::SetClearColor(0.1, 0.1, 0.1, 0.1);
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
		if (m_SpecialMode & SpecialMode::Rotating)
			shape->transform.rotation -= 2;
		if (m_SpecialMode & SpecialMode::Rainbow)
		{
			shape->color.r = sin(ft::Time::TotalTime());
			shape->color.g = cos(ft::Time::TotalTime());
			shape->color.b = cos(ft::Time::TotalTime() + glm::pi<float>() / 2.0f);
		}
		else
			shape->color = glm::vec4(1);
		//if (shape->GetType() == ft::ShapeType::Ellipse) {
			//ft::Ellipse* ellipse = dynamic_cast<ft::Ellipse*>(shape);
			//ellipse->thickness = std::abs(sin(ft::Time::TotalTime()));
		//}
		//else {
			//shape->transform.scale.x = sin(ft::Time::TotalTime()) * 20 + 30;
			//shape->transform.scale.y = sin(ft::Time::TotalTime()) * 20 + 30;
		//}
		if (m_SpecialMode & SpecialMode::Rotating)
			shape->UpdateWorldVertices();
	}

	if (m_Panning)
	{
		glm::vec2 delta = ft::Input::GetMouseDelta();
		glm::vec2 worldDelta = ft::Renderer2D::ScreenDeltaToWorld(delta);
		ft::Renderer2D::GetCamera()->Translate(-worldDelta);
	}
	if (m_Drawing)
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
			line->dashedLine = true;
		}
		else
		{
			m_DrawingShape->transform.position = (m_StartPos + currentPos) / 2.0f;
			m_DrawingShape->transform.scale = abs(m_StartPos - currentPos);
		}
		m_DrawingShape->UpdateWorldVertices();
	}
}

bool SimpleScript::OnEvent(const ft::Event& event)
{
	return false;
}

bool SimpleScript::OnKeyEvent(const ft::KeyEvent& event)
{
	if (event.type == ft::EventType::KeyPress && event.key == GLFW_KEY_ESCAPE)
		ft::Application::Get().Close();
	if (event.type == ft::EventType::KeyPress && event.key == GLFW_KEY_G)
		ft::Application::Get().RemoveModule(GetId());
	return false;
}

bool SimpleScript::OnMouseEvent(const ft::MouseEvent& event)
{
	if (event.type == ft::EventType::MousePress)
	{
		auto& pressEvent = ft::As<ft::MousePressEvent>(event);
		if (pressEvent.button == GLFW_MOUSE_BUTTON_1)
			if (pressEvent.mods & GLFW_MOD_CONTROL)
				m_Panning = true;
			else if (m_InteractionMode == InteractionMode::Drawing)
			{
				m_Drawing = true;
				m_StartPos = ft::Renderer2D::ScreenToWorld(ft::Input::GetMousePosition());
				switch (m_Tool)
				{
				case Tool::Rectangle:
					m_DrawingShape = ft::Renderer2D::AddShape<ft::Rectangle>();
					break;
				case Tool::Ellipse:
					m_DrawingShape = ft::Renderer2D::AddShape<ft::Ellipse>();
					break;
				case Tool::Triangle:
					m_DrawingShape = ft::Renderer2D::AddShape<ft::Polygon>(3);
					break;
				case Tool::Pentagon:
					m_DrawingShape = ft::Renderer2D::AddShape<ft::Polygon>(5);
					break;
				case Tool::Line:
					m_DrawingShape = ft::Renderer2D::AddShape<ft::Line>(m_StartPos, m_StartPos + glm::vec2{ 0.0001,0.0001 });
					break;
				case Tool::Texture:
					m_DrawingShape = ft::Renderer2D::AddShape<ft::TextureQuad>("assets/mushroom.png");
					break;
				default:
					break;
				}
				m_DrawingShape->transform.position = m_StartPos;
				m_DrawingShape->transform.scale = glm::vec2(0.00001f);
				if (m_ToolModifier == ToolModifier::Outline)
					if (m_Tool == Tool::Ellipse)
						dynamic_cast<ft::Ellipse*>(m_DrawingShape)->thickness = 0.1;
					else		
						m_DrawingShape->isOutline = true;
				else
					m_DrawingShape->isOutline = false;

			}
		if (pressEvent.button == GLFW_MOUSE_BUTTON_2)
			if (m_Drawing)
			{
				m_Drawing = false;
				ft::Renderer2D::RemoveShape(m_DrawingShape->GetID());
			}
	}
	if (event.type == ft::EventType::MouseRelease)
	{
		auto& releaseEvent = ft::As<ft::MouseReleaseEvent>(event);
		if (releaseEvent.button == GLFW_MOUSE_BUTTON_1)
		{
			m_Panning = false;
			if (m_Drawing)
			{
				m_Drawing = false;
				m_Shapes.push_back(m_DrawingShape);
			}
		}
	}
	if (event.type == ft::EventType::MouseScroll && ft::Input::IsCtrlDown())
	{
		auto& scrollEvent = ft::As<ft::MouseScrollEvent>(event);
		glm::vec2 previousPos = ft::Renderer2D::ScreenToWorld(ft::Input::GetMousePosition());
		m_LogZoom += scrollEvent.yDelta * m_ZoomSpeed;
		m_LogZoom = std::clamp(m_LogZoom, -4.0f, 2.0f);
		ft::WorldCamera2D* camera = ft::Renderer2D::GetCamera();
		camera->SetZoom(std::exp(m_LogZoom));
		
		// Zooming towards the cursor, adjust position
		glm::vec2 newPos = ft::Renderer2D::ScreenToWorld(ft::Input::GetMousePosition());
		camera->Translate(previousPos - newPos);
	}
	return false;
}

void SimpleScript::ClearShapes()
{
	for (auto& shape : m_Shapes)
		ft::Renderer2D::RemoveShape(shape->GetID());
	m_Shapes.clear();
}
