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

out gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

out vec2 UV;

void main()
{	
	// X = 0.0 if V = 0 or 2, or 5, otherwise 1.0
	// Y = 0.0 if V = 0 or 1, otherwise 1.0
	UV.x = (gl_VertexID == 0 || gl_VertexID == 2 || gl_VertexID == 4) ? 0.0 : 1.0;
	UV.y = (gl_VertexID == 0 || gl_VertexID == 1 || gl_VertexID == 5) ? 0.0 : 1.0;
	gl_Position = vec4(mix(AABB.xy, AABB.zw, UV) * ScreenSize.zw * 2.0 - 1.0, 0.0, 1.0);
	UV.y = 1.0 - UV.y;
}
