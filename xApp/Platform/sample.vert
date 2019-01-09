#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec2 vTexCoords;

uniform mat4 Projection;

out vec3 fColor;
out vec2 fTexCoords;

void main()
{
	fColor = vColor;
	fTexCoords = vTexCoords;
	gl_Position = Projection * vec4(vPosition, 1.0f);
}
