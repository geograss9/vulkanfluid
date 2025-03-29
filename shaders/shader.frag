#version 450
#pragma optionNV (unroll all)



layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
	float deltaTime;
} ubo;

struct Param {
	vec3 texCoordOffset;		// �̻��.
	float densityAbsorption;
} param;



layout(location = 0) in vec4 inBoxParticle_Pos;
layout(location = 1) in vec3 inBoxParticle_TexCoord;
layout(location = 2) in vec3 inBoxParticle_EdgePos;

layout(location = 0) out vec4 outColor;

layout (binding = 3) uniform texture3D storageImageCopy;
layout (binding = 4) uniform sampler storageImageCopySampler;



// https://wallisc.github.io/rendering/2020/05/02/Volumetric-Rendering-Part-2.html
float BeerLambert(float absorptionCoefficient, float distanceTraveled)
{
    return exp(-absorptionCoefficient * distanceTraveled);
}



void main() {
	
	// for initial test.
	//outColor = vec4(inBoxParticle_TexCoord.xyz, 1.0);
	//
	//param.texCoordOffset = vec3(0, 0, 0);
	param.densityAbsorption = 1.0;
	//
	//float lighting = 1.0;											// ����Ʈ���� ���� ����
	//
	int numSteps = 256;
    float stepSize = 1 / float(numSteps);						// = sqrt(3) / float(numSteps);
	//										
	//vec3 cameraPos = vec3(0.0f, -2.0f, -2.0f);						// main.cpp�� ubo.view = glm::lookAt ����.
	vec3 posFromCamera = (inverse(ubo.proj) * inBoxParticle_Pos).xyz;
	vec3 dir = normalize(posFromCamera);
	//
	float boxDiagonal = sqrt(1*1 + 1*1 + 1*1);
	float brightness = 16;



	vec3 boxTexNormCoord_temp1 = vec3(0);
	vec3 boxTexNormCoord_temp2 = vec3(0);
	vec3 boxTexNormCoord_temp3 = vec3(0);
	vec3 boxTexNormCoord_temp4 = vec3(0);
	vec3 boxTexNormCoord = vec3(0);

	vec4 density = vec4(0);
	float alpha;	alpha = 1;
	
	//vec3 boxTexNormCoord_test1 = posFromCamera + (dir * 1e-6) + (dir * 3 * stepSize);
	//vec3 boxTexNormCoord_test2 = (inverse(ubo.view * ubo.model) * vec4(boxTexNormCoord_test1, 1.0)).xyz;
	//vec3 boxTexNormCoord_test3 = boxTexNormCoord_test2 + vec3(0.5);

	for (int i = 0; i < numSteps; i++)
	{ 
		boxTexNormCoord_temp1 = posFromCamera + (dir * 1e-6) + boxDiagonal * (dir * i * stepSize);
		boxTexNormCoord_temp2 = (inverse(ubo.view * ubo.model) * vec4(boxTexNormCoord_temp1, 1.0)).xyz;
		boxTexNormCoord_temp3 = boxTexNormCoord_temp2 + vec3(0.5);
		
		boxTexNormCoord = boxTexNormCoord_temp3;

		// texture3D() ��� �Լ��� ���µ�.
		density = texture(   sampler3D(storageImageCopy, storageImageCopySampler), 
							 boxTexNormCoord, 0   );
							 // storage Image�� unnormalized coordinate�� ���µ�. 360�� x,y,z�� ���ϳ��� ���ȼ�����.
							 // �ݸ� �׳� sampled image�� normalized coordinate�� data�� �޾ƿ´�!

		if (density.r > 1e-3)
		{
			//alpha += brightness * (density.r * stepSize);										// �̰� ������, alpha = 0; ���� �����ؾ���. for�� ����� alpha = alpha;
			alpha *= BeerLambert(param.densityAbsorption * brightness * density.r, stepSize);	// �̰� ������, alpha = 1; ���� �����ؾ���. for�� ����� alpha = 1 - alpha;
		}

		if (abs(boxTexNormCoord.x) > 1 
			|| abs(boxTexNormCoord.y) > 1 
			|| abs(boxTexNormCoord.z) > 1)
			break;

		if ((alpha > 1.01) || (alpha < -0.01))
			break;
	}

	alpha = 1 - alpha;			// BeerLambert()�Լ��� �Ἥ alpha�� ���ϴ� ��Ŀ����� �������� 1-alpha ���־�� �Ѵ�.
	outColor = vec4(alpha, alpha, alpha, alpha);				



	// for test.
	//vec4 getDensity = texture(sampler3D(storageImageCopy, storageImageCopySampler), boxTexNormCoord.xyz, 0);
	//outColor = vec4(getDensity.r, getDensity.r, getDensity.r, getDensity.r);
	//outColor = vec4(1, 1, 1, 1);
	//outColor = vec4(boxTexNormCoord_temp3.x, boxTexNormCoord_temp3.y, boxTexNormCoord_temp3.z, 1.0);
	//outColor = vec4(boxTexNormCoord_test4.x, boxTexNormCoord_test4.y, boxTexNormCoord_test4.z, 1.0);
	//outColor = vec4(inBoxParticle_TexCoord.x, inBoxParticle_TexCoord.y, inBoxParticle_TexCoord.z, 1.0);

}