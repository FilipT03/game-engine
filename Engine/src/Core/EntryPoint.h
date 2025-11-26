#pragma once

#ifdef FT_PLATFORM_WINDOWS

extern ft::Application* ft::CreateApplication();

int main(int argc, char** argv)
{
	ft::Log::Init();
	FT_CORE_ERROR("Error message");
	FT_INFO("Info from client!");

	auto app = ft::CreateApplication();
	app->Run();
	delete app;
}

#endif