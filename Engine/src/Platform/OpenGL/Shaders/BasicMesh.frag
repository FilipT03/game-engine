//!#version 330 core
#ifdef FT_GLSL_INCLUDE
static std::string basicMeshFrag = R"(

#version 330 core

layout(location = 0) out vec4 outColor;

in vec3 chPosition;  
in vec3 chNormal;  

uniform vec3 uLightPosition; 
uniform vec3 uViewPosition; 
uniform vec3 uLightColor;
uniform float uLightIntensity;
uniform float uAmbientIntensity;

uniform vec4 uColor;

void main()
{
    float gamma = 2.2;
    float lightDistance = length(uLightPosition - chPosition);
    float constant  = 1.0;
    float linear    = 0.09;
    float quadratic = 0.032;

    float attenuation = 1.0 /
        (constant + linear * lightDistance +
         quadratic * lightDistance * lightDistance);
    vec3 ambient = uAmbientIntensity * uLightColor;
  	
    // diffuse 
    vec3 norm = normalize(chNormal);
    vec3 lightDir = normalize(uLightPosition - chPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor * uLightIntensity * attenuation;
    
    // specular
    float specularStrength = 0.6;
    vec3 viewDir = normalize(uViewPosition - chPosition);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * uLightColor * uLightIntensity * attenuation;

    vec3 albedo = pow(uColor.rgb, vec3(gamma));
    vec3 result = (ambient + diffuse) * albedo + specular;
    
    result = pow(result, vec3(1.0/gamma)); // Gamma correction

    //outColor = uColor * vec4(ambient + diffuse + specular, 1.0);
    outColor = vec4(result, uColor.a);
}


)";
#endif
