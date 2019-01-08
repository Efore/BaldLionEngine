#include "blpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"

#include "GLFW/glfw3.h"
#include "BaldLion/Application.h"

#include "BaldLion/Events/MouseEvent.h"
#include "BaldLion/Events/ApplicationEvent.h"
#include "BaldLion/Events/KeyEvent.h"

namespace BaldLion
{
	static bool g_mouseJustPressed[5] = { false, false, false, false, false };

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		//TEMPORARY
		// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
	}

	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
		m_time = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());		
	}

	void ImGuiLayer::OnEvent(Event & event)
	{
		Application& app = Application::Get();

		switch (event.GetEventType())
		{
			case EventType::MouseMoved:
				MouseMovedCallback(static_cast<MouseMovedEvent*>(&event)->GetMouseX(), static_cast<MouseMovedEvent*>(&event)->GetMouseY());
				break;

			case EventType::MouseButtonPressed:
				MouseButtonCallback(static_cast<MouseButtonPressedEvent*>(&event)->GetMouseButton(), GLFW_PRESS);
				break;

			case EventType::MouseButtonReleased:				
				MouseButtonCallback(static_cast<MouseButtonReleasedEvent*>(&event)->GetMouseButton(), GLFW_RELEASE);
				break;

			case EventType::MouseScrolled:				
				ScrollCallback(static_cast<MouseScrolledEvent*>(&event)->GetOffsetX(), static_cast<MouseScrolledEvent*>(&event)->GetOffsetY());
				break;

			case EventType::KeyPressed:
				KeyCallback(static_cast<KeyPressedEvent*>(&event)->GetKeyCode(),GLFW_PRESS);
				break;

			case EventType::KeyReleased:
				KeyCallback(static_cast<KeyReleasedEvent*>(&event)->GetKeyCode(),GLFW_RELEASE);
				break;

			case EventType::Char:
				CharCallback(static_cast<CharEvent*>(&event)->GetCodepoint());
				break;

			default:
				break;
		}

		BL_LOG_TRACE("{0}", event);
	}

	void ImGuiLayer::MouseMovedCallback(float posX, float posY)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(posX, posY);
	}

	void ImGuiLayer::MouseButtonCallback(int button, int action)
	{
		ImGuiIO& io = ImGui::GetIO();

		if (button >= 0 && button < IM_ARRAYSIZE(g_mouseJustPressed))
		{
			g_mouseJustPressed[button] = action == GLFW_PRESS;
		}

		io.MouseDown[button] = g_mouseJustPressed[button];
	}

	void ImGuiLayer::ScrollCallback(double xoffset, double yoffset)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += (float)xoffset;
		io.MouseWheel += (float)yoffset;
	}

	void ImGuiLayer::KeyCallback(int key, int action)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (action == GLFW_PRESS)
			io.KeysDown[key] = true;
		if (action == GLFW_RELEASE)
			io.KeysDown[key] = false;

		// Modifiers are not reliable across systems
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
	}

	void ImGuiLayer::CharCallback(unsigned int c)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (c > 0 && c < 0x10000)
			io.AddInputCharacter((unsigned short)c);
	}
}