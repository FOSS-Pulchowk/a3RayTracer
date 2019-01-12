#version 330 core

in vec2 fTexCoords;

out vec4 FragmentColor;

uniform vec3 u_Color;
uniform sampler2D u_Texture;

void main()
{
	//vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(Texture, fTexCoords).r);
	vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(u_Texture, fTexCoords).r);
	FragmentColor = sampled * vec4(u_Color, 1.0f);
	//FragmentColor = texture(u_Texture, fTexCoords);
}