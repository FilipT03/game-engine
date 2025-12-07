//!#version 330 core
#ifdef FT_GLSL_INCLUDE
std::string basicVert = R"(

#version 330 core

uniform mat4 uProjection;

layout(location = 0) in vec2 inWorldPosition;
layout(location = 1) in vec2 inLocalPosition;
out vec2 chPosition;

void main()
{
	chPosition = inLocalPosition;
	gl_Position = uProjection * vec4(inWorldPosition, 0.0, 1.0);
}


)";
#endif