#include <GameEngine.h>
#include <Core/EntryPoint.h>

#include "SimpleScript.h"

class Sandbox : public ft::Application
{
public:
	Sandbox() : ft::Application(ft::WindowProps("App", 1920, 1080, true), 75)
	{
		SimpleScript* ss = RegisterModule<SimpleScript>();
	}
	~Sandbox() {}
};

ft::Application* ft::CreateApplication()
{
	return new Sandbox();
}