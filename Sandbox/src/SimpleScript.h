#pragma once
#include<GameEngine.h>

class SimpleScript : public ft::ScriptComponent
{
public:
	void OnRegister();
	void OnDelete();
	void OnUpdate();
	void OnEvent(const ft::Event& event);
	void OnKeyEvent(const ft::KeyEvent& event);
	void OnMouseEvent(const ft::MouseEvent& event);

private:
	const float m_ZoomSpeed = 0.05f;
	const float m_PanSpeed = 100.00f;

	double m_FpsSum = 0;
	double m_FpsCount = 0;
	bool m_Dragging = false;
	bool m_Panning = false;
	float m_LogZoom = 0.0f;
	glm::vec2 m_StartPos;
	std::vector<ft::Shape*> m_Shapes;
};

