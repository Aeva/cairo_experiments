//#include <math.h>
#include <iostream>
//#include <fstream>
#include <vector>
#include "glue/gl_boilerplate.h"
#include "main.h"
#include "aabb.h"
#include "quad.h"

#include "cairo.h"

ShaderPipeline BgShader;
ShaderPipeline QuadShader;
Buffer ScreenInfo;


std::vector<CairoQuad> Quads;


void DrawTestImage(cairo_t* Ctx, AABB Bounds)
{
	cairo_set_source_rgba(Ctx, 1.0, 0.0, 1.0, 0.5);
	cairo_paint(Ctx);

	//cairo_scale(Ctx, 1.0, -1.0);
	//cairo_translate(Ctx, 0.0, double(-Bounds.Height));
	const int Samples = 30;
	for (int i = 0; i <= Samples; ++i)
	{
		double a = double(i) / double(Samples);
		if (i == Samples)
		{
			cairo_set_source_rgba(Ctx, 1.0, 0.5, 0.5, 1.0);
		}
		else
		{
			cairo_set_source_rgba(Ctx, a, 0.0, a * 0.7, a);
		}
		cairo_select_font_face(Ctx, "monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(Ctx, 256.0);
		cairo_move_to(Ctx, 100.0 * a - 80.0, 200.0 * a + 500.0);
		cairo_show_text(Ctx, "OwO");
	}
}


StatusCode RenderingEvents::Setup()
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

	for (int i = 0; i < 5; ++i)
	{
		int Offset = i * 100;

		Quads.emplace_back();
		Quads[i].Resize(ScreenWidth - 200, ScreenHeight - 200);
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

	return StatusCode::PASS;
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
