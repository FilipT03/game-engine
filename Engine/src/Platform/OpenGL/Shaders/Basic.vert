//!#version 330 core
#ifdef FT_GLSL_INCLUDE
std::string basicVert = R"(

#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inCol;
out vec3 chColor;

void main()
{
	gl_Position = vec4(inPos, 1.0);
	chColor = inCol;
}


)";
#endif