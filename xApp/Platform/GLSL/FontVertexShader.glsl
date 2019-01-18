#version 330 core

layout (location = 0) in vec4 vPosTexCoords;

out vec2 fTexCoords;

uniform mat4 u_Projection;

void main()
{
	fTexCoords = vPosTexCoords.zw;
	gl_Position = u_Projection * vec4(vPosTexCoords.xy, 0.0f, 1.0f);
}
