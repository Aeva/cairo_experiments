#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include "glue/gl_boilerplate.h"
#include "glue/vector_math.h"
#include "glue/blob_builder.h"
#include "main.h"

#include "cairo.h"

using namespace RenderingEvents;

ShaderPipeline SplatShader;
ShaderPipeline QuadShader;
Buffer ScreenInfo;


struct CairoQuad
{
	GLuint Texture;
	Buffer QuadInfo;
	cairo_surface_t* Surface;
	float X;
	float Y;
	float Width;
	float Height;
	void Init(int InWidth, int InHeight)
	{
		if (Surface == nullptr)
		{
			Surface = cairo_image_surface_create(CAIRO_FORMAT_RGBA128F, InWidth, InHeight);
			glCreateTextures(GL_TEXTURE_2D, 1, &Texture);
			X = 0.0;
			Y = 0.0;
			Width = float(InWidth);
			Height = float(InHeight);
		}
	}
	void Move(int InX, int InY)
	{
		X = float(InX);
		Y = float(InY);
		const size_t TotalSize = sizeof(GLfloat) * 4;
		GLfloat AABB[4] = {
			X, Y, X + Width, Y + Height
		};
		QuadInfo.Initialize((void*)& AABB, TotalSize);
	}
	void Upload()
	{
		cairo_surface_flush(Surface);
		glTextureStorage2D(Texture, 1, GL_RGBA8, Width, Height);
		glTextureSubImage2D(Texture, 0, 0, 0, Width, Height, GL_RGBA,
			GL_FLOAT,
			(GLfloat*)cairo_image_surface_get_data(Surface));
		glTextureParameteri(Texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(Texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(Texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(Texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	void Draw()
	{
		glBindTextureUnit(0, Texture);
		QuadInfo.Bind(GL_UNIFORM_BUFFER, 2);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
};


std::vector<CairoQuad> Quads;


void DrawTestImage(cairo_surface_t* Surface)
{
	cairo_t* Ctx = cairo_create(Surface);

	cairo_set_source_rgba(Ctx, 1.0, 0.0, 1.0, 0.1);
	cairo_paint(Ctx);

	//cairo_scale(Ctx, 1.0, -1.0);
	//cairo_translate(Ctx, 0.0, double(-ScreenHeight));
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
		cairo_show_text(Ctx, "Hail Eris!");
	}
}


StatusCode RenderingEvents::Setup()
{
	RETURN_ON_FAIL(SplatShader.Setup(
		{ {GL_VERTEX_SHADER, "shaders/splat.vs.glsl"},
		 {GL_FRAGMENT_SHADER, "shaders/splat.fs.glsl"} }));
	RETURN_ON_FAIL(QuadShader.Setup(
		{ {GL_VERTEX_SHADER, "shaders/quad.vs.glsl"},
		 {GL_FRAGMENT_SHADER, "shaders/quad.fs.glsl"} }));

	{
		const size_t TotalSize = sizeof(GLfloat) * 4;
		GLfloat BufferData[4] = {
			float(ScreenWidth),
			float(ScreenHeight),
			1.0f / float(ScreenWidth),
			1.0f / float(ScreenHeight)
		};
		ScreenInfo.Initialize((void*)&BufferData, TotalSize);
	}


	Quads.emplace_back();
	Quads[0].Init(ScreenWidth-200, ScreenHeight-200);
	DrawTestImage(Quads[0].Surface);
	Quads[0].Upload();
	Quads[0].Move(100, 100);


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

	SplatShader.Activate();
	ScreenInfo.Bind(GL_UNIFORM_BUFFER, 1);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	QuadShader.Activate();
	for (auto& Quad : Quads)
	{
		Quad.Draw();
	}
}
