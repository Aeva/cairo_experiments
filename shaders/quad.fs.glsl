--------------------------------------------------------------------------------

layout(std140, binding = 1)
uniform ScreenInfoBlock
{
	vec4 ScreenSize; // (Width, Height, InvWidth, InvHeight)
};

layout(std140, binding = 2)
uniform QuadBlock
{
	vec4 AABB;
};

layout(binding=0) uniform sampler2D CairoSurface;

in vec2 UV;

layout(location = 0) out vec4 Color;

void main ()
{
	Color = texture(CairoSurface, UV);
}
