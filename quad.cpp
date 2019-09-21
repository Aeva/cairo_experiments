#include "quad.h"

void CairoQuad::Init()
{
	if (Surface == nullptr)
	{
		Surface = cairo_image_surface_create(CAIRO_FORMAT_RGBA128F, Bounds.Width, Bounds.Height);
	}
	if (Texture == 0)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &Texture);
		glTextureStorage2D(Texture, 1, GL_RGBA8, Bounds.Width, Bounds.Height);
		glTextureParameteri(Texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(Texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(Texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(Texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
}


void CairoQuad::Invalidate()
{
	if (Surface != nullptr)
	{
		cairo_surface_destroy(Surface);
		Surface = nullptr;
	}
	if (Texture != 0)
	{
		glDeleteTextures(1, &Texture);
		Texture = 0;
	}
}


void CairoQuad::Move(int NewX, int NewY)
{
	if (Bounds.X != NewX || Bounds.Y != NewY)
	{
		Bounds.X = NewX;
		Bounds.Y = NewY;
		bBoundsIsDirty = true;
	}
}


void CairoQuad::Resize(int NewWidth, int NewHeight)
{
	if (NewWidth != Bounds.Width || NewHeight != Bounds.Height)
	{
		Bounds.Width = NewWidth;
		Bounds.Height = NewHeight;
		Invalidate();
		bBoundsIsDirty = true;
		bSurfaceIsDirty = true;
	}
}


void CairoQuad::SetDrawFn(CairoDrawCallback NewDrawFn)
{
	if (NewDrawFn != DrawFn)
	{
		DrawFn = NewDrawFn;
		bSurfaceIsDirty = true;
	}
}


bool CairoQuad::IsVisible()
{
	return Bounds.Width > 0 && Bounds.Height > 0;
}


void CairoQuad::Regen()
{
	if (IsVisible())
	{
		Init();

		if (bSurfaceIsDirty && DrawFn)
		{
			cairo_t* Ctx = cairo_create(Surface);
			DrawFn(Ctx, Bounds);
			cairo_surface_flush(Surface);
			cairo_destroy(Ctx);
			glTextureSubImage2D(Texture, 0, 0, 0, Bounds.Width, Bounds.Height, GL_RGBA,
				GL_FLOAT, (GLfloat*)cairo_image_surface_get_data(Surface));
		}

		if (bBoundsIsDirty)
		{
			Bounds.Upload(QuadInfo);
		}

		bSurfaceIsDirty = false;
		bBoundsIsDirty = false;
	}
}


void CairoQuad::Render()
{
	if (IsVisible() && Texture != 0)
	{
		glBindTextureUnit(0, Texture);
		QuadInfo.Bind(GL_UNIFORM_BUFFER, 2);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}
