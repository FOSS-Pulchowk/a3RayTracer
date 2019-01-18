#version 330 core

in vec3 fColor;
in vec2 fTexCoords;

out vec4 FragmentColor;

uniform sampler2D u_Diffuse;

void main()
{
	FragmentColor = texture(u_Diffuse, fTexCoords) * vec4(fColor, 1.0f);
}
