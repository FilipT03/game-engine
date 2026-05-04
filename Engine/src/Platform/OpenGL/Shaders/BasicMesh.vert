//!#version 330 core
#ifdef FT_GLSL_INCLUDE
static std::string basicMeshVert = R"(

#version 330 core

uniform mat4 uViewProjection;
uniform mat4 uModel;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

out vec3 chPosition;  
out vec3 chNormal;  

void main()
{
	chPosition = vec3(uModel * vec4(inPosition, 1.0));
	chNormal = mat3(transpose(inverse(uModel))) * inNormal;
	gl_Position = uViewProjection * uModel * vec4(inPosition, 1.0);
}


)";
#endif
