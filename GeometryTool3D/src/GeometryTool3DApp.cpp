#include <GameEngine.h>
#include <Core/EntryPoint.h>
#include <Modules/CameraController3D.h>

#include "MainModule.h"

class GeometryTool3D : public ft::Application
{
public:
	GeometryTool3D() : ft::Application(ft::WindowProps("App", 960, 540, false), 75)
	{
		RegisterModule<ft::CameraController3D>();
		MainModule* mm = RegisterModule<MainModule>();
	}
	~GeometryTool3D() {}
};

ft::Application* ft::CreateApplication()
{
	return new GeometryTool3D();
}