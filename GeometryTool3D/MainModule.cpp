#include "MainModule.h"

void MainModule::OnRegister()
{
	FT_TRACE("MainModule registered");

	ft::Renderer3D::SetClearColor(0.1, 0.1, 0.1, 0.1);
}