#include <GameEngine.h>
#include <Core/EntryPoint.h>

#include "SimpleScript.h"

class Sandbox : public ft::Application
{
public:
	Sandbox() : ft::Application(ft::WindowProps()) 
	{
		RegisterScriptComponent<SimpleScript>();
	}
	~Sandbox() {}
};

ft::Application* ft::CreateApplication()
{
	return new Sandbox();
}