#include <iostream>
#include <vector>
#include "cairo.h"

#include "glue/gl_boilerplate.h"
#include "main.h"
#include "aabb.h"
#include "quad.h"

#define DEBUG_INPUT_EVENTS 0

ShaderPipeline BgShader;
ShaderPipeline QuadShader;
Buffer ScreenInfo;


std::vector<CairoQuad> Quads;


double Texel(int Pixel)
{
	return double(Pixel) + 0.5;
}
#define Texel2(PX1, PX2) Texel(PX1), Texel(PX2)
#define Texel3(PX1, PX2, PX3) Texel(PX1), Texel(PX2), Texel(PX3)
#define Texel4(PX1, PX2, PX3, PX4) Texel(PX1), Texel(PX2), Texel(PX3), Texel(PX4)


void DrawTestImage(cairo_t* Ctx, AABB Bounds)
{
	cairo_set_source_rgba(Ctx, 1.0, 0.0, 1.0, 0.5);
	cairo_paint(Ctx);

	const int Top = 0;
	const int Left = 0;
	const int Bottom = Bounds.Height - 1;
	const int Right = Bounds.Width - 1;
	const int Width = Right - Left;
	const int Height = Bottom - Top;
	const int CenterX = Bounds.Width / 2 - 1;
	const int CenterY = Bounds.Height / 2 - 1;

	const int LineWidth = 1;
	const int Margin = LineWidth + 2;

	const float FontSize = Bounds.Height / 1.8;
	const float HalfSize = FontSize / 2;

	{
		cairo_pattern_t* Gradient = cairo_pattern_create_linear(Texel4(CenterX, Top, CenterX, Bottom));
		cairo_pattern_add_color_stop_rgba(Gradient, 0.0, 1.0, 1.0, 1.0, 0.9);
		cairo_pattern_add_color_stop_rgba(Gradient, 1.0, 0.7, 0.7, 0.7, 0.9);
		cairo_set_source(Ctx, Gradient);
		cairo_pattern_destroy(Gradient);
		cairo_rectangle(Ctx, Texel2(Left + Margin, Top + Margin), Width - (Margin * 2), Height - (Margin * 2));
		cairo_fill(Ctx);
	}

	{
		cairo_set_source_rgba(Ctx, 0.85, 0.85, 0.85, 1.0);
		cairo_set_line_width(Ctx, 1.0);
		const int LineSpacing = 3;
		for (int i = LineSpacing; i < (Bottom - (Margin * 2)); i += LineSpacing)
		{
			cairo_move_to(Ctx, Texel2(Left + Margin, Top + Margin + i));
			cairo_line_to(Ctx, Texel2(Right - Margin, Top + Margin + i));
		}
		cairo_stroke(Ctx);
	}

	{
		cairo_set_source_rgba(Ctx, 1.0, 1.0, 1.0, 1.0);
		cairo_set_line_width(Ctx, double(LineWidth));
		cairo_rectangle(Ctx, Texel2(Left + Margin, Top + Margin), Width - (Margin * 2), Height - (Margin * 2));
		cairo_stroke(Ctx);
	}

	{
		const int Margin = (LineWidth * 2) + 2;
		cairo_set_source_rgba(Ctx, 0.0, 0.0, 0.0, 1.0);
		cairo_set_line_width(Ctx, double(LineWidth));
		cairo_rectangle(Ctx, Texel2(Left + Margin, Top + Margin), Width - (Margin * 2), Height - (Margin * 2));
		cairo_stroke(Ctx);
	}

	{
		cairo_select_font_face(Ctx, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(Ctx, FontSize);
		cairo_move_to(Ctx, Texel2(Left + 4, Top + (FontSize * 0.8)));
		cairo_show_text(Ctx, "o.O");
	}
}


StatusCode RenderingEvents::Setup(GLFWwindow* Window)
{
	RETURN_ON_FAIL(BgShader.Setup(
		{ {GL_VERTEX_SHADER, "shaders/splat.vs.glsl"},
		 {GL_FRAGMENT_SHADER, "shaders/splat.fs.glsl"} }));
	RETURN_ON_FAIL(QuadShader.Setup(
		{ {GL_VERTEX_SHADER, "shaders/quad.vs.glsl"},
		 {GL_FRAGMENT_SHADER, "shaders/quad.fs.glsl"} }));

	{
		GLfloat BufferData[4] = {
			float(ScreenWidth),
			float(ScreenHeight),
			1.0f / float(ScreenWidth),
			1.0f / float(ScreenHeight)
		};
		ScreenInfo.Upload((void*)&BufferData, sizeof(BufferData));
	}

	for (int i = 0; i < 4; ++i)
	{
		int Offset = i * 100;

		Quads.emplace_back();
		Quads[i].Resize(400, 400);
		Quads[i].Move(Offset, Offset);
		Quads[i].SetDrawFn(DrawTestImage);
	}

	// cheese opengl into letting us draw triangles without any data
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1);
	glFrontFace(GL_CW);
	glEnable(GL_BLEND);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetKeyCallback(Window, RenderingEvents::KeyCallback);
	glfwSetCharCallback(Window, RenderingEvents::CharCallback);
	glfwSetCursorPosCallback(Window, RenderingEvents::CursorPosCallback);
	glfwSetCursorEnterCallback(Window, RenderingEvents::CursorEnterCallback);
	glfwSetMouseButtonCallback(Window, RenderingEvents::MouseButtonCallback);
	glfwSetScrollCallback(Window, RenderingEvents::ScrollCallback);

	return StatusCode::PASS;
}


