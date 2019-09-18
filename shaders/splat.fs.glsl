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
	vec4 CairoColor = texelFetch(CairoSurface, ivec2(gl_FragCoord.xy), 0).bgra;

	vec3 UVW = vec3(gl_FragCoord.xy * ScreenSize.zw, 0.0);
	UVW.z = 1.0 - (UVW.x * UVW.y);

	Color = vec4(UVW.xzy * (1.0 - CairoColor.a) + CairoColor.rgb, 1.0);
}
