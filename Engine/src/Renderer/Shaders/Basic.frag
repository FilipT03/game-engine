//!#version 330 core
#ifdef FT_GLSL_INCLUDE
std::string basicFrag = R"(

#version 330 core

layout(location = 0) out vec4 outColor;
in vec3 chColor;

void main()
{
	outColor = vec4(chColor, 1.0);
}


)";
#endif