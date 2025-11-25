#include <GameEngine.h>

class Sandbox : public ft::Application
{
public:
	Sandbox() {}
	~Sandbox() {}
};

ft::Application* ft::CreateApplication()
{
	return new Sandbox();
}