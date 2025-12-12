#include "pch.h"
#include "Window.h"
#include "Core/Log.h"
#include "Platform/OpenGL/GLContext.h"
#include <glm/glm.hpp>

#include <stb_image.h>

namespace ft {

	Window::Window(const WindowProps& props, EventCallback eventCallback) : m_Props(props)
	{
		if (props.fullscreen)
		{
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			m_Window = glfwCreateWindow(mode->width, mode->height, props.title.c_str(), monitor, nullptr);
		}
		else
			m_Window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);
		if (m_Window == NULL)
		{
			FT_ENGINE_ERROR("Failed to create GLFW window");
			return;
		}

		#ifdef FT_OPENGL_RENDERER
			m_Context = new GLContext(m_Window);
		#else
			FT_ENGINE_ERROR("Unknown rendering API");
			return;
		#endif
		m_Context->Init();

		m_EventCallback = eventCallback;

		glfwSetWindowUserPointer(m_Window, &m_EventCallback);


		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event = WindowCloseEvent();
			callback(event);
		});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			EventCallback& callback = *(EventCallback*)glfwGetWindowUserPointer(window);

			WindowResizeEvent event = WindowResizeEvent(width, height);
			callback(event);
		});
	};

	Window::~Window() 
	{
		if (m_Window)
		{
			glfwDestroyWindow(m_Window);
			glfwTerminate();
		}
		if (m_Context)
		{
			delete m_Context;
			m_Context = nullptr;
		}
	}

	const glm::vec2 Window::GetFrameBufferSize() const
	{
		int fbWidth, fbHeight;
		glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
		return { fbWidth, fbHeight };
	}

	const glm::vec2 Window::GetWindowSize() const
	{
		int width, height;
		glfwGetWindowSize(m_Window, &width, &height);
		return { width, height };
	}

	std::unique_ptr<Window> Window::Create(const WindowProps& props, EventCallback eventCallback)
	{
		if (!glfwInit())
		{
			FT_ENGINE_ERROR("Failed to init GLFW");
			return nullptr;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		return std::make_unique<Window>(props, eventCallback);
	}
	
	void Window::Update()
	{
		m_Context->SwapBuffers();
		glfwPollEvents();
	}

	GLFWcursor* Window::LoadImageToCursor(const std::string& imagePath) const
	{
		int TextureWidth;
		int TextureHeight;
		int TextureChannels;

		unsigned char* ImageData = stbi_load(imagePath.c_str(), &TextureWidth, &TextureHeight, &TextureChannels, 0);

		if (ImageData != NULL)
		{
			GLFWimage image;
			image.width = TextureWidth;
			image.height = TextureHeight;
			image.pixels = ImageData;

			// Tacka na površini slike kursora koja se ponaša kao hitboks
			int hotspotX = TextureWidth / 5;
			int hotspotY = TextureHeight / 6;

			GLFWcursor* cursor = glfwCreateCursor(&image, hotspotX, hotspotY);
			stbi_image_free(ImageData);
			return cursor;
		}
		else {
			std::cout << "Kursor nije ucitan! Putanja kursora: " << imagePath << std::endl;
			stbi_image_free(ImageData);

		}
	}
}