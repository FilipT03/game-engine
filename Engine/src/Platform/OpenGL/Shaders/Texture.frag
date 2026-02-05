//!#version 330 core
#ifdef FT_GLSL_INCLUDE
std::string textureFrag = R"(

#version 330 core

uniform sampler2D uTexture;

in vec2 chPosition;

layout(location = 0) out vec4 outColor;
uniform vec4 uColor;
uniform int uFlipY;

void main()
{
    vec2 uv = chPosition + 0.5;

    if (uFlipY == 1)
        uv.y = 1.0 - uv.y;

    outColor = texture(uTexture, uv) * uColor;
}



)";
#endif
