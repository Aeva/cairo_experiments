#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include "glue/gl_boilerplate.h"
#include "glue/vector_math.h"
#include "glue/blob_builder.h"
#include "main.h"

using namespace RenderingEvents;

ShaderPipeline SplatShader;

Buffer ScreenInfo;


StatusCode RenderingEvents::Setup()
{
	RETURN_ON_FAIL(SplatShader.Setup(
		{ {GL_VERTEX_SHADER, "shaders/splat.vs.glsl"},
		 {GL_FRAGMENT_SHADER, "shaders/splat.fs.glsl"} }));

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

	// cheese opengl into letting us draw triangles without any data
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	return StatusCode::PASS;
}


void RenderingEvents::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SplatShader.Activate();
	ScreenInfo.Bind(GL_UNIFORM_BUFFER, 1);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
