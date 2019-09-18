--------------------------------------------------------------------------------

layout(std140, binding = 1)
uniform ScreenInfoBlock
{
	vec4 ScreenSize; // (Width, Height, InvWidth, InvHeight)
};

layout(binding=0) uniform sampler2D CairoSurface;

layout(location = 0) out vec4 Color;

void main ()
{
	const vec4 FgColor = texelFetch(CairoSurface, ivec2(gl_FragCoord.xy), 0);
	const vec2 UV = vec2(gl_FragCoord.xy * ScreenSize.zw);
	const vec3 BgColor = vec3(UV.x, 1.0 - (UV.x * UV.y), UV.y);
	Color = vec4(BgColor * (1.0 - FgColor.a) + FgColor.rgb, 1.0);
}
