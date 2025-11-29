#include <GameEngine.h>
#include <Core/EntryPoint.h>

#include "SimpleScript.h"

class Sandbox : public ft::Application
{
public:
	Sandbox() : ft::Application(ft::WindowProps("App", 1600, 900), 75)
	{
		RegisterScriptComponent<SimpleScript>();
	}
	~Sandbox() {}
};

ft::Application* ft::CreateApplication()
{
	return new Sandbox();
}