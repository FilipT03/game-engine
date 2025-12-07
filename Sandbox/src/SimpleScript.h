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
	const float m_ZoomSpeed = 5.0f;

	double m_FpsSum = 0;
	double m_FpsCount = 0;
	bool m_Dragging = false;
	glm::vec2 m_StartPos;
	std::vector<ft::Shape*> m_Shapes;
};

