#version 330 core

in vec3 fColor;
in vec2 fTexCoords;
out vec4 FragmentColor;

uniform sampler2D u_Texture;

void main()
{
	FragmentColor = texture(u_Texture, fTexCoords) * vec4(fColor, 1.0f);
}
