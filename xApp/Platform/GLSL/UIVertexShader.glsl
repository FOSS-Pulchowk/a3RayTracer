#version 330 core

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vTexCoords;

uniform mat4 u_Projection;

out vec3 fColor;
out vec2 fTexCoords;

void main()
{
	fColor = vColor;
	fTexCoords = vTexCoords;
	gl_Position = u_Projection * vec4(vPosition, 1.0f, 1.0f);
}
