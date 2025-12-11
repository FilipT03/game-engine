#pragma once
#include<GameEngine.h>
#include<UI/UIElement.h>

class SimpleScript : public ft::Module
{
public:
	void OnRegister();
	void OnDelete();
	void OnUpdate();
	bool OnEvent(const ft::Event& event);
	bool OnKeyEvent(const ft::KeyEvent& event);
	bool OnMouseEvent(const ft::MouseEvent& event);

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

	bool m_Dragging = false;
	ft::Shape* m_DrawingShape;
};

