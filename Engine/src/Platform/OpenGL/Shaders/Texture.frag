//!#version 330 core
#ifdef FT_GLSL_INCLUDE
std::string ellipseFrag = R"(

#version 330 core

uniform vec4 uTexture;

in vec2 chPosition;

layout(location = 0) out vec4 outColor;
uniform vec4 uColor;

void main()
{
    outCol = texture(chPosition, uTexture);
}



)";
#endif
