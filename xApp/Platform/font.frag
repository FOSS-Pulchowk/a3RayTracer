#version 330 core

in vec3 fColor;
in vec2 fTexCoords;

out vec4 FragmentColor;

uniform sampler2D Texture;

void main()
{
	//vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(Texture, fTexCoords).r);
	vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(Texture, fTexCoords).a);
	FragmentColor = sampled * vec4(fColor, 1.0f);
	//FragmentColor = texture(Texture, fTexCoords);
}