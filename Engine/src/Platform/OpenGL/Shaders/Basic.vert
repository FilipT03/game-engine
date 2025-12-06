//!#version 330 core
#ifdef FT_GLSL_INCLUDE
std::string basicVert = R"(

#version 330 core

uniform mat4 uProjection;

layout(location = 0) in vec2 inPos;
uniform vec3 uColor;
out vec3 chColor;

void main()
{
	gl_Position = uProjection * vec4(inPos, 0.0, 1.0);
	chColor = uColor;
}


)";
#endif