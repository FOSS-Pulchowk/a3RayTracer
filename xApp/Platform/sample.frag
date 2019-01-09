#version 330 core

in vec3 fColor;
in vec2 fTexCoords;
out vec4 FragmentColor;

uniform sampler2D Texture;

void main()
{
	FragmentColor = texture(Texture, fTexCoords);
	//FragmentColor = vec4(fColor, 1.0f);
}
