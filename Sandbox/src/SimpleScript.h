#pragma once
#include<GameEngine.h>

class SimpleScript : public ft::ScriptComponent
{
public:
	void OnRegister();
	void OnUpdate();
	void OnEvent(const ft::Event& event);
};

