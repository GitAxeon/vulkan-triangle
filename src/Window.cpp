#include "Window.hpp"
#include "Vulkan/VulkanRenderingContext.hpp"

Window::Window(const WindowInfo& info)
    : m_WindowInfo(info), m_Wrapper(std::make_unique<SDLWindowWrapper>(info))
{
    Log.Info("Created Window with custom info");
}

Window::~Window()
{
    Log.Info("Destructed window");
}

SDL_Window* Window::GetNativeWindow() const
{
    return m_Wrapper->GetNativeWindow();
}

bool Window::IsOpen() const
{
    return m_Open;
}

void Window::Close()
{
    m_Open = false;
}