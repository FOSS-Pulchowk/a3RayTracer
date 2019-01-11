#version 330 core

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vTexCoords;

out vec3 fColor;
out vec2 fTexCoords;

uniform mat4 Projection;

void main()
{
	fColor = vColor;
	fTexCoords = vTexCoords;
	gl_Position = vec4(vPosition, 0.0f, 1.0f);
}