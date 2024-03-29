--------------------------------------------------------------------------------

layout(std140, binding = 1)
uniform ScreenInfoBlock
{
	vec4 ScreenSize; // (Width, Height, InvWidth, InvHeight)
};

layout(location = 0) out vec4 Color;

void main ()
{
	const vec2 UV = vec2(gl_FragCoord.xy * ScreenSize.zw);
	Color = vec4(UV.x, 1.0 - (UV.x * UV.y), UV.y, 1.0);
}
