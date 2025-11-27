#pragma once

#ifdef FT_PLATFORM_WINDOWS

extern ft::Application* ft::CreateApplication();

int main(int argc, char** argv)
{
	ft::Log::Init();

	auto app = ft::CreateApplication();
	app->Run();
	delete app;
}

#endif