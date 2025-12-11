#pragma once
#include<GameEngine.h>
#include<UI/UIElement.h>

enum class Tool {
	Rectangle, Ellipse, Triangle, Pentagon, Line, Texture
};

enum class ToolModifier {
	Filled, Outline
};

enum SpecialMode {
	Normal = 0, Rainbow = 1, Rotating = 2
};

enum class InteractionMode {
	Drawing, Transforming 
};


class SimpleScript : public ft::Module
{
public:
	void OnRegister();
	void OnDelete();
	void OnUpdate();
	bool OnEvent(const ft::Event& event);
	bool OnKeyEvent(const ft::KeyEvent& event);
	bool OnMouseEvent(const ft::MouseEvent& event);

	void ClearShapes();
private:
	const float m_ZoomSpeed = 0.05f;
	const float m_PanSpeed = 100.00f;

	double m_FpsSum = 0;
	double m_FpsCount = 0;
	bool m_Panning = false;
	bool m_Ellipses = false;
	bool m_Lines = false;
	float m_LogZoom = 0.0f;
	glm::vec2 m_StartPos;
	std::vector<ft::Shape*> m_Shapes;
	std::vector<ft::UIElement*> m_UIElements;

	bool m_Drawing = false;
	ft::Shape* m_DrawingShape;
	Tool m_Tool = Tool::Rectangle;
	ToolModifier m_ToolModifier = ToolModifier::Filled;
	int m_SpecialMode = SpecialMode::Normal;
	InteractionMode m_InteractionMode = InteractionMode::Drawing;
};

