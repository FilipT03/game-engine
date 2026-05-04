//!#version 330 core
#ifdef FT_GLSL_INCLUDE
static std::string basicVert = R"(

#version 330 core

uniform mat4 uViewProjection;

layout(location = 0) in vec2 inWorldPosition;
layout(location = 1) in vec2 inLocalPosition;
out vec2 chPosition;

void main()
{
	chPosition = inLocalPosition;
	gl_Position = uViewProjection * vec4(inWorldPosition, 0.0, 1.0);
}


)";
#endif
