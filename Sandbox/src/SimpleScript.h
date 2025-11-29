#pragma once
#include<GameEngine.h>

class SimpleScript : public ft::ScriptComponent
{
public:
	void OnRegister();
	void OnUpdate();
	void OnEvent(const ft::Event& event);
	void OnKeyEvent(const ft::KeyEvent& event);
	void OnMouseEvent(const ft::MouseEvent& event);
};

