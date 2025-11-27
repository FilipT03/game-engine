#include <GameEngine.h>

class Sandbox : public ft::Application
{
public:
	Sandbox() : ft::Application(ft::WindowProps()) {}
	~Sandbox() {}
};

ft::Application* ft::CreateApplication()
{
	return new Sandbox();
}