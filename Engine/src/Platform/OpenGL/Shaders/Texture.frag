//!#version 330 core
#ifdef FT_GLSL_INCLUDE
std::string textureFrag = R"(

#version 330 core

uniform sampler2D uTexture;

in vec2 chPosition;

layout(location = 0) out vec4 outColor;
uniform vec4 uColor;

void main()
{
    //outColor = vec4(chPosition+0.5, 0.0, 1.0);
    outColor = texture(uTexture, chPosition+0.5) * uColor;
}



)";
#endif
