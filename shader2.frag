#version 450



layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	float deltaTime;
} ubo;

layout(location = 1) in vec3 in_TexCoord;

layout(location = 0) out vec4 outColor;



void main() {	
	outColor = vec4(in_TexCoord.xyz, 1.0);
}