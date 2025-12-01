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
	double m_FpsSum = 0;
	double m_FpsCount = 0;
};

