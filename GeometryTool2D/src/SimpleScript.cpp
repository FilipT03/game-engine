#include "SimpleScript.h"
#include <API/Renderer2D.h>
#include <API/UI.h>


void SimpleScript::OnRegister()
{
	FT_TRACE("Registered SimpleScript");

	m_DrawingCursor = ft::Application::Get().GetWindow().LoadImageToCursor("assets/cursor.png");
	m_TransformCursor = ft::Application::Get().GetWindow().LoadImageToCursor("assets/transformCursor.png");
	glfwSetCursor(ft::Application::Get().GetWindow().GetNativeWindow(), m_DrawingCursor);


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
	ft::Renderer2D::AddUIShape<ft::Rectangle>(ft::Transform2D({ 83, 167 + j*133 }, { 50, 50 }), glm::vec4(0, 0, 0, 1));
	ft::Renderer2D::AddUIShape<ft::Ellipse>(0, 0.01, ft::Transform2D({ 217, 167 + j*133 }, { 50, 50 }), glm::vec4(0, 0, 0, 1));
	j++;
	ft::Renderer2D::AddUIShape<ft::Polygon>(3, ft::Transform2D({ 83, 167 + j*133 }, { 50, 50 }, 180), glm::vec4(0, 0, 0, 1));
	ft::Renderer2D::AddUIShape<ft::Polygon>(5, ft::Transform2D({ 217, 167 + j*133 }, { 50, 50 }, 180), glm::vec4(0, 0, 0, 1));
	j++;
	ft::Renderer2D::AddUIShape<ft::Line>(glm::vec2{ 58, 142 + j * 133 }, glm::vec2{ 108, 192 + j * 133 }, glm::vec4(0, 0, 0, 1));
	ft::Renderer2D::AddUIShape<ft::TextureQuad>("assets/mushroom.png", ft::Transform2D({217, 167 + j * 133}, {60, 60}));
	j++;
	ft::Renderer2D::AddUIShape<ft::Polygon>(4, ft::Transform2D({ 83, 167 + j * 133 }, { 100, 100 }, 45), glm::vec4(0, 0, 0, 1));
	ft::Renderer2D::AddUIShape<ft::Polygon>(4, ft::Transform2D({ 217, 167 + j * 133 }, { 100, 100 }, 45), glm::vec4(0, 0, 0, 1))->isOutline = true;
	j++;

	ft::Panel* namePanel = ft::UI::AddElement<ft::Panel>("assets/namePanel.png", ft::Rect(1680, 0, 240, 150));
	m_UIElements.push_back(namePanel);

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
	buttons[10]->SetClickCallback([&mode = m_InteractionMode, this]() {
		if (mode == InteractionMode::Drawing)
		{
			mode = InteractionMode::Transforming;
			this->SetCursor(false);
		}
		else
		{
			mode = InteractionMode::Drawing;
			this->CancelSelection();
			this->SetCursor(true);
		}
	});
	buttons[11]->SetTexture("assets/clearButton.png");
	buttons[11]->SetClickCallback([this]() {
		this->ClearShapes();
		this->CancelSelection();
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

	for (auto& [id, shape] : m_Shapes)
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

	if ((m_SpecialMode & SpecialMode::Rotating) && m_SelectionBox != nullptr)
	{
		m_SelectionBox->transform.rotation -= 2;
		m_SelectionBox->UpdateWorldVertices();
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
	if (m_SelectionBox != nullptr)
		m_SelectionBox->color.a = m_TransformingMode == TransformingMode::None ? 1 : 0;

	if (m_TransformingMode == TransformingMode::Moving && m_SelectedShape != 0)
	{
		m_Shapes[m_SelectedShape]->transform.position = ft::Renderer2D::ScreenToWorld(ft::Input::GetMousePosition());
		m_Shapes[m_SelectedShape]->UpdateWorldVertices();
	}
	if (m_TransformingMode == TransformingMode::Rotating && m_SelectedShape != 0)
	{
		glm::vec2 shapeScreen = ft::Renderer2D::WorldToScreen(m_Shapes[m_SelectedShape]->transform.position);
		glm::vec2 distance = ft::Renderer2D::ScreenDeltaToWorld(shapeScreen - ft::Input::GetMousePosition());
		float scalar = glm::length(distance);
		m_Shapes[m_SelectedShape]->transform.rotation = (scalar / FT_VIEW_UNITS) * 360;
		m_Shapes[m_SelectedShape]->UpdateWorldVertices();
	}
	if (m_TransformingMode == TransformingMode::Scaling && m_SelectedShape != 0 && m_SelectionBox != nullptr)
	{
		glm::vec2 shapeScreen = ft::Renderer2D::WorldToScreen(m_Shapes[m_SelectedShape]->transform.position);
		glm::vec2 distance = ft::Renderer2D::ScreenDeltaToWorld(shapeScreen - ft::Input::GetMousePosition());
		m_Shapes[m_SelectedShape]->transform.scale = m_SelectionBox->transform.scale *  (distance / (FT_VIEW_UNITS / 7.0f));
		m_Shapes[m_SelectedShape]->UpdateWorldVertices();
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
	if (event.type == ft::EventType::KeyPress && event.key == GLFW_KEY_P)
		ft::Application::Get().RemoveModule(GetId());
	if (event.type == ft::EventType::KeyPress && event.key == GLFW_KEY_G && m_SelectedShape != 0)
	{
		if (m_TransformingMode == TransformingMode::Moving)
			m_TransformingMode = TransformingMode::None;
		else
			m_TransformingMode = TransformingMode::Moving;
		if (m_SelectionBox != nullptr)
		{
			m_SelectionBox->transform = m_Shapes[m_SelectedShape]->transform;
			m_SelectionBox->UpdateWorldVertices();
		}
	}
	if (event.type == ft::EventType::KeyPress && event.key == GLFW_KEY_R && m_SelectedShape != 0)
	{
		if (m_TransformingMode == TransformingMode::Rotating)
			m_TransformingMode = TransformingMode::None;
		else
			m_TransformingMode = TransformingMode::Rotating;
		if (m_SelectionBox != nullptr)
		{
			m_SelectionBox->transform = m_Shapes[m_SelectedShape]->transform;
			m_SelectionBox->UpdateWorldVertices();
		}
	}
	if (event.type == ft::EventType::KeyPress && event.key == GLFW_KEY_S && m_SelectedShape != 0)
	{
		if (m_TransformingMode == TransformingMode::Scaling)
			m_TransformingMode = TransformingMode::None;
		else
			m_TransformingMode = TransformingMode::Scaling;
		if (m_SelectionBox != nullptr)
		{
			m_SelectionBox->transform = m_Shapes[m_SelectedShape]->transform;
			m_SelectionBox->UpdateWorldVertices();
		}
	}
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
			else if (m_InteractionMode == InteractionMode::Transforming)
			{
				if (m_TransformingMode != TransformingMode::None)
				{
					m_TransformingMode = TransformingMode::None;
					m_SelectionBox->transform = m_Shapes[m_SelectedShape]->transform;
					m_SelectionBox->color.a = 1;
					m_SelectionBox->UpdateWorldVertices();
					return true;
				}
				bool selected = false;
				for(auto it = m_Shapes.rbegin(); it != m_Shapes.rend(); ++it)
				{
					glm::vec2 world = ft::Renderer2D::ScreenToWorld(ft::Input::GetMousePosition());
					glm::vec2 diff = glm::abs(world - it->second->transform.position);
					if (diff.x <= it->second->transform.scale.x / 2.0 && diff.y <= it->second->transform.scale.y / 2.0)
					{
						m_TransformingPressed = true;
						m_SelectingShape = it->second->GetID();
						selected = true;
						if (m_SelectionBox != nullptr)
						{
							ft::Renderer2D::RemoveShape(m_SelectionBox->GetID());
							m_SelectionBox = nullptr;
							m_SelectedShape = 0;
							m_TransformingMode = TransformingMode::None;
						}
						break;
					}
				}
				if (!selected)
				{
					CancelSelection();
				}
				return selected;
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
				m_Shapes.insert({ m_DrawingShape->GetID(), m_DrawingShape });
			}
			else if (m_TransformingPressed)
			{
				if (m_SelectingShape == 0)
					return false;
				ft::Shape* shape = m_Shapes[m_SelectingShape];

				glm::vec2 world = ft::Renderer2D::ScreenToWorld(ft::Input::GetMousePosition());
				glm::vec2 diff = glm::abs(world - shape->transform.position);
				if (diff.x <= shape->transform.scale.x / 2.0 && diff.y <= shape->transform.scale.y / 2.0)
				{
					m_SelectedShape = shape->GetID();
					m_SelectionBox = ft::Renderer2D::AddShape<ft::Rectangle>();
					m_SelectionBox->transform = shape->transform;
					m_SelectionBox->color = { 0.4, 0.4, 1, 1 };
					m_SelectionBox->isOutline = true;
					m_SelectionBox->UpdateWorldVertices();
					m_SelectingShape = 0;
					m_TransformingPressed = false;
					m_TransformingMode = TransformingMode::None;
					return true;
				}
				m_SelectingShape = 0;
				m_TransformingPressed = false;
				m_TransformingMode = TransformingMode::None;
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
	for (auto& [id, shape] : m_Shapes)
		ft::Renderer2D::RemoveShape(shape->GetID());
	m_Shapes.clear();
}

void SimpleScript::CancelSelection()
{
	m_SelectedShape = 0;
	m_SelectingShape = 0;
	m_TransformingMode == TransformingMode::None;
	if (m_SelectionBox != nullptr)
	{
		ft::Renderer2D::RemoveShape(m_SelectionBox->GetID());
		m_SelectionBox = nullptr;
	}
	m_TransformingPressed = false;
}

void SimpleScript::SetCursor(bool drawing)
{
	if (drawing)
		glfwSetCursor(ft::Application::Get().GetWindow().GetNativeWindow(), m_DrawingCursor);
	else
		glfwSetCursor(ft::Application::Get().GetWindow().GetNativeWindow(), m_TransformCursor);
}