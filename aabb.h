#pragma once
#include "glue/gl_boilerplate.h"

struct AABB
{
	int X;
	int Y;
	int Width;
	int Height;
	void Upload(Buffer& Dest);
};