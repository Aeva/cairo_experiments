--------------------------------------------------------------------------------

layout(std140, binding = 1)
uniform ScreenInfoBlock
{
	vec4 ScreenSize; // (Width, Height, InvWidth, InvHeight)
};

layout(location = 0) out vec4 Color;
void main ()
{
	vec3 UVW = vec3(gl_FragCoord.xy * ScreenSize.zw, 0.0);
	UVW.z = 1.0 - (UVW.x * UVW.y);
	Color = vec4(UVW.xzy, 1.0);
}