void RenderingEvents::KeyCallback(GLFWwindow* Window, int Key, int ScanCode, int Action, int Modifiers)
{
	// For "Key" defines, see https://www.glfw.org/docs/latest/group__keys.html
	// "Action" will be one of GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE.
	// The docs suggest this callback doesn't honor the keyboard layout, but
	// in practice it seems to do so just fine?
#if DEBUG_INPUT_EVENTS
	const char* KeyName = glfwGetKeyName(Key, ScanCode);
	const char* ActionName = (Action == GLFW_PRESS ? "Pressed" : (Action == GLFW_REPEAT ? "Repeat" : "Released"));
	if (KeyName)
	{
		std::cout << "KeyCallback: " << KeyName << " " << ActionName << "\n";
	}
	else
	{
		std::cout << "KeyCallback: nameless key " << ActionName << "\n";
	}
#endif
}


void RenderingEvents::CharCallback(GLFWwindow* Window, unsigned int CodePoint)
{
#if DEBUG_INPUT_EVENTS
	std::cout << "CharCallback: \"" << (char)CodePoint << "\"\n";
#endif
}


void RenderingEvents::CursorPosCallback(GLFWwindow* Window, double CursorX, double CursorY)
{
	// Top-left is the origin.
#if DEBUG_INPUT_EVENTS
	std::cout << "MouseMove: " << CursorX << ", " << CursorY << "\n";
#endif
}


void RenderingEvents::CursorEnterCallback(GLFWwindow* Window, int Entered)
{
	// Called when the Cursor enders or leaves the window.
}


void RenderingEvents::MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Modifiers)
{
	// For "Button" defines, see https://www.glfw.org/docs/latest/group__buttons.html
	// "Action" will be one of GLFW_PRESS or GLFW_RELEASE.
#if DEBUG_INPUT_EVENTS
	const char* ActionName = (Action == GLFW_PRESS ? "Pressed" : "Released");
	const char* ButtonName = (Button == GLFW_MOUSE_BUTTON_LEFT ? "Left" : (Button == GLFW_MOUSE_BUTTON_RIGHT ? "Right" : (Button == GLFW_MOUSE_BUTTON_MIDDLE ? "Middle" : "Unknown")));
	std::cout << "MouseButton:" << ButtonName << " button " << ActionName << "\n";
#endif
}


void RenderingEvents::ScrollCallback(GLFWwindow* Window, double ScrollX, double ScrollY)
{
	// ScrollY < 0 means scrolling downward.
	// ScrollY > 0 means scrolling upward.
	// Value will be something like "-1" or "-2".
#if DEBUG_INPUT_EVENTS
	std::cout << "Scroll By:" << ScrollX << ", " << ScrollY << "\n";
#endif
}


void RenderingEvents::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BgShader.Activate();
	ScreenInfo.Bind(GL_UNIFORM_BUFFER, 1);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	QuadShader.Activate();
	for (auto& Quad : Quads)
	{
		Quad.Regen();
	}
	for (auto& Quad : Quads)
	{
		Quad.Render();
	}
}
