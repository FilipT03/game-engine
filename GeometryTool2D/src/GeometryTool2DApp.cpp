#include <GameEngine.h>
#include <Core/EntryPoint.h>

#include "SimpleScript.h"

class GeometryTool2D : public ft::Application
{
public:
	GeometryTool2D() : ft::Application(ft::WindowProps("App", 1920, 1080, true), 75)
	{
		SimpleScript* ss = RegisterModule<SimpleScript>();
	}
	~GeometryTool2D() {}
};

ft::Application* ft::CreateApplication()
{
	return new GeometryTool2D();
}