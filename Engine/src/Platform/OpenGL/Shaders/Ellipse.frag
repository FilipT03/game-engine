//!#version 330 core
#ifdef FT_GLSL_INCLUDE
std::string ellipseFrag = R"(

#version 330 core

uniform vec4 uColor;
uniform float uThickness;
uniform float uAA;

in vec2 chPosition;

layout(location = 0) out vec4 outColor;


void main()
{
    float dist = length(chPosition*2) - 1;

    if (uThickness <= 0.0)
    {
        float alpha = 1.0 - smoothstep(-uAA, uAA, dist);
        outColor = vec4(uColor.rgb, uColor.a * alpha);
        return;
    }

    float aOuter = 1.0 - smoothstep(-uAA, uAA, dist);
    float aInner = 1.0 - smoothstep(-uThickness - uAA, -uThickness + uAA, dist);
    // Bigger circle - smaller circle = ring
    float alpha = clamp(aOuter - aInner, 0.0, 1.0);

    outColor = vec4(uColor.rgb, uColor.a * alpha);
}


)";
#endif
