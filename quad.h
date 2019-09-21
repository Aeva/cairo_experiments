#pragma once
#include "aabb.h"
#include "cairo.h"


typedef void (*CairoDrawCallback)(cairo_t* Ctx, AABB Bounds);


struct CairoQuad
{
	GLuint Texture;
	Buffer QuadInfo;
	cairo_surface_t* Surface;
	AABB Bounds;
	bool bSurfaceIsDirty;
	bool bBoundsIsDirty;
	CairoDrawCallback DrawFn;

	void Init();
	void Invalidate();
	void Move(int NewX, int NewY);
	void Resize(int NewWidth, int NewHeight);
	void SetDrawFn(CairoDrawCallback NewDrawFn);
	bool IsVisible();
	void Regen();
	void Render();
};