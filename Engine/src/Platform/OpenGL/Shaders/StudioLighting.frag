#ifdef FT_GLSL_INCLUDE
static std::string studioLightingFrag = R"(

#version 330 core

layout(location = 0) out vec4 outColor;

in vec3 chPosition;  
in vec3 chNormal;  

uniform vec3 uViewPosition; 
uniform vec4 uColor;

void main()
{
    vec3 normal = normalize(chNormal);
    vec3 viewDir = normalize(uViewPosition - chPosition);

    // =========================================================================
    // LIGHT SETUP (Camera Relative)
    // =========================================================================
    // We define lights relative to the view so they move with you.
    // No uniforms needed for positions.

    // Light A: KEY LIGHT (Top-Right-Front) - Provides definition
    // We construct this by offsetting the View Position
    vec3 lightPosA = uViewPosition + vec3(10.0, 10.0, 10.0);
    vec3 lightDirA = normalize(lightPosA - chPosition);

    // Light B: FILL LIGHT (Left-Front) - Softens shadows
    vec3 lightPosB = uViewPosition + vec3(-10.0, 0.0, 10.0);
    vec3 lightDirB = normalize(lightPosB - chPosition);

    // =========================================================================
    // DIFFUSE (Half-Lambert / Wrapped)
    // =========================================================================
    // Standard dot(N,L) creates harsh black shadows. 
    // Wrapped lighting maps the -1..1 dot product to 0..1 range.
    // This ensures you can see geometry even on the "dark" side.

    float NdotL_A = dot(normal, lightDirA);
    float diffA = NdotL_A * 0.5 + 0.5; // Map -1..1 to 0..1
    diffA = pow(diffA, 2.0); // Curve it slightly for contrast

    float NdotL_B = dot(normal, lightDirB);
    float diffB = NdotL_B * 0.5 + 0.5;
    
    // Light A is white (Strong), Light B is slightly cool/grey (Weak)
    vec3 diffuse = (diffA * vec3(0.7)) + (diffB * vec3(0.2, 0.2, 0.3));

    // =========================================================================
    // SPECULAR (Blinn-Phong)
    // =========================================================================
    // Reduced intensity (0.2) so it doesn't look like plastic.
    // Increased shininess (64.0) so the highlight is smaller/tighter.

    vec3 halfwayDir = normalize(lightDirA + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = vec3(0.2) * spec; // Low intensity white specular

    // =========================================================================
    // COMBINE
    // =========================================================================
    
    // 1. Multiply Diffuse by Object Color (Preserves your Cyan)
    vec3 finalColor = diffuse * pow(uColor.rgb, vec3(2.2));
    
    // 2. Add Specular on top (Gloss)
    finalColor += specular;

    // 3. Simple Ambient floor (just in case)
    finalColor += vec3(0.05) * uColor.rgb;

    // 4. Output (Gamma Corrected for monitors)
    outColor = vec4(pow(finalColor, vec3(1.0/2.2)), uColor.a);
}

)";
#endif