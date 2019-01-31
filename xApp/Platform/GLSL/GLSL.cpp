#include "GLSLShaders.h"

namespace a3 {

	namespace Shaders {

		const char* GLBasic2DVertex = 
			"#version 330 core\n												"
			"layout(location = 0) in vec3 vPosition;							"
			"layout(location = 1) in vec3 vColor;								"
			"layout(location = 2) in vec2 vTexCoords;							"
			"uniform mat4 u_Projection;											"
			"out vec3 fColor;													"
			"out vec2 fTexCoords;												"
			"void main()														"
			"{																	"
			"	fColor = vColor;												"
			"	fTexCoords = vTexCoords;										"
			"	gl_Position = u_Projection * vec4(vPosition, 1.0f);				"
			"}																	"
			;

		const char* GLBasic2DFragment = 
			"#version 330 core\n														"
			"in vec3 fColor;															"
			"in vec2 fTexCoords;														"
			"out vec4 FragmentColor;													"
			"uniform sampler2D u_Diffuse;												"
			"void main()																"
			"{																			"
			"	FragmentColor = texture(u_Diffuse, fTexCoords) * vec4(fColor, 1.0f);	"
			"}																			"
			;

		const char* GLFontVertex = 
			"#version 330 core\n													"
			"layout(location = 0) in vec4 vPosTexCoords;							"
			"out vec2 fTexCoords;													"
			"uniform mat4 u_Projection;												"
			"void main()															"
			"{																		"
			"	fTexCoords = vPosTexCoords.zw;										"
			"	gl_Position = u_Projection * vec4(vPosTexCoords.xy, 0.0f, 1.0f);	"
			"}																		"
			;

		const char* GLFontFragment = 
			"#version 330 core\n															"
			"in vec2 fTexCoords;															"
			"out vec4 FragmentColor;														"
			"uniform vec3 u_Color;															"
			"uniform sampler2D u_FontAtlas;													"
			"void main()																	"
			"{																				"
			"	vec4 sampled = vec4(1.0f, 1.0f, 1.0f, texture(u_FontAtlas, fTexCoords).r);	"
			"	FragmentColor = sampled * vec4(u_Color, 1.0f);								"
			"}																				"
			;

		const char* GLBatch2DVertex = 
			"#version 330 core\n												"
			"layout(location = 0) in vec2 vPosition;							"
			"layout(location = 1) in vec3 vColor;								"
			"layout(location = 2) in vec2 vTexCoords;							"
			"uniform mat4 u_Projection;											"
			"out vec3 fColor;													"
			"out vec2 fTexCoords;												"
			"out vec3 fFragmentPos;												"
			"void main()														"
			"{																	"
			"	fColor = vColor;												"
			"	fTexCoords = vTexCoords;										"
			"	fFragmentPos = vec3(vPosition, 0.0f);								"
			"	gl_Position = u_Projection * vec4(vPosition, 0.0f, 1.0f);		"
			"}																	"
			;

		const char* GLBatch2DFragment = 
			"#version 330 core\n															"
			"in vec3 fColor;																"
			"in vec2 fTexCoords;															"
			"in vec3 fFragmentPos;															"
			"out vec4 FragmentColor;														"
			"uniform sampler2D u_TextureAtlas;												"
			"uniform vec2 u_SpotLightPosition;												"
			"uniform vec3 u_SpotLightColor;													"
			"uniform float u_SpotLightIntensity;											"
			"void main()																	"
			"{																				"
			"	vec4 sampled = texture(u_TextureAtlas, fTexCoords) * vec4(fColor, 1.0f);	"
			"	vec3 finalColor = sampled.rgb;												"
			"	float len = length(u_SpotLightPosition - fFragmentPos.xy);					"
			"	float intensity = u_SpotLightIntensity / (1.0f + 0.2 * len);"
			"	finalColor = intensity * finalColor * u_SpotLightColor + 0.02f * finalColor;"
			"	FragmentColor = vec4(finalColor, sampled.a);								"
			"}																				"
			;

	}

}