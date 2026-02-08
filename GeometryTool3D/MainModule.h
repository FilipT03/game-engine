#pragma once
#include<GameEngine.h>
#include<UI/UIElement.h>
#include <GLFW/glfw3.h>

class MainModule : public ft::Module
{
public:
	void OnRegister();
	void OnDelete() {};
	void OnUpdate() {};
	bool OnEvent(const ft::Event& event) { return false; };
	bool OnKeyEvent(const ft::KeyEvent& event);
	bool OnMouseEvent(const ft::MouseEvent& event) { return false; };

private:
};

