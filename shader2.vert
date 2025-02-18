#version 450



layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	float deltaTime;
} ubo;



layout(location = 0) in vec3 inBoxParticle_Pos;
layout(location = 1) in vec3 inBoxParticle_TexCoord;

//layout(location = 0) out vec4 outBoxParticle_Pos;
layout(location = 1) out vec3 outBoxParticle_TexCoord;



void main() {

	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inBoxParticle_Pos, 1.0);
	outBoxParticle_TexCoord = vec3(1, 1, 1);
	//gl_PointSize = 1.0;
	//Rasterization에 의해 나타날, lineWidth는, main.cpp에서 VkPipelineRasterizationStateCreateInfo.lineWidth 에서 설정한다.
}