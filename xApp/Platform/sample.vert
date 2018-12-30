#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

uniform mat4 Projection;
uniform mat4 ModalView;

out vec3 fColor;

void main()
{
	fColor = vColor;
	gl_Position = Projection * ModalView * vec4(vPosition, 1.0f);
}
