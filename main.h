#pragma once
#include "glue/errors.h"
#include "glue/gl_boilerplate.h"


namespace RenderingEvents
{
	StatusCode Setup(GLFWwindow* Window);
	void Render();

	void KeyCallback(GLFWwindow* Window, int Key, int ScanCode, int Action, int Modifiers);
	void CharCallback(GLFWwindow* Window, unsigned int Codepoint);
	void CursorPosCallback(GLFWwindow* Window, double CursorX, double CursorY);
	void CursorEnterCallback(GLFWwindow* Window, int Entered);
	void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Modifiers);
	void ScrollCallback(GLFWwindow* Window, double ScrollX, double ScrollY);
}

