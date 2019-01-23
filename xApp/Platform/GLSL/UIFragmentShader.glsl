#version 330 core

in vec3 fColor;
in vec2 fTexCoords;

out vec4 FragmentColor;

uniform sampler2D u_UITexture;

void main()
{
	vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(u_UITexture, fTexCoords).r);
	FragmentColor = sampled * vec4(fColor, 1.0f);
}