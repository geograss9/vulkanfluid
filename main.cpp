//#define VK_USE_PLATFORM_WIN32_KHR	// surface ���鶧, platform-specific�� ����� ����ϵ�, glfw�� �ٷλ����ϴ� ����� ����ϵ�, �������.
#include <vulkan/vulkan.hpp>
//#include </usr/include/vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN		// ���ݱ���, �� ��ũ�� ���� ���ص�, �� �Ǿ���.
#include <GLFW/glfw3.h>
//#include </usr/include/GLFW/glfw3.h>
//#define GLFW_EXPOSE_NAIVE_WIN32	// surface ���鶧, platform-specific�� ����� ����ϵ�, glfw�� �ٷλ����ϴ� ����� ����ϵ�, �������.
//#include <GLFW/glfw3native.h>		// surface ���鶧, platform-specific�� ����� ����ϵ�, glfw�� �ٷλ����ϴ� ����� ����ϵ�, �������.
//#include </usr/include/GLFW/glfw3native.h>
#define WIN_WIDTH 800
#define WIN_HEIGHT 800

//g++ -std=c++17 -Wall -g -o vulkflu main.cpp -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi

//const char* extensionList[] = {"", ""};
#include <iostream>
std::vector<const char*> gl_userRequExtensions = {		// = userRequestingExtensions
	VK_EXT_DEBUG_UTILS_EXTENSION_NAME		// = "VK_EXT_debug_utils"
};
const std::vector<const char*> userRequDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME				// = "VK_KHR_swapchain"
	//, VK_KHR_FORMAT_FEATURE_FLAGS_2_EXTENSION_NAME	// = "VK_KHR_format_feature_flags2"
};

#ifdef NDEBUG
const bool gl_enableValidationLayers = false;
#else
const bool gl_enableValidationLayers = true;
#endif
std::vector<const char*> gl_validationLayers = {
	"VK_LAYER_KHRONOS_validation"			// ������ �빮�ڸ����� ������ enum�� �ȸ����.
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallbackFunc(VkDebugUtilsMessageSeverityFlagBitsEXT debugSeverity,
													VkDebugUtilsMessageTypeFlagsEXT debugType,
													const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
													void* pUserData)
{
	//std::cout << "Debug Callback Test" << std::endl;		// for test.
	if (debugSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
		std::cout << std::endl;
		std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;
	}
	
	//return VK_TRUE;			// If the callback returns true, then the call is aborted with the VK_ERROR_VALIDATION_FAILED_EXT error. This is normally only used to test the validation layers themselves,
	return VK_FALSE;
}



#include <optional>			// �� �ڷ����� surface�� ȣȯ�Ǵ� queueFamilyIndex�� ã�� ���� ���� ����ϰ� �ɼ��ۿ����� �ڷ����̴�.
#include <bitset>
#include <set>				// queueFamilyIndex�� ��ġ�� �ʰ�, queueCreateInfo�� �����ϱ�����, �ڷ���.

#include <fstream>
// #include <shaderc/shaderc.hpp>
#define SOURCE_TEXT_MAX_SIZE (10000)



const int MAX_FRAMES_IN_FLIGHT = 2;
static void framebufferResizeCallbackFunc(GLFWwindow* window, int width, int height);



//#include <array>
#define GLM_FORCE_RADIANS			// �Լ����� �μ���, radian�� �޵����ϴ� ��ũ���ε�.
#define GLM_FORCE_DEPTH_ZERO_TO_ONE	// GLM will use the OpenGL depth range of -1.0 to 1.0 by default. We need to configure it to use the Vulkan range of 0.0 to 1.0 using the GLM_FORCE_DEPTH_ZERO_TO_ONE definition.
#include <glm/glm.hpp>
struct Vertex {
	alignas(16) glm::vec3 pos;		// alignas(16)���� 16�� byte�� �ǹ���.
	alignas(16) glm::vec4 color;
	alignas(16) glm::vec2 texCoord;
};
const std::vector<Vertex> vertices = {
	{{-0.5f,  0.0f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	{{-0.5f,  0.0f,  0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	{{ 0.5f,  0.0f,  0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
	{{ 0.5f,  0.0f, -0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

	{{-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	{{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	{{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
	{{ 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};
const std::vector<uint16_t> indices = {
	0, 2, 1, 0, 3, 2,
	4, 6, 5, 4, 7, 6
};

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) float deltaTime;
};



// uniform buffer ������Ʈ�� �ʿ��� ������ϵ�.
#include <glm/gtc/matrix_transform.hpp>		// glm::rotate()�Լ� ������, �̰� include �ؾ���.	// we can use GLM_FORCE_RADIANS to force all GLM functions to adopt radians.
#include <glm/gtx/string_cast.hpp>			// mat4 ���ڷ� ������ִ� �Լ� include �ϱ� ����.
#include <chrono>



#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"



// for generating particles.
#include <random> 		// #include "/usr/include/c++/13/random"
#define PARTICLE_COUNT (256)
struct Particle {
	alignas(16)	glm::vec3 position;		// �ݵ�� padding�� �����. ���ߴϱ� ��� particle�� position ������Ʈ�� �� �ߵǳ�. (�е����������� �Ϻ� ��ƼŬ�� �̻��ϰ� ��������.)
	alignas(16)	glm::vec4 color;
	alignas(16)	glm::vec3 velocity;
};
#define ku_PI 3.141592		// 3.14159265358979323846



#define BOX_PARTICLE_COUNT (8)
struct BoxParticle {
	alignas(16) glm::vec3 position;
	alignas(16) glm::vec3 texCoord;
} boxParticles[BOX_PARTICLE_COUNT] = {
	{{-0.5f,  0.5f, -0.5f}, {  0.0f,  360.0f,  0.0f}},
	{{-0.5f,  0.5f,  0.5f}, {  0.0f,  360.0f,  360.0f}},
	{{ 0.5f,  0.5f,  0.5f}, {  360.0f,  360.0f,  360.0f}},
	{{ 0.5f,  0.5f, -0.5f}, {  360.0f,  360.0f,  0.0f}},
	//
	{{-0.5f, -0.5f, -0.5f}, {  0.0f,  0.0f,  0.0f}},
	{{-0.5f, -0.5f,  0.5f}, {  0.0f,  0.0f,  360.0f}},
	{{ 0.5f, -0.5f,  0.5f}, {  360.0f,  0.0f,  360.0f}},
	{{ 0.5f, -0.5f, -0.5f}, {  360.0f,  0.0f,  0.0f}}
};
const std::vector<uint16_t> boxIndices = {
	0, 1, 2, 0, 2, 3,
	0, 5, 1, 0, 4, 5,
	4, 6, 5, 4, 7, 6,
	2, 6, 7, 2, 7, 3,
	0, 7, 4, 0, 3, 7,
	1, 5, 6, 1, 6, 2
};
#define STORAGE_IMAGE_WIDTH (128)
#define STORAGE_IMAGE_HEIGHT (128)
#define STORAGE_IMAGE_DEPTH (128)
#define STORAGE_IMAGE_CHANNEL (1)
#define VECTOR_STORAGE_IMAGE_CHANNEL (3)
#define RGBA_STORAGE_IMAGE_CHANNEL (4)
const std::vector<uint16_t> boxLineIndices = {
	0, 1, 1, 2, 2, 3, 3, 0,
	0, 4, 1, 5, 2, 6, 3, 7,
	4, 5, 5, 6, 6, 7, 7, 4
};

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}






class vulkanStart2
{
public:
//////////////////////////////////////////////////////////////////////////////////////////////
	bool framebufferResized = false;
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

/////////////////////////////////////////////////////////////////////////////////////////////
private:
	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Vulkan", nullptr, nullptr);
		
		// The reason that we're creating a static function as a callback is because GLFW does not know how to 
		// properly call a member function with the right this pointer to our HelloTriangleApplication instance.
		// However, we do get a reference to the GLFWwindow in the callback and there is another GLFW function 
		// that allows you to store an arbitrary pointer inside of it: glfwSetWindowUserPointer.
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallbackFunc);
	}

	void initVulkan() {
		
		createInstance();
		// get required + user-requested extensions
		// get matched extensions with Vulkan-supporting extensions
		// get matched user-requested ValidationLayers with VulkanExtension-providing ValidationLayers.
		// DebugCallbackFunc�� �����,
		// �� �Լ���, instance�� ��ũ��ų �Լ��� �����. (vkCreateDebugUtilsMessenterEXT�� Ȯ���Լ���, instance�� �ٷ� ��������ָ鼭 �������ִ� �Լ��� �ƴϴ�.)
		setupDebugCallback();
		// Vulkan��ü���� �������, Surface�� ����� ���ؼ���, �߰��� header files�� �� include�ϴ� ����� �ִ�.
		//			platform-specific extension (OSȯ�濡 �´�, Vulkan�� 'Ȯ����', �� 'VK_KHR_win32_surface')�� �̿��Ͽ� suface�� �����ϴ� ����� ������, ( �̰��, 'glfwGetRequiredInstanceExtensions' (instance�����Ҷ� �޾Ҵ� extension����).�� �̿��ϰ�, �� include�ؾ��� header������ �ִ�. )
		//			SOȯ�� ������� ���ֹ����ʰ�, ��𼭵�, glfwCreateWindowSurface�� �̿��� �����Ҽ��� �ִ�.
		//			platform-specific extension�� �����, surface�� �����ϴ� �����, glfw��, �ڵ����� ó�����ִ� �����̴�.
		createSurface();
		// createSurface()�ϴ� �� ��ü��, instance�� ����, surface�� �����Ǵ� ���̹Ƿ�, instnace�� �翬�� surface�� �����ϴ� ���� �ǹ���.
		// �Ŀ� physicalDevice�� pick�Ǹ�, Physical Device�� ����, surface�� �����Ǵ����� Ȯ���غ��ƾ� �Ѵ�. (presentation support by Queue Families)

		// Enumerate Physical Devices.
		//			vkEnumeratePhysicalDevices()�Լ� ����Ҷ�, instance�� ��ũ������ PhysicalDevice�� ã�� �����ϴ� ���̹Ƿ�, 
		//			�׶� �̹� PhysicalDevice�� instance�� ��ũ�Ǿ, instance�� destroy�ɶ�, PhysicalDevice�� ���� �ڵ����� ��ȯ�Ǵµ�.
		// get properties and features of each physical devices.
		pickPhysicalDevice();
		// find QueueFamil'ies' on the picked physical device. 
		//			create struct for Queue Family index. 'findQueueFamilies' function's returnType is 'struct'.
		// 			find QueueFamilyIndices()�Լ�������, queueFamilyIndex���� �ߺ����� ã��, QFIndices�� ������������,
		// vkDeviceQueueCreateInfo������, �ݸ�, �� queue��, �� queueFamilyIndex���� ���� �޶�� �Ѵ�. �ߺ��� queueFamilyIndex���� ������ �ȵȴ�. (Each queue must have a unique queueFamilyIndex within pQueueCreateInfos.) 
		// vkDeviceCreateInfo  (QueueCreateInfo���� ������ ������ �迭�� �����͸� ���⿡ �־��ش�.)
		//			device���鶧, device�ȿ� ���� Queue�鵵 ���� �Ѳ����� �����ְ�, ����Եȴ�.
		createDevice();
		// get a queue handle.
		// Physical Device�� ����, surface�� �����Ǵ����� Ȯ���غ��ƾ� �Ѵ�. (presentation support by Queue Families)
		// �Ŀ�, ����ü�� ���鶧, device extension info �ִ� �ڸ���, ����ü�� extension �߰�.
		
		createCommandPool();
		// command pool�� ��������� �����ν�, resource�� data�� ���鶧, ������ �ӽ÷� ����� command buffer�� ����� ����ϰ� ȸ���ϵ��� �C���ְԵǾ���.
				
		// Checking for if swapchain device extension is supported.
		// if so, then, enable(turn on) the swapchain device extension, at the createDeviceInfo
		// Querying details of swap chain support.
		// ������ query�ؼ� ���� �����͸� swchCreateInfo�� ���� ��,
		createSwapChain();
		// Retrieve the handles of the VkImages in the Swapchain.
		createSwapChainImageViews();	// �� image����, �׿� �����ϴ� imageView�� �����.
		
		//
		// ���� ���۸� ����� ��������, commandBuffer�� ���� ������(Ư�� copyBuffer ���ɾ�), ��� createCommandPool(); ���Ŀ� �Ʒ� �Լ����� ��ġ�ؾ� �Ѵ�.
		createUniformBuffer();
		createDepthResources();
		//////
		createDensity();
		createDensityView();
		createDensityCopy();
		createDensityCopyView();
		createDensityCopySampler();
		//////
		createVelocityDensityResources();
		createVelocityDensityTempResources();
		createDivergencePressureResources();
		createDivergencePressureTempResources();
		createBCResources();
		createVorticityResources();
		//////
		createVertexBuffer();
		createIndexBuffer();
		createIndexBuffer2();
		//
		createDescriptorSetLayout();
		createDescriptorPool();
		createDescriptorSets();
		createDescriptors();
		//

		// createAttachment();
		// createAttachmentRef();	// attachment�� attachmentRef�� �����Ѵ�.
		// createSubPass();			// Subpass�� pAttachments �����ͷ�, attachment�� �ƴ�, attachmentRef �����Ͱ� ����! 
		// SubpassDependency �ۼ�.
		createRenderPass();			// pAttachments �����ͷ�, attachment �����Ͱ� ����.		// ���� �� attachment array(pAttachment)�� ���Ҽ��� �� �ε����� ��������, attachmentRef�� attament�� �����Ѵ�.

		// �� �����Ӹ��� ���徿�� �̹��� �� �̹����䰡 ������, �� ������ �̹����� �������ֵ���, �����ӹ��۸� ���� ���� �����.
		createFrameBuffers();

		//createDescriptorSetLayout();
		createPipelineLayout();		// pipelineLayout�� descriptorSetLayout�� ȣȯ�Ǿ���Ѵ�. ���� descriptorSetLayout�� ���������, pipelineLayout�� ����������Ұ��̴�.
		// compute pipeline�� �����ܰ��� compute shader stage �� ������ ����, �� �ϳ��� compute shader (stage)���� �������ִ�.
		createComputePipelines();	
		// viewport�� scissor ����. viewportState ����.
		// dynamic state ����.
		// VertexInput State ����.
		// InputAssembly State ����.
		// Rasterization State ����.
		// Multisample State ����.
		// BlendAttachment State �� BlendState(CreateInfo) ����.
		// shaderc include �ؼ� ����ϱ�. spir-v �������ϰ� shaderModule ����� ��ȯ�ϴ� �Լ� �����.
		// shader Module �� shaderStage ����.
		// GraphicsPipelineCreateInfo�� �� ������ ��� �ְ�, ����.
		createGraphicsPipeline();
		createGraphicsPipeline2();

		createCommandBuffer();
		// commandBufferObject�� �����, commandBufferMemory�� �Ҵ��ϰ�, Object�� Memory�� �Ҵ��ؾ��Ѵ�.
		// command buffer�� ���ɾ �Է��ϱ� ���ؼ���, vkBeginCommandBuffer()�Լ��� vkEndCommandBuffer()�Լ� ���̿���, ���ɾ �Է��ϴ� �Լ��� ������Ѿ��Ѵ�.
		// vkCmd �� �����ϴ� �Լ����� ����, command buffer��, ���ɾ���� �Է��ϵ��� �Ѵ�.
		// compute pipeline�� �����ϴ� ���ɾ��, 'BeginRenderPass�� BeginRenderPass ����'�� ��ġ�ϸ� �ȵǴ�. ��������.
		// graphics pipeline�� �����ϴ� ���ɾ��, 'BeginRenderPass�� BeginRenderPass ����'�� ��ġ�ؾ� �ϴ� ��!
		// transfer pipeline stage�� �����ϴ� ���ɾ��, 'BeginRenderPass�� BeginRenderPass ����' ���̵� ���̵�, �����Ӱ� ��ġ�Ҽ� �ִ� ���ϴ�.
		//recordCommandBuffer		// drawFrame()�Լ��� �����Լ��� ��ġ��Ű�� �ۼ�.

		createSyncObject();

		//// �Ʒ� �Լ��� mainLoop()�Լ� �ȿ� ������.
		////updateUniformBuffer();
		////recordCommandBuffer();
		//drawFrame();
		////

	}





// Functions in mainLoop() //////////////////////////////////////////////////////////////////////
private:	 
	void updateUniformBuffer(uint32_t inFlightFrame) {
		// �ַ� uniformBuffer data�� ������Ʈ ��.

		auto currentTime = std::chrono::high_resolution_clock::now();
		//std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
		
		//static std::chrono::steady_clock::time_point startTime = currentTime;
		static std::chrono::system_clock::time_point startTime = currentTime;		// �ʱ�ȭ �Ҷ��� ���� ��.
		static float time = 0;
		

		
		float deltaTime_ = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		time += deltaTime_;
		if (time >= 8.0f)
		{
			time = 0;
		}
		UniformBufferObject ubo{};
		ubo.deltaTime = deltaTime_;



		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		ubo.view = glm::lookAt(glm::vec3(0.0f, -1.0f, -1.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
		ubo.proj = glm::perspective(glm::radians(50.0f), ((float)swapChainExtent.width / (float)swapChainExtent.height), 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;		// GLM was originally designed for OpenGL, where the Y coordinate of the clip coordinates is inverted from Vulkan's coordinate system.

		memcpy(this->uniformBuffersMapped[inFlightFrame], &ubo, sizeof(ubo));



		startTime = std::chrono::high_resolution_clock::now();
	}
	void recordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex) {

		// If the command buffer was already recorded once, then a call to vkBeginCommandBuffer will implicitly reset it.
		VkCommandBufferBeginInfo comBuffBeginInfo{};
		comBuffBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		comBuffBeginInfo.flags = 0;							// optional.	// The flags parameter specifies how we're going to use the command buffer. 
		//comBuffBeginInfo.pInheritanceInfo = nullptr;		// optional.	// �� �Ķ���ʹ�, �ش� Begin �ϴ� ���۰�, second buffer �϶�, �ǹ��ִ� ����.

		if (vkBeginCommandBuffer(commandBuffer, &comBuffBeginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin ComBuff.");
		}	std::cout << "ComBuff has started now." << std::endl;


			



			//for (size_t i = 0; i < 2; i++)
			//{
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->computePipelines[0]);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->gctPipelineLayout,
					0, 1, &(this->descriptorSets[this->inFlightFrame]),
					0, nullptr);
				vkCmdDispatch(commandBuffer,
					(STORAGE_IMAGE_WIDTH / 8) /*+ 1*/, (STORAGE_IMAGE_HEIGHT / 8), (STORAGE_IMAGE_DEPTH / 8));



				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->computePipelines[1]);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->gctPipelineLayout,
					0, 1, &(this->descriptorSets[this->inFlightFrame]),
					0, nullptr);
				vkCmdDispatch(commandBuffer,
					(STORAGE_IMAGE_WIDTH / 8) /*+ 1*/, (STORAGE_IMAGE_HEIGHT / 8), (STORAGE_IMAGE_DEPTH / 8));



			for (size_t i = 0; i < 100; i++)
			{
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->computePipelines[2]);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->gctPipelineLayout,
					0, 1, &(this->descriptorSets[this->inFlightFrame]),
					0, nullptr);
				vkCmdDispatch(commandBuffer,
					(STORAGE_IMAGE_WIDTH / 8) /*+ 1*/, (STORAGE_IMAGE_HEIGHT / 8), (STORAGE_IMAGE_DEPTH / 8));



				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->computePipelines[3]);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->gctPipelineLayout,
					0, 1, &(this->descriptorSets[this->inFlightFrame]),
					0, nullptr);
				vkCmdDispatch(commandBuffer,
					(STORAGE_IMAGE_WIDTH / 8) /*+ 1*/, (STORAGE_IMAGE_HEIGHT / 8), (STORAGE_IMAGE_DEPTH / 8));
			}



			//for (size_t i = 4; i < (this->computePipelines).size(); i++)
			//{
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->computePipelines[4]);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->gctPipelineLayout,
					0, 1, &(this->descriptorSets[this->inFlightFrame]),
					0, nullptr);
				vkCmdDispatch(commandBuffer,
					(STORAGE_IMAGE_WIDTH / 8) /*+ 1*/, (STORAGE_IMAGE_HEIGHT / 8), (STORAGE_IMAGE_DEPTH / 8));



				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->computePipelines[5]);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->gctPipelineLayout,
					0, 1, &(this->descriptorSets[this->inFlightFrame]),
					0, nullptr);
				vkCmdDispatch(commandBuffer,
					(STORAGE_IMAGE_WIDTH / 8) /*+ 1*/, (STORAGE_IMAGE_HEIGHT / 8), (STORAGE_IMAGE_DEPTH / 8));



				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->computePipelines[6]);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->gctPipelineLayout,
					0, 1, &(this->descriptorSets[this->inFlightFrame]),
					0, nullptr);
				vkCmdDispatch(commandBuffer,
					(STORAGE_IMAGE_WIDTH / 8) /*+ 1*/, (STORAGE_IMAGE_HEIGHT / 8), (STORAGE_IMAGE_DEPTH / 8));



				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->computePipelines[7]);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->gctPipelineLayout,
					0, 1, &(this->descriptorSets[this->inFlightFrame]),
					0, nullptr);
				vkCmdDispatch(commandBuffer,
					(STORAGE_IMAGE_WIDTH / 8) /*+ 1*/, (STORAGE_IMAGE_HEIGHT / 8), (STORAGE_IMAGE_DEPTH / 8));



				
			//}
			





			this->cmdImgMemBarrier(commandBuffer, (this->density),
				VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT,
				VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

			VkExtent3D imageSize = { (uint32_t)STORAGE_IMAGE_WIDTH, (uint32_t)STORAGE_IMAGE_HEIGHT, (uint32_t)STORAGE_IMAGE_DEPTH };
			cmdCopyImage(commandBuffer,
				this->density, this->densityCopy,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				imageSize);

			this->cmdImgMemBarrier(commandBuffer, (this->density),
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL,
				VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_SHADER_WRITE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

			this->cmdImgMemBarrier(commandBuffer, (this->densityCopy),
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_SHADER_READ_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);



			VkRenderPassBeginInfo renderPassBeginInfo{};
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = this->renderPass;
			renderPassBeginInfo.renderArea.extent = this->swapChainExtent;
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			//
			// clear�ؾ��� attachment��, colorAttachment�� depthAttachment��, �̷��� ���� 2�� ���� �����Ƿ�,
			// VkClearValue��ü��, ������ Attachment�� ������, ���� ����� �־��־�� �Ѵ�.
				std::array<VkClearValue, 2> clearValues;	//  = { clearValue0, clearValue1 };
					VkClearValue clearValue0; clearValue0.color = { 0.0f, 0.0f, 0.0f, 0.0f };
											  //clearValue1.depthStencil = { 1.0f, 0 };
				clearValues[0] = clearValue0;
					//VkClearDepthStencilValue v; v.depth = 0.0f; v.stencil = 0;
					VkClearValue clearValue1; //clearValue0.color = { 0.1f, 0.1f, 0.1f, 1.0f }; 
											  clearValue1.depthStencil = { 1.0f, 0 };
				clearValues[1] = clearValue1;
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();
			//
			renderPassBeginInfo.framebuffer = framebuffers[imageIndex];
			// The imageIndex refers to the VkImage in our swapChainImages array.

			// �Ʒ��� viewport ũ�� �����Է��ϴ� �ڵ带 �ּ�ó���ϸ�, window resize�ص� ȭ��ũ�Ⱑ �Ⱥ��ϰԵǰ�, 
			// �ּ�ó�� ���ϸ�, �������Ӹ��� ���ο� window resize ũ�Ⱑ swapChain�� ����ũ�⸦ �����Ͽ�, 
			// �ᱹ �װ� viewport ũ���� ��ȭ�� �̾�����. 
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(swapChainExtent.width);
			viewport.height = static_cast<float>(swapChainExtent.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(commandBuffer, 0, 1, &(this->viewport));

			scissor.offset = { 0, 0 };
			scissor.extent = this->swapChainExtent;
			vkCmdSetScissor(commandBuffer, 0, 1, &(this->scissor));

			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);



				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipeline);
				VkBuffer boxVertexBuffers[] = { this->vertexBuffer };
				VkDeviceSize boxOffsets[] = { 0 };
				// index of the first vertex input binding. the number of vertex input bindings.
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, boxVertexBuffers, boxOffsets);
				vkCmdBindIndexBuffer(commandBuffer, this->indexBuffer, 0, VK_INDEX_TYPE_UINT16);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->gctPipelineLayout,
										// the index of the first descriptor set, the number of sets to bind, and the array of sets to bind.
										0, 1, &((this->descriptorSets)[this->inFlightFrame]),
										0, nullptr);
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(boxIndices.size()), 1, 0, 0, 0);
			


			// vkCmdClearAttachments()�Լ��� renderPass �ȿ��� ����ɼ�����!
			VkClearValue clearDepthValue{};
			clearDepthValue.depthStencil.depth = 1.0f;

			VkClearAttachment clearAttachmentInfo{};
			clearAttachmentInfo.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			clearAttachmentInfo.clearValue = clearDepthValue;
			// colorAttachment is only meaningful if VK_IMAGE_ASPECT_COLOR_BIT is set in aspectMask, in which case it is an index into the currently bound color attachments.
			//clearAttachmentInfo.colorAttachment = ;

			VkRect2D rectInfo{};
			rectInfo.extent = this->swapChainExtent;
			rectInfo.offset = VkOffset2D{0, 0};

			VkClearRect clearRectInfo{};
			clearRectInfo.rect = rectInfo;
			clearRectInfo.baseArrayLayer = 0;
			clearRectInfo.layerCount = 1;

			vkCmdClearAttachments(commandBuffer, 1, &clearAttachmentInfo, 1, &clearRectInfo);



				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphicsPipeline2);
				vkCmdBindVertexBuffers(commandBuffer, 0, 1, boxVertexBuffers, boxOffsets);
				vkCmdBindIndexBuffer(commandBuffer, this->indexBuffer2, 0, VK_INDEX_TYPE_UINT16);
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->gctPipelineLayout,
					// the index of the first descriptor set, the number of sets to bind, and the array of sets to bind.
					0, 1, &((this->descriptorSets)[this->inFlightFrame]),
					0, nullptr);
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(boxLineIndices.size()), 1, 0, 0, 0);



			vkCmdEndRenderPass(commandBuffer);



			this->cmdImgMemBarrier(commandBuffer, (this->densityCopy),
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT,
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

			

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

	}
	void drawFrame() {
		// 0. ���� �������� �������� ���� ��������, 
		// 1. ����(�̹�) �������� �������� �غ�(������������ imageIndex �޾ƿ���, uniformBuffer ������Ʈ, commandBuffer�� �ۼ�, ����)�ϰ�,
		//		�̶�, interrupt����ϰ�, window�� resize�Ǵ� �޽����� ����, �̿� ������ �޴�, swapChain���� imageIndex�� �޾ƿ��� �κ���, �� ó���ϴ� �κ� ������ ��.
		// 2. ���� �������� ������ ���image��, framebuffer? swapChain? ���� present�ؾ��Ѵ�.
		//		�̶�, interrupt����ϰ�, window�� resize�Ǵ� �޽����� ����, �̿� ������ �޴�, presentation�ܰ踦, �� ó���ϴ� �κ� ������ ��.
		std::cout << std::endl;		// �α� ��¿�  ���� ����.



		// 0. �� fence�� set�Ǹ�(���� 1�̵Ǹ�), "���� �������� �������� ���� �������� ��Ȳ."�� �ǹ��ϰԵ�. ��, ������������ ���image�� present�Ǳ� �����ϴ� ��Ȳ. �׷��� ���� swapChain�� ȭ�鿡 ����� ���������� ���� ��Ȳ�ε�.
		vkWaitForFences(this->device, 1, &(this->inFlightFences[this->inFlightFrame]), VK_TRUE, UINT64_MAX);



		// 1. ����(�̹�) �������� �������� �غ�(������������ imageIndex �޾ƿ���)
		uint32_t imageIndex;
		// The iamgeIndex refers to the VkImage in our swapChainImages array.
		// ��Ȯ����, �̹����� �������� �ϴ°� �ƴ϶�, ���� �����ӿ� image�� �� index�ڸ��� �˾Ƴ��� ����, �ϴ°ų�.
		VkResult result = vkAcquireNextImageKHR(this->device, this->swapChain,
			UINT64_MAX, this->imageAvailableSemaphores[this->inFlightFrame],
			VK_NULL_HANDLE, &imageIndex);

		// window�� resize�Ǹ�, swapChain�� recreate�ǵ��� �� �κ�.
		// VK_ERROR_OUT_OF_DATE_KHR: The swap chain has become incompatible with the surface and can no longer be used for rendering. Usually happens after a window resize.
		// VK_SUBOPTIMAL_KHR: The swap chain can still be used to successfully present to the surface, but the surface properties are no longer matched exactly.
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;		// return�����ν�, �̹� �������� �ٽ� �����ϱ�. (�̹� �������� �������� ����.)
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to recreate swapChain and get images.");
		}

		// �� �ڵ���� �տ� �� fence�� reset��Ű�� �ڵ尡 ������, 
		// swapchain�� recreate�� ��, �ΰ��Ǵ� �� ����, �ٷ� �ٽ� fence�� wait�ϰ� �Ǳ� ������, 
		// �ΰ��� ���� ���, ������ fence�� 1�� set���� �ʴ� ���� �߻��Ѵ�.
		// �� ������, recreate swapChain�� ����, fence�� 0���� reset�ϰ� �ڵ������ν�, ����wait�� ������ �Ѱ�.
		vkResetFences(this->device, 1, &(this->inFlightFences[this->inFlightFrame]));



		// 1. ����(�̹�) �������� �������� �غ�(���� Buffer, Image data ������Ʈ)
		updateUniformBuffer(this->inFlightFrame);



		// 1. ����(�̹�) �������� �������� �غ�(commandBuffer�� �ۼ�)
		vkResetCommandBuffer(this->gctCommandBuffers[this->inFlightFrame], 0);

		// command buffer �� (recodrf ��) begin ��Ű������, CommandBufferBeginInfo�� �ۼ��Ѵ�.�� �� �����Ѵ�.
		// renderPass �� Begin �Ѵ�.
		// vkCmd �� �����ϴ� �Լ����, command buffer��, ���ɵ��� �Է��Ѵ�.
		recordCommandBuffer(this->gctCommandBuffers[this->inFlightFrame], imageIndex);



		// 1. ����(�̹�) �������� �������� �غ�(commandBuffer�� ����)
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &(this->gctCommandBuffers[this->inFlightFrame]);
		//
		// a pointer to an array of VkSemaphore handles upon which to wait before the command buffers for this batch begin execution.
		// submit�� queue�۾����� ���۵Ǳ� ����, set�Ǿ��־���� semaphore.
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &(this->imageAvailableSemaphores[this->inFlightFrame]);
		//// queue�� submit�Ǳ� ����, �۾��� �����־���� stage.
		VkPipelineStageFlags WaitDstStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		//// VkPipelineStageFlags�� enum���ڰ��� ��� ����, VkPipelineStageFlagBits�� enum���ڰ�(enum constant).
		submitInfo.pWaitDstStageMask = WaitDstStages;
		//
		// a pointer to an array of VkSemaphore handles which will be signaled when the command buffers for this batch have completed execution.
		// submit�� queue�۾����� �� �Ϸ�Ǹ�, set�� semaphore.
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &(this->renderFinishedSemaphores[this->inFlightFrame]);	// ��, submit�� command buffer �� �Էµ� ������ �� ����Ϸ�Ǹ�, signal set�� semaphore�� �����Ѵ�!

		// The last parameter references an optional fence that will be signaled when the command buffers finish execution.
		// This allows us to know when it is safe for the command buffer to be reused.
		if (vkQueueSubmit(this->Queues[0], 1, &submitInfo, this->inFlightFences[this->inFlightFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit the command buffer.");
		}	std::cout << "command buffer is submitted." << std::endl;
		// this->Queues[0]�� graphicsQueueFamily�� queue�� �ǹ���.



		// 2. ���� �������� ������ ���image��, framebuffer? swapChain? ���� present�ϴ� �ܰ�.
		// present ��, pipeline�� �� �Ϸ�������, submitting the result back to the swap chain. �ϴ� ���� ����.
		// �׷���, present ������, CPU�� GPU�� �������Ӹ��� ���� ��ȣ�� �༭, ȭ�鿡 ����ϰ� �ϴ� �ſ���? GPU�� ��� �˾Ƽ� ����ϴ°� �ƴϰ�?
		// �װ� �ƴ϶��, ��, �� �������� drawFrame()�Լ��� ����ɶ�����, �Ʒ� �ڵ尡 ����ǰ�, �ڵ带 �ۼ�����?
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &(this->swapChain);
		presentInfo.pImageIndices = &imageIndex;
		// the number of semaphores to wait for before issuing the present request. �ո����� ��...
		// ��, ����̹����� swapChain���� �����ϱ� ����, wait���� �����Ǿ��־�� ��, semaphore���� �����ϴ°� �´� �� ����... 
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &(this->renderFinishedSemaphores[this->inFlightFrame]);
		presentInfo.pResults = nullptr;		// swapChain�� presentation �����ϸ� VK_SUCCESS �� ������, VkResult�� ������ �־��ָ� ��. �ʿ������,  nullptr��.

		// window resizing ������, vkQueuePresentKHR �� �� ������� �ʾҴٸ�, 
		// swapChain recreate�ϰ�, ���� ���������� �Ѿ��.
		result = vkQueuePresentKHR(this->Queues[0], &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR
			|| result == VK_SUBOPTIMAL_KHR
			|| framebufferResized)
		{
			framebufferResized = false;
			recreateSwapChain();
			// return ���� �������ν�, ���� ���������� �Ѿ��.
		}
		else if (result == VK_SUCCESS) {
			std::cout << "image is presented to swapChain" << std::endl;
			std::cout << "swapChain width = " << this->swapChainExtent.width << std::endl;
			std::cout << "swapChain height = " << this->swapChainExtent.height << std::endl;
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present the image to swapChain.");
		}



		// ���� inFlight Frame ��(index) ���.
		this->inFlightFrame = (this->inFlightFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
	////
	void mainLoop() {
		while(!glfwWindowShouldClose(this->window)) {

			drawFrame();
			glfwPollEvents();
		}

		//vkQueueWaitIdle(this->Queues[0]);
		vkDeviceWaitIdle(this->device);
	}
	
	



	//1////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions in initVulkan() ///////////////////////////////////////////////////////////////////////////////////////
private:
	void createInstance() {

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.apiVersion = VK_API_VERSION_1_3;
		appInfo.pApplicationName = "VulkanStart2";
		appInfo.applicationVersion = static_cast<uint32_t>(1.0);
		appInfo.pEngineName = "start2engine";
		appInfo.engineVersion = static_cast<uint32_t>(1.0);



		std::vector<std::string> userRequWindowRequExtensions;
		this->getUserRequWindowRequExtensions(userRequWindowRequExtensions);
																			 
		std::vector<const char*> matchedExtensions;
		if (!this->checkExtensions(userRequWindowRequExtensions, matchedExtensions)) {
			throw std::runtime_error("Extensions requested, but not available!");
		}


		//uint32_t howManyValidationLayersMatched = 0;
		std::vector<const char*> matchedValidationLayers;
		if ( gl_enableValidationLayers 
			 && !(this->checkValidationLayers(matchedValidationLayers)) )
		{
			throw std::runtime_error("Validation layers requested, but not available!");
		}


		VkDebugUtilsMessengerCreateInfoEXT debugCallbackInfoForInst{};
		this->populateDebugCallbackInfo(debugCallbackInfoForInst);


		VkInstanceCreateInfo instCreateInfo{};
		instCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instCreateInfo.pApplicationInfo = &appInfo;
		instCreateInfo.enabledExtensionCount = static_cast<uint32_t>(matchedExtensions.size());
		instCreateInfo.ppEnabledExtensionNames = matchedExtensions.data();

		if(gl_enableValidationLayers) {
			instCreateInfo.enabledLayerCount = static_cast<uint32_t>(matchedValidationLayers.size());
			instCreateInfo.ppEnabledLayerNames = matchedValidationLayers.data();
			instCreateInfo.pNext = &debugCallbackInfoForInst;
			//instCreateInfo.flags = 0;
		} 
		else {
			instCreateInfo.enabledLayerCount = 0;
			instCreateInfo.ppEnabledLayerNames = nullptr;
			instCreateInfo.pNext = nullptr;
			//instCreateInfo.flags = 0;
		}

		//VkResult result;
		if(vkCreateInstance(&instCreateInfo, nullptr, &(this->instance)) != VK_SUCCESS) {
			throw std::runtime_error("failed to create Vulkan Instance!");
		}
	}
	void setupDebugCallback() {
		if(!gl_enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT debugCallbackInfo{};
		this->populateDebugCallbackInfo(debugCallbackInfo);

		if (this->createDebugUtilsMessengerEXT(this->instance, 
   											   &debugCallbackInfo, 
   											   nullptr, 
   											   &(this->debugMessenger))  
			!=  VK_SUCCESS) 
		{
			throw std::runtime_error("failed to set up debugCallback function!");								
		}
	}
	void createSurface() {
		if (glfwCreateWindowSurface(this->instance, this->window, nullptr, &(this->surface)) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		// if (lookInside_glfwCreateWindowSurface(this->instance, this->window, nullptr, &(this->surface)) != VK_SUCCESS) {
		// 	throw std::runtime_error("failed to create window surface!");
		// }

		//if (glfwCreateWindowSurface(this->instance, this->window, nullptr, &(this->surface)) != VK_SUCCESS) {
		//	throw std::runtime_error("failed to create window surface!");
		//}
	}

	void pickPhysicalDevice() {
		uint32_t phyDeviceCount = 0;
		vkEnumeratePhysicalDevices(this->instance, &phyDeviceCount, nullptr);
		if(phyDeviceCount ==0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}
		std::vector<VkPhysicalDevice> phyDevices(phyDeviceCount);
		vkEnumeratePhysicalDevices(this->instance, &phyDeviceCount, phyDevices.data());

		for (const auto& phyDevice : phyDevices)
		{
			if (isDeviceSuitable(phyDevice)) {
				this->physicalDevice = phyDevice;
				return;
			}
		}

		throw std::runtime_error("failed to peak a physical device!");			
	}
	void createDevice() {

		//QueueFamilyIndices QFIndices;		// to member variable.
		QFIndices = findQueueFamilies(this->physicalDevice);

		std::set<uint32_t> uniqueQueueFamilyIndices = { QFIndices.graphicsQueueFamilyIndex.value(),
														QFIndices.presentQueueFamilyIndex.value(),
														QFIndices.computeQueueFamilyIndex.value() };
		std::vector<float> queueFamilyPriorities;		// vector�� const �ڷ����� ������ �ڷ������� �Ҵ� �����ֳ�
		queueFamilyPriorities.resize(uniqueQueueFamilyIndices.size(), 1.0f);
		
		std::vector<VkDeviceQueueCreateInfo> dQueueCreateInfos;
		uint32_t i = 0;
		for (const auto& uniqueQueueFamilyIndex : uniqueQueueFamilyIndices) {
			VkDeviceQueueCreateInfo dQueueCreateInfo{};
			dQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			dQueueCreateInfo.queueFamilyIndex = uniqueQueueFamilyIndex;
			dQueueCreateInfo.queueCount = 1;
			// queueFamilyPriorities.push_back(1.0f);	// �̷��� �ϸ� for�� �ۿ��� �Ҵ��ߴ� ������ �޸𸮰� ������µ�. �ڲ� Priorities�� ���� �Ѿ��ٰ� ���� �߳�.
			dQueueCreateInfo.pQueuePriorities = &queueFamilyPriorities[i];
			dQueueCreateInfos.push_back(dQueueCreateInfo);
			i++;
		}

		

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		//
		// The defiference between information of instance and device, is that, extensions and validation layers of device are 'device' specific.
		// An example of a device specific extension is VK_KHR_swapchain.
		// It is possible that there are Vulkan devices in the system that lack this ability, for example because they only support compute operations.
		// Previous implementations of Vulkan made a distinction between instance and device specific validation layers, 
		// but this is no longer the case. That means that the enabledLayerCount and ppEnabledLayerNames fields of VkDeviceCreateInfo are ignored by up - to - date implementations.
		// However, it is still a good idea to set them anyway to be compatible with older implementations :
		// ���: instance���鶧 ������ �����̴�, device ���鶧�� �ߺ������� ������ �ʿ���� ��?
		// ������, ��ġ���ʴ� extension. VL�� �߰�����߰���. ��) "VK_KHR_swapchain"
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(userRequDeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = userRequDeviceExtensions.data();
		//deviceCreateInfo.enabledLayerCount;			
		//deviceCreateInfo.ppEnabledLayerNames;
		//
		// Each queue must have a unique queueFamilyIndex within pQueueCreateInfos. (�� queue��, �� queueFamilyIndex���� ���� �޶�� �Ѵ�. �ߺ��� queueFamilyIndex���� ������ �ȵȴ�. ) 
		// ��, The 'queueFamilyIndex' member of each element of pQueueCreateInfos 'must be unique' within pQueueCreateInfos
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(dQueueCreateInfos.size());
		std::cout << std::endl << std::endl << "number of created unique queue family: " << dQueueCreateInfos.size() << std::endl;	// for test.
		deviceCreateInfo.pQueueCreateInfos = dQueueCreateInfos.data();
		//
		VkPhysicalDeviceProperties PhyDeProperties{};
		vkGetPhysicalDeviceProperties(this->physicalDevice, &PhyDeProperties);
		std::cout << "PhyDeProperties maxBoundDescriptorSets: " << PhyDeProperties.limits.maxBoundDescriptorSets << std::endl;
		// �� �׷���ī�忡�� 32����� ����.
		//
		VkPhysicalDeviceFeatures PhyDeFeatures{};		// this is difference from PhyDeFeatures in isSuitable() function.
		vkGetPhysicalDeviceFeatures(this->physicalDevice, &PhyDeFeatures);
		deviceCreateInfo.pEnabledFeatures = &PhyDeFeatures;
		//
		//deviceCreateInfo.flags = 0;

		if(vkCreateDevice(this->physicalDevice, &deviceCreateInfo, nullptr, &(this->device)) != VK_SUCCESS) {
			throw std::runtime_error("failed to create device!");
		}



		// get queue handles.
		// �� �׷���ī�忡���� queueFamily�� �ϳ��� ���õɰű� ������, queue�� �ϳ��� ������ ����.
		// �� �ϳ��� queue�� graphics, compute, transit �� ��.
		Queues.resize(uniqueQueueFamilyIndices.size());
		uint32_t j = 0;
		for (const auto& uniqueQueueFamilyIndex : uniqueQueueFamilyIndices) {
			vkGetDeviceQueue(this->device, uniqueQueueFamilyIndex, 0, &Queues[j]);
			std::cout << j+1 << "th queue handle is acquired" << std::endl;
			j++;
		}
	}

	void createCommandPool() {
		// Queue�� ���ʹ� �ٸ���, commandBuffer��, ���δٸ� �� commandBuffer ������, ���ΰ���(�ߺ��Ǵ�) queueFamilyIndex�� �������ִ�.

		VkCommandPoolCreateInfo gctCommandPoolInfo{};
		gctCommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		gctCommandPoolInfo.queueFamilyIndex = QFIndices.graphicsQueueFamilyIndex.value();
		// �������ӹ̴� command buffer �ٽ� record ���� �Ŷ��, �Ʒ� �÷��׷� �����������. VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
		gctCommandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(this->device, &gctCommandPoolInfo, nullptr, &(this->gctCommandPool))
			!= VK_SUCCESS)
		{
			throw std::runtime_error("failed to create Graphics Command Pool.");
		}	std::cout << "Graphics & Compute & Transfer - Command Pool is created." << std::endl;

	}

	void createSwapChain() {
			VkSurfaceCapabilitiesKHR surfaceCapabilInfo{};
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->physicalDevice, this->surface, &surfaceCapabilInfo);
			
			uint32_t swapChainImageCount = 1;
			//swapChainImageCount = surfaceCapabilInfo.minImageCount + 1;
			swapChainImageCount = MAX_FRAMES_IN_FLIGHT;

			//swchCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;

			swapChainFormat = VK_FORMAT_B8G8R8A8_SRGB;

			//swchCreateInfo.imageUsage = surfaceCapabilInfo.supportedUsageFlags; �ϰ� 
			// VK_FORMAT_B8G8R8A8_SRGB ����ϸ� ������.
			// �׳� VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ����.

			int tempWidth = 0;
			int tempHeight = 0;
			glfwGetFramebufferSize(this->window, &tempWidth, &tempHeight);
			this->swapChainExtent.width = tempWidth;
			this->swapChainExtent.height = tempHeight;
			//swapChainExtent.width = surfaceCapabilInfo.currentExtent.width;
			//swapChainExtent.height = surfaceCapabilInfo.currentExtent.height;
			
			

			VkSwapchainCreateInfoKHR swchCreateInfo{};
			swchCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swchCreateInfo.surface = this->surface;
			swchCreateInfo.minImageCount = swapChainImageCount;
			swchCreateInfo.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			// recreate swapChain �ÿ�, resizing�� �� ũ����ȯ�� �ǰ� �Ϸ��� �̺κа�, ��Ÿ surface���� ������ �κ��� �� �ڵ����� �ϴ� ��?��
			swchCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;	// = 0x00000001
			swchCreateInfo.clipped = VK_TRUE;
			swchCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swchCreateInfo.oldSwapchain = VK_NULL_HANDLE;
			// queueFamilyIndexCount is the number of queue families having access to the image(s) of the swapchain when imageSharingMode is VK_SHARING_MODE_CONCURRENT.
			swchCreateInfo.queueFamilyIndexCount = 0;
			swchCreateInfo.pQueueFamilyIndices = nullptr;
			//
			swchCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			//swchCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT 
			//							| VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
			//							| VK_IMAGE_USAGE_TRANSFER_DST_BIT 
			//							| VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			swchCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swchCreateInfo.imageFormat = swapChainFormat;					// or VK_FORMAT_R8G8B8A8_UNORM ?
			swchCreateInfo.imageExtent = swapChainExtent;
			swchCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
			// imageArrayLayers is the number of views in a multiview/stereo surface. For non-stereoscopic-3D applications, this value is 1.
			swchCreateInfo.imageArrayLayers = 1;

			if(  vkCreateSwapchainKHR(device, &swchCreateInfo, nullptr, &(this->swapChain))  !=  VK_SUCCESS  ) {
				throw std::runtime_error("failed to create Swapchain!");
			}



			vkGetSwapchainImagesKHR(this->device, this->swapChain, &swapChainImageCount, nullptr);
			this->swapChainImages.resize(swapChainImageCount);
			vkGetSwapchainImagesKHR(this->device, this->swapChain, &swapChainImageCount, this->swapChainImages.data());

			std::cout << "swapChain and " << swapChainImageCount << " swapChainImages are created" << std::endl;
			// swapChainImageCount = 3 �� ����.
	}
	void createSwapChainImageViews() {

		swapChainImageViews.resize(swapChainImages.size());


		VkComponentMapping ComponentMapping{};
		ComponentMapping.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ComponentMapping.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ComponentMapping.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ComponentMapping.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		
		VkImageSubresourceRange imageSubresInfo{};
		imageSubresInfo.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageSubresInfo.baseMipLevel = 0;
		imageSubresInfo.levelCount = 1;
		imageSubresInfo.baseArrayLayer = 0;
		imageSubresInfo.layerCount = 1;

		for (uint32_t i = 0; i < swapChainImages.size(); i++)
		{
			VkImageViewCreateInfo imageViewCreateInfo{};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = swapChainImages[i];
			imageViewCreateInfo.format = this->swapChainFormat;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.components = ComponentMapping;
			imageViewCreateInfo.subresourceRange = imageSubresInfo;
			//imageViewCreateInfo.flags = 0;

			if(vkCreateImageView(this->device, &imageViewCreateInfo, nullptr, &(this->swapChainImageViews[i])) != VK_SUCCESS ) {
				throw std::runtime_error("failed to create image views for Swapchain");
			}
		}

		std::cout << swapChainImageViews.size() << " swapChainImageViews are created" << std::endl;
	}

	void createRenderPass() {
		
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		// initialLayout is the layout the attachment image subresource will be in when a render pass instance begins.
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		// VK_IMAGE_LAYOUT_UNDEFINED  ��¥�� clear�ɰŴϱ� don't care��.
		// finalLayout is the layout the attachment image subresource will be transitioned to when a render pass instance ends.
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		// value specifying how the contents of color and depth components of the attachment are treated at the beginning of the subpass where it is first used.
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		VkAttachmentReference colorAttachmentRef{};
		// attachment is either an integer value identifying an attachment at the corresponding index in VkRenderPassCreateInfo::pAttachments, or VK_ATTACHMENT_UNUSED to signify that this attachment is not used.
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;		// glsl���� ������ layout����, �ٸ� ������ image layout�̴�!



		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = this->depthFormat;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		// Ȥ�� VK_IMAGE_LAYOUT_UNDEFINED	// ó���� attachment ���鶧����, Ư���� ����������, ������ VK_IMAGE_LAYOUT_UNDEFINED�� �����ؾ� �ϴ� ��.  
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


		
		VkSubpassDescription subpass{};
		// pipelineBindPoint is a VkPipelineBindPoint value specifying the pipeline type supported for this subpass.
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		//subpass.depthStencilAttachmentCount�� ������ 1���� �ۿ� ������, ���ʿ� Count�� ���� �ʿ䰡 ���µ�.
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		//subpass.inputAttachmentCount;
		//subpass.pInputAttachments;
		//subpass.preserveAttachmentCount;
		//subpass.pPreserveAttachments;
		//subpass.pResolveAttachments;
		// flags is a bitmask of VkSubpassDescriptionFlagBits specifying usage of the subpass.
		//subpass.flags = 0;



		//VkDependencyInfo dependencyInfo{};		// �̰Ŵ� VkSubpassDependencyInfo �ʹ� �ٸ�����! �̰Ŵ�, specifying dependency information for a synchronization command.
		// SubpassDependency�� semaphor����� ���þ��� ������ ������ ��!
		VkSubpassDependency dependencyInfo{};
		dependencyInfo.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencyInfo.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
									  | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		// ������ �޸𸮸� ���� attachment�� �̹����� ���ؼ�, src�� read�� ����, write�� �ߵ�, �޸𸮿� ���� ����� �������� �������. ���� 0�̴�. color attachment�� clear�ǵ��� �տ��� ���������Ƿ�, ���� src�� �� attachment�� read�� �ʿ䵵 ����!
		// ���� ���� ��������ϴ� ������Ȳ������, src(��) subPass�� dst(��,����) subPass�� �����͸� �����ϴ� ��Ȳ�� �ƴϹǷ�, ��SubPass�� ��SubPass�κ��� ���� input(srcAccess)�� ���ٴ� �ǹ̷�, �� ���ڸ� 0���� �־ �Ǵµ�.
		// src subPass��, �ش� �޸�(attachment image data)�� ������ � operation�� �����ߵ�, 
		// dst SubPass�������� �Ѿ����, �ش� memory��, attachment�� ���� load�Ǹ鼭, clear�� ���̱� ������, (load op������ ���� clear�϶�� �����Ǿ����Ƿ�)
		// ���ΰ��� ���Ӽ��� ���ٰ� ���� ��.
		dependencyInfo.srcAccessMask = 0;	
		dependencyInfo.dstSubpass = 0;
		dependencyInfo.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
									  | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		// ������ �޸𸮸� ���� attachment�� �̹����� ���ؼ�, dst�� draw�� �̹�����, src�� �ǵ�ȴ� �޸𸮿� ������ �޸��� color Attachment��, write�ϴ� ���� �Ϸμ� �������ִ� Subpass�̴�. ���� �״�� �����ָ� �ȴ�. 
		dependencyInfo.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		//dependencyInfo.dependencyFlags = 0;



		std::vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment };

		VkRenderPassCreateInfo RPCreateInfo{};
		RPCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RPCreateInfo.subpassCount = 1;
		RPCreateInfo.pSubpasses = &subpass;
		RPCreateInfo.dependencyCount = 1;
		RPCreateInfo.pDependencies = &dependencyInfo;
		RPCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		RPCreateInfo.pAttachments = attachments.data();
		//RPCreateInfo.flags = 0;
		
		if (vkCreateRenderPass(this->device, &RPCreateInfo, nullptr, &(this->renderPass))  !=  VK_SUCCESS) {
			throw std::runtime_error("failed to create RanderPass.");
		}

		std::cout << "RenderPass is created" << std::endl;
	}

	void createFrameBuffers() {
		// �� imageView(attachment��) ���� framebuffer�� �ϳ��� �����? �� �̷��� ��ȿ�������� ������?
		// �ϳ��� framebuffer �� ���� �̹��� �Ѳ����� �������ְ� ����� �ȵų�?
		// �� �̹����� ���� �ٸ� �����ӿ� ���� �̹����� �׷���?

		(this->framebuffers).resize((this->swapChainImageViews).size());

		for (size_t i = 0; i < (this->swapChainImageViews).size(); i++)
		{
			std::array<VkImageView, 2> imageToAttachments = {(this->swapChainImageViews)[i], this->depthImageView};

			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = this->renderPass;
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageToAttachments.size());
			framebufferCreateInfo.pAttachments = imageToAttachments.data();	// �̸��� pAttachments �� �Ǿ�������, a pointer to an array of 'VkImageView handles', �̴�. �� attachment�� �� imageViews��� �ǹ̷� �̷��� �̸� ���ѵ�.
			framebufferCreateInfo.width = swapChainExtent.width;
			framebufferCreateInfo.height = swapChainExtent.height;
			framebufferCreateInfo.layers = 1;								// �׳� �̹��� �޸𸮰����� 3���� �ε������ �����ϸ� �ȴ�.
		
			if (vkCreateFramebuffer(this->device, &framebufferCreateInfo, nullptr, &(this->framebuffers)[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffers.");
			}
		}
		
		std::cout << "Framebuffers are created." << std::endl;
	}

	
	
	void createUniformBuffer() {

		VkDeviceSize bufferSize = static_cast<VkDeviceSize>(sizeof(UniformBufferObject));

		(this->uniformBuffers).resize(MAX_FRAMES_IN_FLIGHT);
		(this->uniformBufferMemories).resize(MAX_FRAMES_IN_FLIGHT);
		(this->uniformBuffersMapped).resize(MAX_FRAMES_IN_FLIGHT);



		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkBufferCreateInfo uboCreateInfo{};
			uboCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			uboCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			uboCreateInfo.size = bufferSize;
			uboCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			if (vkCreateBuffer(this->device, &uboCreateInfo, nullptr, &(this->uniformBuffers[i])) != VK_SUCCESS) {
				throw std::runtime_error("failed to create uniform buffer.");
			}	std::cout << "Uniform Buffer is created." << std::endl;



			VkMemoryRequirements memRequirements{};
			vkGetBufferMemoryRequirements(this->device, this->uniformBuffers[i], &memRequirements);

			VkMemoryAllocateInfo uniBuffMemAllocInfo{};
			uniBuffMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			uniBuffMemAllocInfo.allocationSize = memRequirements.size;
			uniBuffMemAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits,
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				| VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			if (vkAllocateMemory(this->device, &uniBuffMemAllocInfo, nullptr, &(this->uniformBufferMemories[i])) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate uniform buffer memory.");
			}	std::cout << "Uniform buffer memory is allocated." << std::endl;



			vkBindBufferMemory(this->device, this->uniformBuffers[i], this->uniformBufferMemories[i], 0);



			vkMapMemory(this->device, this->uniformBufferMemories[i], 0, bufferSize, 0, &((this->uniformBuffersMapped)[i]));
		}

	}
	void createDepthResources() {
		
		VkImageCreateInfo imgCreateInfo{};
		imgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		VkExtent3D swapChainExtent3D{ this->swapChainExtent.width, this->swapChainExtent.height, 1};
		imgCreateInfo.extent = swapChainExtent3D;
		imgCreateInfo.arrayLayers = 1;
		// Physical device����, depth image format����, ����ڰ� ������ format�� �����ϴ���, query�غ��ƾ� �Ѵ�.
		std::vector<VkFormat> userDepthFormats = { VK_FORMAT_D24_UNORM_S8_UINT,
												   VK_FORMAT_D32_SFLOAT_S8_UINT,
												   VK_FORMAT_D32_SFLOAT };
		this->depthFormat = findSupportedDepthImageFormat(userDepthFormats, 
													 VK_IMAGE_TILING_OPTIMAL,
													 VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		//std::cout << "depthFormat: " << this->depthFormat << std::endl;		// for test.
		imgCreateInfo.format = this->depthFormat;
		imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imgCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imgCreateInfo.mipLevels = 1;
		imgCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imgCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// �Ʒ��� ���δٸ� queueFamily�� image�� �����ؼ� ���� ä���ִ� �Ķ���ʹ�.
		//imgCreateInfo.queueFamilyIndexCount ; 
		//imgCreateInfo.pQueueFamilyIndices ;

		if ( vkCreateImage(this->device, &imgCreateInfo, nullptr, &(this->depthImage)) != VK_SUCCESS ) {
			throw std::runtime_error("failed to create depth iamge.");
		}	std::cout << "Depth image is created!" << std::endl;



		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(this->device, this->depthImage, &memRequirements);

		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memRequirements.size;
		memAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits, 
														   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if ( vkAllocateMemory(this->device, &memAllocInfo, nullptr, &(this->depthImageMemory)) != VK_SUCCESS ) {
			throw std::runtime_error("failed to allocate depth image memory.");
		}	std::cout << "Depth image memory is allocated!" << std::endl;



		vkBindImageMemory(this->device, this->depthImage, this->depthImageMemory, 0);
		// ����
		// Memory should be bound by calling vkBindImageMemory().
		// If image is non - sparse then it must be bound completely and contiguously to a single VkDeviceMemory object.



		// ����
		// imageView�� �����ϱ� ����, �ݵ�� image�� �Ҵ�� memory�� bind �Ǿ��־�� �Ѵ�! �ȱ׷��� validation layer error �޽��� ��.
		VkImageViewCreateInfo imgViewCreateInfo{};
		imgViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imgViewCreateInfo.image = this->depthImage;
		imgViewCreateInfo.format = this->depthFormat;
		// Below is about swiggling.
		imgViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
		imgViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
		imgViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
		imgViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
		imgViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imgViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		imgViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imgViewCreateInfo.subresourceRange.layerCount = 1;
		imgViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imgViewCreateInfo.subresourceRange.levelCount = 1;

		if (vkCreateImageView(this->device, &imgViewCreateInfo, nullptr, &(this->depthImageView)) != VK_SUCCESS) {
			throw std::runtime_error("failed to create depth iamge view.");
		}	std::cout << "Depth image view is created!" << std::endl;

	}
	void createDensity() {

		int texWidth, texHeight, texDepth, texChannels;
		texWidth = STORAGE_IMAGE_WIDTH;
		texHeight = STORAGE_IMAGE_HEIGHT;
		texDepth = STORAGE_IMAGE_DEPTH;
		texChannels = STORAGE_IMAGE_CHANNEL;
		VkDeviceSize image3Dsize = texWidth * texHeight * texDepth * texChannels * sizeof(float);	// �� �������� ä�δ� byte��.
		
		std::vector<float> pixels(image3Dsize, 0.0f);		// build�� clean���ϸ�, �������࿡�� �ʱ�ȣ�� ������ �״�� ���� �����࿡�� �̾�����;;  std::vector ������, �Ҵ�� �޸� �ּҰ� ������, �������࿡�� �ʱ�ȭ�� �����ϱ� ������ ���������ߴ� �����Ͱ� �̾����� ��!
		
		std::cout << "Test for initializing values of density in CPU: " << pixels[ (2 + 0*texWidth + 0*texWidth*texHeight) * texChannels ] << std::endl;



		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createAllocBindBuffer(stagingBuffer, stagingBufferMemory, image3Dsize, 
								VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
								| VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
								VK_BUFFER_USAGE_TRANSFER_SRC_BIT);



		void* data;
		vkMapMemory(this->device, stagingBufferMemory, 0, image3Dsize, 0, &data);
		memcpy(data, pixels.data(), static_cast<size_t>(image3Dsize));
		vkUnmapMemory(this->device, stagingBufferMemory);



		VkExtent3D tempExtent = { (uint32_t)texWidth, (uint32_t)texHeight, (uint32_t)texDepth };
		std::vector<VkFormat> userRequFormat;	userRequFormat.push_back(VK_FORMAT_R32_SFLOAT);
		createAllocBindImage(this->density, this->densityMemory, 
			VK_IMAGE_TYPE_3D, tempExtent, userRequFormat, 0,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);



		VkCommandBuffer tempComBuff;
		allocBeginComBuff(tempComBuff);

			cmdBuffMemBarrier(tempComBuff, stagingBuffer,
				(VkDeviceSize)0, VK_WHOLE_SIZE, 
				// srcAccessMask (VK_ACCESS_HOST_WRITE_BIT) is not supported by stage mask (VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT).
				0, VK_ACCESS_TRANSFER_READ_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
			cmdImgMemBarrier(tempComBuff, this->density,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0, VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

			cmdCopyBufferToImage(tempComBuff, 
				stagingBuffer, this->density, 
				0, 
				VkOffset3D{0, 0, 0}, tempExtent);

			cmdImgMemBarrier(tempComBuff, this->density,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL,
				VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_WRITE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

		endSubmitFreeComBuff(tempComBuff);



		vkFreeMemory(this->device, stagingBufferMemory, nullptr);
		vkDestroyBuffer(this->device, stagingBuffer, nullptr);

	}
	void createDensityView() {

		//VkFormatProperties formProp{};
		//vkGetPhysicalDeviceFormatProperties(this->physicalDevice, VK_FORMAT_R32_SFLOAT, &formProp);
		//std::cout << "FormatProperties.bufferFeatures: " << std::hex << formProp.bufferFeatures << std::endl;
		//std::cout << "FormatProperties.linearTilingFeatures: " << std::hex << formProp.linearTilingFeatures << std::endl;
		//std::cout << "FormatProperties.optimalTilingFeatures: " << std::hex << formProp.optimalTilingFeatures << std::endl;
			
		createImageView(this->densityView, this->density, VK_FORMAT_R32_SFLOAT, VK_IMAGE_VIEW_TYPE_3D);

	}
	void createDensityCopy() {

		VkExtent3D tempExtent = { (uint32_t)STORAGE_IMAGE_WIDTH, (uint32_t)STORAGE_IMAGE_HEIGHT, (uint32_t)STORAGE_IMAGE_DEPTH };
		std::vector<VkFormat> userRequFormat;	userRequFormat.push_back(VK_FORMAT_R32_SFLOAT);
		createAllocBindImage(this->densityCopy, this->densityCopyMemory,
			VK_IMAGE_TYPE_3D, tempExtent, userRequFormat,
			VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);



		VkCommandBuffer tempComBuff;
		allocBeginComBuff(tempComBuff);

		cmdImgMemBarrier(tempComBuff, this->densityCopy,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			0, VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

		endSubmitFreeComBuff(tempComBuff);

	}
	void createDensityCopyView() {

		createImageView(this->densityCopyView, this->densityCopy, VK_FORMAT_R32_SFLOAT, VK_IMAGE_VIEW_TYPE_3D);
		
	}
	void createDensityCopySampler() {

		createImageSampler(this->densityCopySampler);

	}
	//
	
	void createVelocityDensityResources() {

		struct Vel {
			float x;
			float y;
			float z;
		};
		struct Voxel {
			Vel vel;
			float den;
		};

		VkDeviceSize image3Dcount = STORAGE_IMAGE_WIDTH * STORAGE_IMAGE_HEIGHT * STORAGE_IMAGE_DEPTH;
		VkDeviceSize image3Dsize = STORAGE_IMAGE_WIDTH * STORAGE_IMAGE_HEIGHT * STORAGE_IMAGE_DEPTH
								   * sizeof(Voxel);
		std::cout << sizeof(Voxel) << std::endl;
		
		std::vector<Voxel> pixels(image3Dcount, {{0.1f, 0.0f, 0.0f}, 0.0f});		// build�� clean���ϸ�, �������࿡�� �ʱ�ȣ�� ������ �״�� ���� �����࿡�� �̾�����;;  std::vector ������, �Ҵ�� �޸� �ּҰ� ������, �������࿡�� �ʱ�ȭ�� �����ϱ� ������ ���������ߴ� �����Ͱ� �̾����� ��!

		std::cout << "Test for velocity value in CPU: (";
		std::cout << pixels[ 2 
							 + (3 * STORAGE_IMAGE_WIDTH) 
							 + (4 * STORAGE_IMAGE_WIDTH * STORAGE_IMAGE_HEIGHT) ].vel.x;
		std::cout << ", ";
		std::cout << pixels[ 2
							 + (3 * STORAGE_IMAGE_WIDTH)
							 + (4 * STORAGE_IMAGE_WIDTH * STORAGE_IMAGE_HEIGHT)].vel.y;
		std::cout << ", ";
		std::cout << pixels[2
			+ (3 * STORAGE_IMAGE_WIDTH)
			+ (4 * STORAGE_IMAGE_WIDTH * STORAGE_IMAGE_HEIGHT)].vel.z;
		std::cout << ")" << std::endl;

		std::cout << "Test for density value in CPU: ";
		std::cout << pixels[2
							+ (3 * STORAGE_IMAGE_WIDTH)
							+ (4 * STORAGE_IMAGE_WIDTH * STORAGE_IMAGE_HEIGHT)].den;
		std::cout << std::endl;



		const int stagingBufferCopyCount = 4;
		VkDeviceSize stagingBufferCount = (image3Dcount / stagingBufferCopyCount);
		VkDeviceSize stagingBufferSize = (image3Dsize / stagingBufferCopyCount);
		//
		std::array<VkBuffer*, stagingBufferCopyCount> stagingBufferPtrs;
		std::array<VkDeviceMemory*, stagingBufferCopyCount> stagingBufferMemoryPtrs;
		std::array <void*, stagingBufferCopyCount> dataPtrs;
		//

		


		(this->velocityDensity).resize(MAX_FRAMES_IN_FLIGHT);
		(this->velocityDensityMemories).resize(MAX_FRAMES_IN_FLIGHT);
		VkExtent3D extent = { (uint32_t)STORAGE_IMAGE_WIDTH, (uint32_t)STORAGE_IMAGE_HEIGHT, (uint32_t)STORAGE_IMAGE_DEPTH };
		std::vector<VkFormat> userFormats = { VK_FORMAT_R32G32B32A32_SFLOAT,
											  VK_FORMAT_R32G32B32_SFLOAT,
											  VK_FORMAT_R16G16B16_SFLOAT,
											  VK_FORMAT_R16G16B16_SNORM,
											  VK_FORMAT_R32_SFLOAT,
											  VK_FORMAT_R16_SFLOAT };
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			createAllocBindImage((this->velocityDensity)[i], (this->velocityDensityMemories)[i],
				VK_IMAGE_TYPE_3D, extent, userFormats,
				VK_FORMAT_FEATURE_TRANSFER_DST_BIT | VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT);



			for (size_t j = 0; j < stagingBufferCopyCount; j++)
			{
				VkBuffer stagingBuffer0;
				VkDeviceMemory stagingBufferMemory0;
				void* dataPtr0 = nullptr;
				
				std::cout << "first try" << std::endl;

				createAllocBindBuffer(stagingBuffer0, stagingBufferMemory0, stagingBufferSize,
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
					| VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

				vkMapMemory(this->device, stagingBufferMemory0, 0, stagingBufferSize, 0, &dataPtr0);
				memcpy(dataPtr0, &pixels[(j * stagingBufferCount)], static_cast<size_t>(stagingBufferSize));
				vkUnmapMemory(this->device, stagingBufferMemory0);



				VkCommandBuffer tempComBuff;
				allocBeginComBuff(tempComBuff);

				cmdBuffMemBarrier(tempComBuff, stagingBuffer0,
					0, stagingBufferSize,
					0, VK_ACCESS_TRANSFER_READ_BIT,
					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
				cmdImgMemBarrier(tempComBuff, (this->velocityDensity[i]),
					VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					0, VK_ACCESS_TRANSFER_WRITE_BIT,
					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

				cmdCopyBufferToImage( tempComBuff, stagingBuffer0, this->velocityDensity[i],
					0,
					VkOffset3D{ (int32_t)0, (int32_t)0, (int32_t)(j * (STORAGE_IMAGE_DEPTH / stagingBufferCopyCount)) },
					VkExtent3D{ (uint32_t)STORAGE_IMAGE_WIDTH, (uint32_t)STORAGE_IMAGE_HEIGHT, (uint32_t)(STORAGE_IMAGE_DEPTH / stagingBufferCopyCount) } );

				//cmdBuffMemBarrier(tempComBuff, stagingBuffer0,
				//	0, stagingBufferSize,
				//	0, VK_ACCESS_TRANSFER_READ_BIT,
				//	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);
				cmdImgMemBarrier(tempComBuff, (this->velocityDensity[i]),
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL,
					VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_WRITE_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

				endSubmitFreeComBuff(tempComBuff);



				vkFreeMemory(this->device, stagingBufferMemory0, nullptr);
				vkDestroyBuffer(this->device, stagingBuffer0, nullptr);
			}
		}







		(this->velocityDensityViews).resize(static_cast<size_t>(MAX_FRAMES_IN_FLIGHT));
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			createImageView((this->velocityDensityViews)[i], (this->velocityDensity)[i], VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_3D);
		}



		createImageSampler(this->velocityDensitySampler);



		std::cout << "VelocityDensity resources are created!" << std::endl;

	}
	void createVelocityDensityTempResources() {

		VkDeviceSize image3Dsize = STORAGE_IMAGE_WIDTH * STORAGE_IMAGE_HEIGHT * STORAGE_IMAGE_DEPTH
			* RGBA_STORAGE_IMAGE_CHANNEL * sizeof(float);



		(this->velocityDensityTemp).resize(MAX_FRAMES_IN_FLIGHT);
		(this->velocityDensityTempMemories).resize(MAX_FRAMES_IN_FLIGHT);

		VkExtent3D extent = { (uint32_t)STORAGE_IMAGE_WIDTH, (uint32_t)STORAGE_IMAGE_HEIGHT, (uint32_t)STORAGE_IMAGE_DEPTH };
		std::vector<VkFormat> userFormats = { VK_FORMAT_R32G32B32A32_SFLOAT,
											  VK_FORMAT_R32G32B32_SFLOAT,
											  VK_FORMAT_R16G16B16_SFLOAT,
											  VK_FORMAT_R16G16B16_SNORM,
											  VK_FORMAT_R32_SFLOAT,
											  VK_FORMAT_R16_SFLOAT };

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			createAllocBindImage((this->velocityDensityTemp)[i], (this->velocityDensityTempMemories)[i],
				VK_IMAGE_TYPE_3D, extent, userFormats,
				VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT,
				VK_IMAGE_USAGE_STORAGE_BIT);

			VkCommandBuffer tempComBuff;
			allocBeginComBuff(tempComBuff);

			cmdImgMemBarrier(tempComBuff, (this->velocityDensityTemp[i]),
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
				0, VK_ACCESS_SHADER_READ_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

			endSubmitFreeComBuff(tempComBuff);
		}



		(this->velocityDensityTempViews).resize(static_cast<size_t>(MAX_FRAMES_IN_FLIGHT));
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			createImageView((this->velocityDensityTempViews)[i], (this->velocityDensityTemp)[i], VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_3D);
		}	std::cout << "VelocityDensityTemp resources are created!" << std::endl;

	}
	void createDivergencePressureResources() {

		VkDeviceSize image3Dsize = STORAGE_IMAGE_WIDTH * STORAGE_IMAGE_HEIGHT * STORAGE_IMAGE_DEPTH
			* RGBA_STORAGE_IMAGE_CHANNEL * sizeof(float);



		(this->divergencePressure).resize(MAX_FRAMES_IN_FLIGHT);
		(this->divergencePressureMemories).resize(MAX_FRAMES_IN_FLIGHT);

		VkExtent3D extent = { (uint32_t)STORAGE_IMAGE_WIDTH, (uint32_t)STORAGE_IMAGE_HEIGHT, (uint32_t)STORAGE_IMAGE_DEPTH };
		std::vector<VkFormat> userFormats = { VK_FORMAT_R32G32B32A32_SFLOAT,
											  VK_FORMAT_R32G32B32_SFLOAT,
											  VK_FORMAT_R16G16B16_SFLOAT,
											  VK_FORMAT_R16G16B16_SNORM,
											  VK_FORMAT_R32_SFLOAT,
											  VK_FORMAT_R16_SFLOAT };

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			createAllocBindImage((this->divergencePressure)[i], (this->divergencePressureMemories)[i],
				VK_IMAGE_TYPE_3D, extent, userFormats,
				VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT,
				VK_IMAGE_USAGE_STORAGE_BIT);

			VkCommandBuffer tempComBuff;
			allocBeginComBuff(tempComBuff);
				cmdImgMemBarrier(tempComBuff, (this->divergencePressure[i]),
					VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
					0, VK_ACCESS_SHADER_READ_BIT,
					VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
			endSubmitFreeComBuff(tempComBuff);
		}



		(this->divergencePressureViews).resize(static_cast<size_t>(MAX_FRAMES_IN_FLIGHT));
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			createImageView((this->divergencePressureViews)[i], (this->divergencePressure)[i], VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_3D);
		}	std::cout << "divergencePressure resources are created!" << std::endl;

	}
	void createDivergencePressureTempResources() {
		
		VkDeviceSize image3Dsize = STORAGE_IMAGE_WIDTH * STORAGE_IMAGE_HEIGHT * STORAGE_IMAGE_DEPTH
			* RGBA_STORAGE_IMAGE_CHANNEL * sizeof(float);



		(this->divergencePressureTemp).resize(MAX_FRAMES_IN_FLIGHT);
		(this->divergencePressureTempMemories).resize(MAX_FRAMES_IN_FLIGHT);

		VkExtent3D extent = { (uint32_t)STORAGE_IMAGE_WIDTH, (uint32_t)STORAGE_IMAGE_HEIGHT, (uint32_t)STORAGE_IMAGE_DEPTH };
		std::vector<VkFormat> userFormats = { VK_FORMAT_R32G32B32A32_SFLOAT,
											  VK_FORMAT_R32G32B32_SFLOAT,
											  VK_FORMAT_R16G16B16_SFLOAT,
											  VK_FORMAT_R16G16B16_SNORM,
											  VK_FORMAT_R32_SFLOAT,
											  VK_FORMAT_R16_SFLOAT };

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			createAllocBindImage((this->divergencePressureTemp)[i], (this->divergencePressureTempMemories)[i],
				VK_IMAGE_TYPE_3D, extent, userFormats,
				VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT,
				VK_IMAGE_USAGE_STORAGE_BIT);

			VkCommandBuffer tempComBuff;
			allocBeginComBuff(tempComBuff);

			cmdImgMemBarrier(tempComBuff, (this->divergencePressureTemp[i]),
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
				0, VK_ACCESS_SHADER_WRITE_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

			endSubmitFreeComBuff(tempComBuff);
		}



		(this->divergencePressureTempViews).resize(static_cast<size_t>(MAX_FRAMES_IN_FLIGHT));
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			createImageView((this->divergencePressureTempViews)[i], (this->divergencePressureTemp)[i], VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_3D);
		}	std::cout << "divergencePressureTemp resources are created!" << std::endl;

	}
	void createBCResources() {
	
		VkDeviceSize image3Dsize = STORAGE_IMAGE_WIDTH * STORAGE_IMAGE_HEIGHT * STORAGE_IMAGE_DEPTH
								   * RGBA_STORAGE_IMAGE_CHANNEL * sizeof(float);



		//(this->BC).resize(MAX_FRAMES_IN_FLIGHT);
		//(this->BCMemories).resize(MAX_FRAMES_IN_FLIGHT);

		VkExtent3D extent = { (uint32_t)STORAGE_IMAGE_WIDTH, (uint32_t)STORAGE_IMAGE_HEIGHT, (uint32_t)STORAGE_IMAGE_DEPTH };
		std::vector<VkFormat> userFormats = { VK_FORMAT_R8_SINT };

		//for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			createAllocBindImage(this->BC, this->BCMemories,
						VK_IMAGE_TYPE_3D, extent, userFormats,
						VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT,
						VK_IMAGE_USAGE_STORAGE_BIT);

			VkCommandBuffer tempComBuff;
			allocBeginComBuff(tempComBuff);

			cmdImgMemBarrier(tempComBuff, (this->BC),
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
				0, VK_ACCESS_SHADER_READ_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

			endSubmitFreeComBuff(tempComBuff);
		//}



		//(this->BCViews).resize(static_cast<size_t>(MAX_FRAMES_IN_FLIGHT));
		//for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		//{
			createImageView(this->BCViews, this->BC, VK_FORMAT_R8_SINT, VK_IMAGE_VIEW_TYPE_3D);
		//}	
		std::cout << "divergencePressureTemp resources are created!" << std::endl;

	}
	void createVorticityResources() {
		
		(this->vorticity).resize(MAX_FRAMES_IN_FLIGHT);
		(this->vorticityMemories).resize(MAX_FRAMES_IN_FLIGHT);

		VkExtent3D extent = { (uint32_t)STORAGE_IMAGE_WIDTH, (uint32_t)STORAGE_IMAGE_HEIGHT, (uint32_t)STORAGE_IMAGE_DEPTH };
		std::vector<VkFormat> userFormats = { VK_FORMAT_R32G32B32A32_SFLOAT,
											  VK_FORMAT_R32G32B32_SFLOAT,
											  VK_FORMAT_R16G16B16_SFLOAT,
											  VK_FORMAT_R16G16B16_SNORM,
											  VK_FORMAT_R32_SFLOAT,
											  VK_FORMAT_R16_SFLOAT };

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			createAllocBindImage((this->vorticity)[i], (this->vorticityMemories)[i],
				VK_IMAGE_TYPE_3D, extent, userFormats,
				VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT,
				VK_IMAGE_USAGE_STORAGE_BIT);

			VkCommandBuffer tempComBuff;
			allocBeginComBuff(tempComBuff);

			cmdImgMemBarrier(tempComBuff, (this->vorticity[i]),
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
				0, VK_ACCESS_SHADER_READ_BIT,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

			endSubmitFreeComBuff(tempComBuff);
		}



		(this->vorticityViews).resize(static_cast<size_t>(MAX_FRAMES_IN_FLIGHT));
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			createImageView((this->vorticityViews)[i], (this->vorticity)[i], VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_VIEW_TYPE_3D);
		}	std::cout << "vorticity resources are created!" << std::endl;
	}
	//
	void createVertexBuffer() {

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		VkDeviceSize vertexBufferSize = sizeof(BoxParticle) * static_cast<VkDeviceSize>(BOX_PARTICLE_COUNT);

		VkBufferCreateInfo stagingBuffCreateInfo{};
		stagingBuffCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBuffCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBuffCreateInfo.size = static_cast<VkDeviceSize>(vertexBufferSize);
		stagingBuffCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(this->device, &stagingBuffCreateInfo, nullptr, &stagingBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create staging buffer for storageImage.");
		}	std::cout << "staging buffer for Vertex Buffer is created." << std::endl;

		VkMemoryRequirements memRequirements{};
		vkGetBufferMemoryRequirements(this->device, stagingBuffer, &memRequirements);

		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memRequirements.size;
		memAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			| VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

		if (vkAllocateMemory(this->device, &memAllocInfo, nullptr, &stagingBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate staging buffer moemory for storageImage.");
		}	std::cout << "staging buffer memory for Vertex Buffer is allocated." << std::endl;

		if (vkBindBufferMemory(this->device, stagingBuffer, stagingBufferMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate staging buffer moemory for storageImage.");
		}	std::cout << "staging buffer memory for Vertex Buffer is bound." << std::endl;



		void* data;
		vkMapMemory(this->device, stagingBufferMemory, 0, vertexBufferSize, 0, &data);
		memcpy(data, boxParticles, static_cast<size_t>(vertexBufferSize));
		vkUnmapMemory(this->device, stagingBufferMemory);



		VkBufferCreateInfo vertexBufferInfo{};
		vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		vertexBufferInfo.size = vertexBufferSize;
		vertexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(this->device, &vertexBufferInfo, nullptr, &(this->vertexBuffer)) != VK_SUCCESS) {
			throw std::runtime_error("failed to create Vertex Buffer.");
		}	std::cout << "Vertex Buffer is created." << std::endl;



		//VkMemoryRequirements 
		memRequirements = {};
		vkGetBufferMemoryRequirements(this->device, this->vertexBuffer, &memRequirements);

		VkMemoryAllocateInfo vertexBufferMemAllocInfo{};
		vertexBufferMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vertexBufferMemAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		vertexBufferMemAllocInfo.allocationSize = memRequirements.size;

		if (vkAllocateMemory(this->device, &vertexBufferMemAllocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate memory to Vertex Buffer.");
		}	std::cout << "Vertex Buffer memory is allocated." << std::endl;

		vkBindBufferMemory(this->device, this->vertexBuffer, this->vertexBufferMemory, 0);



		copyBuffer_(stagingBuffer, this->vertexBuffer, vertexBufferSize);



		vkFreeMemory(this->device, stagingBufferMemory, nullptr);
		vkDestroyBuffer(this->device, stagingBuffer, nullptr);
	}
	void createIndexBuffer() {

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		VkDeviceSize boxIndicesSize = sizeof(uint16_t) * boxIndices.size();

		VkBufferCreateInfo stagingBufferInfo{};
		stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		//		
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBufferInfo.size = boxIndicesSize;
		//
		stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// sharingMode = VK_SHARING_MODE_EXCLUSIVE �� ��쿡��, 
		// �Ʒ� vertexBufferInfo.queueFamilyIndexCount ��, vertexBufferInfo.pQueueFamilyIndices �� �Ƚᵵ �ȴ�.
		// If sharingMode is VK_SHARING_MODE_CONCURRENT, queueFamilyIndexCount must be greater than 1
		//stagingBufferInfo.queueFamilyIndexCount = 1;
		// If sharingMode is VK_SHARING_MODE_CONCURRENT, 
		// pQueueFamilyIndices must be a valid pointer to an array of queueFamilyIndexCount uint32_t values
		//uint32_t graphicsQFIndex = this->QFIndices.graphicsQueueFamilyIndex.value();
		//stagingBufferInfo.pQueueFamilyIndices = &graphicsQFIndex;

		if (vkCreateBuffer(this->device, &stagingBufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create staging buffer for Index Buffer.");
		}	std::cout << "staging buffer for Index Buffer is created." << std::endl;



		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(this->device, stagingBuffer, &memRequirements);

		VkMemoryAllocateInfo stagingBufferMemAllocInfo{};
		stagingBufferMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		stagingBufferMemAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBufferMemAllocInfo.allocationSize = memRequirements.size;

		if (vkAllocateMemory(this->device, &stagingBufferMemAllocInfo, nullptr, &stagingBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate memory for Index Buffer.");
		}	std::cout << "staging buffer memory for Index Buffer is allocated." << std::endl;



		vkBindBufferMemory(this->device, stagingBuffer, stagingBufferMemory, 0);



		void* data;
		vkMapMemory(this->device, stagingBufferMemory, 0, boxIndicesSize, 0, &data);
		memcpy(data, boxIndices.data(), (size_t)boxIndicesSize);
		vkUnmapMemory(this->device, stagingBufferMemory);



		VkBufferCreateInfo indexBufferInfo{};
		indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		//		
		indexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		indexBufferInfo.size = boxIndicesSize;
		//
		indexBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// sharingMode = VK_SHARING_MODE_EXCLUSIVE �� ��쿡��, 
		// �Ʒ� vertexBufferInfo.queueFamilyIndexCount ��, vertexBufferInfo.pQueueFamilyIndices �� �Ƚᵵ �ȴ�.
		// If sharingMode is VK_SHARING_MODE_CONCURRENT, queueFamilyIndexCount must be greater than 1
		//vertexBuffer2Info.queueFamilyIndexCount = 1;
		// If sharingMode is VK_SHARING_MODE_CONCURRENT, 
		// pQueueFamilyIndices must be a valid pointer to an array of queueFamilyIndexCount uint32_t values
		//uint32_t graphicsQFIndex = this->QFIndices.graphicsQueueFamilyIndex.value();
		//vertexBuffer2Info.pQueueFamilyIndices = &graphicsQFIndex;

		if (vkCreateBuffer(this->device, &indexBufferInfo, nullptr, &(this->indexBuffer)) != VK_SUCCESS) {
			throw std::runtime_error("failed to create Index Buffer.");
		}	std::cout << "Index Buffer is created." << std::endl;



		VkMemoryRequirements memRequirements2;
		vkGetBufferMemoryRequirements(this->device, this->indexBuffer, &memRequirements2);

		VkMemoryAllocateInfo indexBufferMemAllocInfo{};
		indexBufferMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		indexBufferMemAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements2.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		indexBufferMemAllocInfo.allocationSize = memRequirements2.size;

		if (vkAllocateMemory(this->device, &indexBufferMemAllocInfo, nullptr, &(this->indexBufferMemory)) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate memory for Index buffer.");
		}	std::cout << "Index buffer memory is allocated." << std::endl;



		if (vkBindBufferMemory(this->device, this->indexBuffer, this->indexBufferMemory, 0)
			 != VK_SUCCESS) {
			throw std::runtime_error("failed to bind Index buffer and its memory.");
		}	std::cout << "Index buffer and its memory is bounded." << std::endl;



		// The vertexBuffer is now allocated from a memory type that is device local, 
		// which generally means that we're not able to use vkMapMemory. 
		// However, we can copy data from the stagingBuffer to the vertexBuffer, using commmand buffer. 
		copyBuffer_(stagingBuffer, this->indexBuffer, boxIndicesSize);

		vkFreeMemory(this->device, stagingBufferMemory, nullptr);
		vkDestroyBuffer(this->device, stagingBuffer, nullptr);

	}
	void createIndexBuffer2() {

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		VkDeviceSize boxLineIndicesSize = sizeof(uint16_t) * boxLineIndices.size();

		VkBufferCreateInfo stagingBufferInfo{};
		stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		//		
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBufferInfo.size = boxLineIndicesSize;
		//
		stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// sharingMode = VK_SHARING_MODE_EXCLUSIVE �� ��쿡��, 
		// �Ʒ� vertexBufferInfo.queueFamilyIndexCount ��, vertexBufferInfo.pQueueFamilyIndices �� �Ƚᵵ �ȴ�.
		// If sharingMode is VK_SHARING_MODE_CONCURRENT, queueFamilyIndexCount must be greater than 1
		//stagingBufferInfo.queueFamilyIndexCount = 1;
		// If sharingMode is VK_SHARING_MODE_CONCURRENT, 
		// pQueueFamilyIndices must be a valid pointer to an array of queueFamilyIndexCount uint32_t values
		//uint32_t graphicsQFIndex = this->QFIndices.graphicsQueueFamilyIndex.value();
		//stagingBufferInfo.pQueueFamilyIndices = &graphicsQFIndex;

		if (vkCreateBuffer(this->device, &stagingBufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create staging buffer for Index Buffer.");
		}	std::cout << "staging buffer for Index Buffer is created." << std::endl;



		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(this->device, stagingBuffer, &memRequirements);

		VkMemoryAllocateInfo stagingBufferMemAllocInfo{};
		stagingBufferMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		stagingBufferMemAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBufferMemAllocInfo.allocationSize = memRequirements.size;

		if (vkAllocateMemory(this->device, &stagingBufferMemAllocInfo, nullptr, &stagingBufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate memory for Index Buffer.");
		}	std::cout << "staging buffer memory for Index Buffer is allocated." << std::endl;



		vkBindBufferMemory(this->device, stagingBuffer, stagingBufferMemory, 0);



		void* data;
		vkMapMemory(this->device, stagingBufferMemory, 0, boxLineIndicesSize, 0, &data);
		memcpy(data, boxLineIndices.data(), (size_t)boxLineIndicesSize);
		vkUnmapMemory(this->device, stagingBufferMemory);



		VkBufferCreateInfo indexBuffer2Info{};
		indexBuffer2Info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		//		
		indexBuffer2Info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		indexBuffer2Info.size = boxLineIndicesSize;
		//
		indexBuffer2Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// sharingMode = VK_SHARING_MODE_EXCLUSIVE �� ��쿡��, 
		// �Ʒ� vertexBufferInfo.queueFamilyIndexCount ��, vertexBufferInfo.pQueueFamilyIndices �� �Ƚᵵ �ȴ�.
		// If sharingMode is VK_SHARING_MODE_CONCURRENT, queueFamilyIndexCount must be greater than 1
		//indexBuffer2Info.queueFamilyIndexCount = 1;
		// If sharingMode is VK_SHARING_MODE_CONCURRENT, 
		// pQueueFamilyIndices must be a valid pointer to an array of queueFamilyIndexCount uint32_t values
		//uint32_t graphicsQFIndex = this->QFIndices.graphicsQueueFamilyIndex.value();
		//indexBuffer2Info.pQueueFamilyIndices = &graphicsQFIndex;

		if (vkCreateBuffer(this->device, &indexBuffer2Info, nullptr, &(this->indexBuffer2)) != VK_SUCCESS) {
			throw std::runtime_error("failed to create Index Buffer.");
		}	std::cout << "Index Buffer is created." << std::endl;



		VkMemoryRequirements memRequirements2;
		vkGetBufferMemoryRequirements(this->device, this->indexBuffer2, &memRequirements2);

		VkMemoryAllocateInfo indexBufferMemAllocInfo{};
		indexBufferMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		indexBufferMemAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements2.memoryTypeBits,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		indexBufferMemAllocInfo.allocationSize = memRequirements2.size;

		if (vkAllocateMemory(this->device, &indexBufferMemAllocInfo, nullptr, &(this->indexBuffer2Memory)) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate memory for Index buffer 2.");
		}	std::cout << "Index buffer 2 memory is allocated." << std::endl;



		if (vkBindBufferMemory(this->device, this->indexBuffer2, this->indexBuffer2Memory, 0)
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to bind Index buffer 2 and its memory.");
		}	std::cout << "Index buffer 2 and its memory is bounded." << std::endl;



		// The vertexBuffer is now allocated from a memory type that is device local, 
		// which generally means that we're not able to use vkMapMemory. 
		// However, we can copy data from the stagingBuffer to the vertexBuffer, using commmand buffer. 
		copyBuffer_(stagingBuffer, this->indexBuffer2, boxLineIndicesSize);

		vkFreeMemory(this->device, stagingBufferMemory, nullptr);
		vkDestroyBuffer(this->device, stagingBuffer, nullptr);

	}
	//
	void createDescriptorSetLayout() {

		VkDescriptorSetLayoutBinding uboDescSetLayoutBinding{};
		uboDescSetLayoutBinding.binding = 0;
		uboDescSetLayoutBinding.descriptorCount = 1;
		uboDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboDescSetLayoutBinding.pImmutableSamplers = nullptr;
		uboDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS | VK_SHADER_STAGE_COMPUTE_BIT;
		// If descriptorType specifies a VK_DESCRIPTOR_TYPE_SAMPLER or VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER type descriptor, 
		// then pImmutableSamplers can be used to initialize a set of immutable samplers. 
		// Immutable samplers are permanently bound into the set layout and must not be changed
		//uboDescSetLayoutBinding.pImmutableSamplers = VK_NULL_HANDLE;




		// The reason why we have two layout bindings for shader storage buffer objects
		// is because each frame needs to know about the last frames' particle positions
		//			Frame0		Frame1		Frame2		...
		//	read	SSBO[1]		SSBO[0]		SSBO[1]		...
		//	write	SSBO[0]		SSBO[1]		SSBO[0]		...
		//
		VkDescriptorSetLayoutBinding lastDensityDescSetLayoutBinding{};
		lastDensityDescSetLayoutBinding.binding = 1;
		lastDensityDescSetLayoutBinding.descriptorCount = 1;
		lastDensityDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		lastDensityDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// �Ʒ� �Ķ���ʹ�, which pipeline shader stages can access a resource for this binding, �� �ǹ���.
		lastDensityDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding currentDensityDescSetLayoutBinding{};
		currentDensityDescSetLayoutBinding.binding = 2;
		currentDensityDescSetLayoutBinding.descriptorCount = 1;
		currentDensityDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		currentDensityDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// �Ʒ� �Ķ���ʹ�, which pipeline shader stages can access a resource for this binding, �� �ǹ���.
		currentDensityDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	
		VkDescriptorSetLayoutBinding densityCopiesDescSetLayoutBinding{};
		densityCopiesDescSetLayoutBinding.binding = 3;
		densityCopiesDescSetLayoutBinding.descriptorCount = 1;
		densityCopiesDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		densityCopiesDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding densityCopySamplerDescSetLayoutBinding{};
		densityCopySamplerDescSetLayoutBinding.binding = 4;
		densityCopySamplerDescSetLayoutBinding.descriptorCount = 1;
		densityCopySamplerDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		// �Ʒ��� Ư�� sampler�� imageDescriptor�� ���ӽ�Ű�� ������, ���� �Ķ�����ε�. nullptr�̸�, �ش� descriptorSet�ȿ� �ִ� ��� image descriptor��, sampler�� �����ִµ�.
		densityCopySamplerDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// �Ʒ� �Ķ���ʹ�, which pipeline shader stages can access a resource for this binding, �� �ǹ���.
		densityCopySamplerDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;



		VkDescriptorSetLayoutBinding lastVelocityDensityDescSetLayoutBinding{};
		lastVelocityDensityDescSetLayoutBinding.binding = 5;
		lastVelocityDensityDescSetLayoutBinding.descriptorCount = 1;
		lastVelocityDensityDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		lastVelocityDensityDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// �Ʒ� �Ķ���ʹ�, which pipeline shader stages can access a resource for this binding, �� �ǹ���.
		lastVelocityDensityDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding currentVelocityDensityDescSetLayoutBinding{};
		currentVelocityDensityDescSetLayoutBinding.binding = 6;
		currentVelocityDensityDescSetLayoutBinding.descriptorCount = 1;
		currentVelocityDensityDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		currentVelocityDensityDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// �Ʒ� �Ķ���ʹ�, which pipeline shader stages can access a resource for this binding, �� �ǹ���.
		currentVelocityDensityDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding velocityDensityTempDescSetLayoutBinding{};
		velocityDensityTempDescSetLayoutBinding.binding = 7;
		velocityDensityTempDescSetLayoutBinding.descriptorCount = 1;
		velocityDensityTempDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		velocityDensityTempDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// �Ʒ� �Ķ���ʹ�, which pipeline shader stages can access a resource for this binding, �� �ǹ���.
		velocityDensityTempDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding lastDivergencePressureDescSetLayoutBinding{};
		lastDivergencePressureDescSetLayoutBinding.binding = 8;
		lastDivergencePressureDescSetLayoutBinding.descriptorCount = 1;
		lastDivergencePressureDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		lastDivergencePressureDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// �Ʒ� �Ķ���ʹ�, which pipeline shader stages can access a resource for this binding, �� �ǹ���.
		lastDivergencePressureDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding currentDivergencePressureDescSetLayoutBinding{};
		currentDivergencePressureDescSetLayoutBinding.binding = 9;
		currentDivergencePressureDescSetLayoutBinding.descriptorCount = 1;
		currentDivergencePressureDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		currentDivergencePressureDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// �Ʒ� �Ķ���ʹ�, which pipeline shader stages can access a resource for this binding, �� �ǹ���.
		currentDivergencePressureDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding divergencePressureTempDescSetLayoutBinding{};
		divergencePressureTempDescSetLayoutBinding.binding = 10;
		divergencePressureTempDescSetLayoutBinding.descriptorCount = 1;
		divergencePressureTempDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		divergencePressureTempDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// �Ʒ� �Ķ���ʹ�, which pipeline shader stages can access a resource for this binding, �� �ǹ���.
		divergencePressureTempDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding bcDescSetLayoutBinding{};
		bcDescSetLayoutBinding.binding = 11;
		bcDescSetLayoutBinding.descriptorCount = 1;
		bcDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		bcDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// �Ʒ� �Ķ���ʹ�, which pipeline shader stages can access a resource for this binding, �� �ǹ���.
		bcDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding vorticityDescSetLayoutBinding{};
		vorticityDescSetLayoutBinding.binding = 12;
		vorticityDescSetLayoutBinding.descriptorCount = 1;
		vorticityDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		vorticityDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// �Ʒ� �Ķ���ʹ�, which pipeline shader stages can access a resource for this binding, �� �ǹ���.
		vorticityDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;



		// �⺻������ std::array = �� �����ؼ� ���� �ֳ�.
		std::array<VkDescriptorSetLayoutBinding, 13> DescSetLayoutBindings = { uboDescSetLayoutBinding,
																			  lastDensityDescSetLayoutBinding,
																			  currentDensityDescSetLayoutBinding,
																			  densityCopiesDescSetLayoutBinding,
																			  densityCopySamplerDescSetLayoutBinding,
																			  lastVelocityDensityDescSetLayoutBinding,
																			  currentVelocityDensityDescSetLayoutBinding,
																			  velocityDensityTempDescSetLayoutBinding,
																			  lastDivergencePressureDescSetLayoutBinding,
																			  currentDivergencePressureDescSetLayoutBinding,
																			  divergencePressureTempDescSetLayoutBinding,
																			  bcDescSetLayoutBinding,
																			  vorticityDescSetLayoutBinding
																			};

		VkDescriptorSetLayoutCreateInfo descSetLayoutCreateInfo{};
		descSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(DescSetLayoutBindings.size());
		descSetLayoutCreateInfo.pBindings = DescSetLayoutBindings.data();

		if ( vkCreateDescriptorSetLayout(this->device, &descSetLayoutCreateInfo, nullptr, &(this->descriptorSetLayout))
			 != VK_SUCCESS )
		{
			throw std::runtime_error("failed to create descriptor set layout.");
		}	std::cout << "Descriptor set layout has been created." << std::endl;

	}
	void createDescriptorPool() {

		VkDescriptorPoolSize poolSize0{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize0.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize0.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);		// ���� DescriptorSet�� ���ļ�, �� ������ Set�� ������ Descriptor ������ �� ���ؼ�, �������� ������.

		VkDescriptorPoolSize poolSize1{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize1.type = VK_DESCRIPTOR_TYPE_SAMPLER;
		poolSize1.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize2{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize2.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize2.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize3{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize3.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		poolSize3.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize4{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize4.type = VK_DESCRIPTOR_TYPE_SAMPLER;
		poolSize4.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);



		VkDescriptorPoolSize poolSize5{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize5.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize5.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize6{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize6.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize6.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize7{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize7.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize7.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize8{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize8.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize8.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize9{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize9.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize9.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize10{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize10.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize10.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize11{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize11.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize11.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize12{};	// �� binding��������, poolSize�� �ѹ��� �����Ѵ�.
		poolSize12.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize12.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		std::array<VkDescriptorPoolSize, 13> poolSizeInfos = { poolSize0, poolSize1, 
															  poolSize2, poolSize3, 
															  poolSize4,
															  poolSize5, poolSize6,
															  poolSize7, poolSize8,
															  poolSize9, poolSize10,
															  poolSize11,  
															  poolSize12 };

		VkDescriptorPoolCreateInfo descPoolCreateInfo{};
		descPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizeInfos.size());
		descPoolCreateInfo.pPoolSizes = poolSizeInfos.data();							// ���� �����ϸ�, pool���� descriptor�� ����.
		descPoolCreateInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);		// ���� �����ϸ�, pool���� descriptorSet�� ����.

		if (vkCreateDescriptorPool(this->device, &descPoolCreateInfo, nullptr, &(this->descriptorPool)) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool.");
		}	std::cout << "Descriptor pool is created." << std::endl;

	}
	void createDescriptorSets() {

		(this->descriptorSets).resize(MAX_FRAMES_IN_FLIGHT);

		std::vector<VkDescriptorSetLayout> descSetLayouts(MAX_FRAMES_IN_FLIGHT, this->descriptorSetLayout);

		VkDescriptorSetAllocateInfo descSetAllocInfo{};
		descSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descSetAllocInfo.descriptorPool = this->descriptorPool;
		descSetAllocInfo.descriptorSetCount = static_cast<uint32_t>(descSetLayouts.size());
		descSetAllocInfo.pSetLayouts = descSetLayouts.data();

		if (vkAllocateDescriptorSets(this->device, &descSetAllocInfo, (this->descriptorSets).data())
			!= VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets.");
		}	std::cout << "descriptor sets are allocated." << std::endl;
	}
	void createDescriptors() {

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo descUniforBuffInfo{};		
			descUniforBuffInfo.buffer = this->uniformBuffers[i];
			descUniforBuffInfo.offset = 0;
			descUniforBuffInfo.range = sizeof(UniformBufferObject);		// = sizeof(UniforBufferObject); or = VK_WHOLE_SIZE ;
			// sizeof(this->uniformBuffers[i]) �� �ϸ� �ȉ�! 
			// ������� ���۰�ü(�����Ǻ���)�� ũ���, ���۰�ü�� �����ϴ�(����Ű��?) �޸��� ũ��.�� ���� �ٸ����̴�!
			// �׸��� �޸��� ũ���, �Էµ� �������� ũ�� ���� �ٸ���.
			// ���⼭�� �Էµ� �������� ũ��μ�, sizeof(UniforBufferObject)�� ��� ��Ȯ�ϴ�.

			VkDescriptorImageInfo descSamplerInfo{};
			descSamplerInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			//descSamplerInfo.imageView = this->densityView;
			descSamplerInfo.sampler = this->velocityDensitySampler;

			VkDescriptorImageInfo descDensityInfo{};
			descDensityInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			descDensityInfo.imageView = this->densityView;
			//descDensityInfo.sampler = this->densityCopySamplers[i];

			VkDescriptorImageInfo descDensityCopiesInfo{};
			descDensityCopiesInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descDensityCopiesInfo.imageView = this->densityCopyView;
			descDensityCopiesInfo.sampler = this->densityCopySampler;

			VkDescriptorImageInfo descDensityCopySamplersInfo{};
			descDensityCopySamplersInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			// Descriptor info only references the sampler.
			//descStorageSamplerInfo.imageView = this->storageImageViews[(i - 1) % MAX_FRAMES_IN_FLIGHT];
			descDensityCopySamplersInfo.sampler = this->densityCopySampler;



			VkDescriptorImageInfo descLastVelocityDensityInfo{};
			descLastVelocityDensityInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			descLastVelocityDensityInfo.imageView = this->velocityDensityViews[(i - 1) % MAX_FRAMES_IN_FLIGHT];

			VkDescriptorImageInfo descCurrentVelocityDensityInfo{};
			descCurrentVelocityDensityInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			descCurrentVelocityDensityInfo.imageView = this->velocityDensityViews[i];

			VkDescriptorImageInfo descVelocityDensityTempInfo{};
			descVelocityDensityTempInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			descVelocityDensityTempInfo.imageView = this->velocityDensityTempViews[i];

			VkDescriptorImageInfo descLastDivergencePressureInfo{};
			descLastDivergencePressureInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			descLastDivergencePressureInfo.imageView = this->divergencePressureViews[(i - 1) % MAX_FRAMES_IN_FLIGHT];

			VkDescriptorImageInfo descCurrentDivergencePressureInfo{};
			descCurrentDivergencePressureInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			descCurrentDivergencePressureInfo.imageView = this->divergencePressureViews[i];

			VkDescriptorImageInfo descDivergencePressureTempInfo{};
			descDivergencePressureTempInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			descDivergencePressureTempInfo.imageView = this->divergencePressureTempViews[i];

			VkDescriptorImageInfo descBCInfo{};
			descBCInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			descBCInfo.imageView = this->BCViews;

			VkDescriptorImageInfo descVorticityInfo{};
			descVorticityInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			descVorticityInfo.imageView = this->vorticityViews[i];






			VkWriteDescriptorSet descUniforBuffWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descUniforBuffWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// the destination descriptor set to update.
			descUniforBuffWriteInfo.dstSet = this->descriptorSets[i];
			descUniforBuffWriteInfo.dstBinding = 0;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descUniforBuffWriteInfo.dstArrayElement = 0;
			// The descriptorCount field specifies how many array elements you want to update.
			descUniforBuffWriteInfo.descriptorCount = 1;
			descUniforBuffWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			// These three fields below depend on the type of desscriptor which one of the three you actually need to use.
			descUniforBuffWriteInfo.pBufferInfo = &descUniforBuffInfo;
			descUniforBuffWriteInfo.pImageInfo = nullptr;
			descUniforBuffWriteInfo.pTexelBufferView = nullptr;

			VkWriteDescriptorSet descLastDensityWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descLastDensityWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// the destination descriptor set to update.
			descLastDensityWriteInfo.dstSet = this->descriptorSets[i];
			descLastDensityWriteInfo.dstBinding = 1;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descLastDensityWriteInfo.dstArrayElement = 0;
			// The descriptorCount field specifies how many array elements you want to update.
			descLastDensityWriteInfo.descriptorCount = 1;
			descLastDensityWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			// These three fields below depend on the type of desscriptor which one of the three you actually need to use.
			descLastDensityWriteInfo.pBufferInfo = nullptr;
			descLastDensityWriteInfo.pImageInfo = &descSamplerInfo;
			descLastDensityWriteInfo.pTexelBufferView = nullptr;
			descLastDensityWriteInfo.pNext = nullptr;

			VkWriteDescriptorSet descCurrentDensityWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descCurrentDensityWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// the destination descriptor set to update.
			descCurrentDensityWriteInfo.dstSet = this->descriptorSets[i];
			descCurrentDensityWriteInfo.dstBinding = 2;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descCurrentDensityWriteInfo.dstArrayElement = 0;
			// The descriptorCount field specifies how many array elements you want to update.
			descCurrentDensityWriteInfo.descriptorCount = 1;
			descCurrentDensityWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			// These three fields below depend on the type of desscriptor which one of the three you actually need to use.
			descCurrentDensityWriteInfo.pBufferInfo = nullptr;
			descCurrentDensityWriteInfo.pImageInfo = &descDensityInfo;
			descCurrentDensityWriteInfo.pTexelBufferView = nullptr;

			VkWriteDescriptorSet descDensityCopiesWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descDensityCopiesWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// the destination descriptor set to update.
			descDensityCopiesWriteInfo.dstSet = this->descriptorSets[i];
			descDensityCopiesWriteInfo.dstBinding = 3;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descDensityCopiesWriteInfo.dstArrayElement = 0;
			// The descriptorCount field specifies how many array elements you want to update.
			descDensityCopiesWriteInfo.descriptorCount = 1;
			descDensityCopiesWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			// These three fields below depend on the type of desscriptor which one of the three you actually need to use.
			descDensityCopiesWriteInfo.pBufferInfo = nullptr;
			descDensityCopiesWriteInfo.pImageInfo = &descDensityCopiesInfo;
			descDensityCopiesWriteInfo.pTexelBufferView = nullptr;

			VkWriteDescriptorSet descDensityCopySamplersWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descDensityCopySamplersWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			// the destination descriptor set to update.
			descDensityCopySamplersWriteInfo.dstSet = this->descriptorSets[i];
			descDensityCopySamplersWriteInfo.dstBinding = 4;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descDensityCopySamplersWriteInfo.dstArrayElement = 0;
			// The descriptorCount field specifies how many array elements you want to update.
			descDensityCopySamplersWriteInfo.descriptorCount = 1;
			descDensityCopySamplersWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			// These three fields below depend on the type of desscriptor which one of the three you actually need to use.
			descDensityCopySamplersWriteInfo.pBufferInfo = nullptr;
			descDensityCopySamplersWriteInfo.pImageInfo = &descDensityCopySamplersInfo;
			descDensityCopySamplersWriteInfo.pTexelBufferView = nullptr;



			VkWriteDescriptorSet descLastVelocityDensityWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descLastVelocityDensityWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descLastVelocityDensityWriteInfo.dstSet = this->descriptorSets[i];
			descLastVelocityDensityWriteInfo.dstBinding = 5;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descLastVelocityDensityWriteInfo.dstArrayElement = 0;
			descLastVelocityDensityWriteInfo.descriptorCount = 1;
			descLastVelocityDensityWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descLastVelocityDensityWriteInfo.pBufferInfo = nullptr;
			descLastVelocityDensityWriteInfo.pImageInfo = &descLastVelocityDensityInfo;
			descLastVelocityDensityWriteInfo.pTexelBufferView = nullptr;

			VkWriteDescriptorSet descCurrentVelocityDensityWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descCurrentVelocityDensityWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descCurrentVelocityDensityWriteInfo.dstSet = this->descriptorSets[i];
			descCurrentVelocityDensityWriteInfo.dstBinding = 6;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descCurrentVelocityDensityWriteInfo.dstArrayElement = 0;
			descCurrentVelocityDensityWriteInfo.descriptorCount = 1;
			descCurrentVelocityDensityWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descCurrentVelocityDensityWriteInfo.pBufferInfo = nullptr;
			descCurrentVelocityDensityWriteInfo.pImageInfo = &descCurrentVelocityDensityInfo;
			descCurrentVelocityDensityWriteInfo.pTexelBufferView = nullptr;

			VkWriteDescriptorSet descVelocityDensityTempWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descVelocityDensityTempWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descVelocityDensityTempWriteInfo.dstSet = this->descriptorSets[i];
			descVelocityDensityTempWriteInfo.dstBinding = 7;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descVelocityDensityTempWriteInfo.dstArrayElement = 0;
			descVelocityDensityTempWriteInfo.descriptorCount = 1;
			descVelocityDensityTempWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descVelocityDensityTempWriteInfo.pBufferInfo = nullptr;
			descVelocityDensityTempWriteInfo.pImageInfo = &descVelocityDensityTempInfo;
			descVelocityDensityTempWriteInfo.pTexelBufferView = nullptr;

			VkWriteDescriptorSet descLastDivergencePressureWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descLastDivergencePressureWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descLastDivergencePressureWriteInfo.dstSet = this->descriptorSets[i];
			descLastDivergencePressureWriteInfo.dstBinding = 8;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descLastDivergencePressureWriteInfo.dstArrayElement = 0;
			descLastDivergencePressureWriteInfo.descriptorCount = 1;
			descLastDivergencePressureWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descLastDivergencePressureWriteInfo.pBufferInfo = nullptr;
			descLastDivergencePressureWriteInfo.pImageInfo = &descLastDivergencePressureInfo;
			descLastDivergencePressureWriteInfo.pTexelBufferView = nullptr;

			VkWriteDescriptorSet descCurrentDivergencePressureWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descCurrentDivergencePressureWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descCurrentDivergencePressureWriteInfo.dstSet = this->descriptorSets[i];
			descCurrentDivergencePressureWriteInfo.dstBinding = 9;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descCurrentDivergencePressureWriteInfo.dstArrayElement = 0;
			descCurrentDivergencePressureWriteInfo.descriptorCount = 1;
			descCurrentDivergencePressureWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descCurrentDivergencePressureWriteInfo.pBufferInfo = nullptr;
			descCurrentDivergencePressureWriteInfo.pImageInfo = &descCurrentDivergencePressureInfo;
			descCurrentDivergencePressureWriteInfo.pTexelBufferView = nullptr;

			VkWriteDescriptorSet descDivergencePressureTempWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descDivergencePressureTempWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descDivergencePressureTempWriteInfo.dstSet = this->descriptorSets[i];
			descDivergencePressureTempWriteInfo.dstBinding = 10;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descDivergencePressureTempWriteInfo.dstArrayElement = 0;
			descDivergencePressureTempWriteInfo.descriptorCount = 1;
			descDivergencePressureTempWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descDivergencePressureTempWriteInfo.pBufferInfo = nullptr;
			descDivergencePressureTempWriteInfo.pImageInfo = &descDivergencePressureTempInfo;
			descDivergencePressureTempWriteInfo.pTexelBufferView = nullptr;

			VkWriteDescriptorSet descBCWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descBCWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descBCWriteInfo.dstSet = this->descriptorSets[i];
			descBCWriteInfo.dstBinding = 11;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descBCWriteInfo.dstArrayElement = 0;
			descBCWriteInfo.descriptorCount = 1;
			descBCWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descBCWriteInfo.pBufferInfo = nullptr;
			descBCWriteInfo.pImageInfo = &descBCInfo;
			descBCWriteInfo.pTexelBufferView = nullptr;

			VkWriteDescriptorSet descVorticityWriteInfo{};				// descriptorSet�� descriptor�� write�Ѵٴ� �ǹ�.
			descVorticityWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descVorticityWriteInfo.dstSet = this->descriptorSets[i];
			descVorticityWriteInfo.dstBinding = 12;
			// It's possible to update multiple descriptors at once in an array, starting at index dstArrayElement.
			descVorticityWriteInfo.dstArrayElement = 0;
			descVorticityWriteInfo.descriptorCount = 1;
			descVorticityWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			descVorticityWriteInfo.pBufferInfo = nullptr;
			descVorticityWriteInfo.pImageInfo = &descVorticityInfo;
			descVorticityWriteInfo.pTexelBufferView = nullptr;
			

			std::array<VkWriteDescriptorSet, 13> descriptorWriteInfos = { descUniforBuffWriteInfo,
																		  descLastDensityWriteInfo,
																		  descCurrentDensityWriteInfo,
																		  descDensityCopiesWriteInfo,
																		  descDensityCopySamplersWriteInfo,
																		  //
																		  descLastVelocityDensityWriteInfo,
																		  descCurrentVelocityDensityWriteInfo,
																		  descVelocityDensityTempWriteInfo,
																		  descLastDivergencePressureWriteInfo,
																		  descCurrentDivergencePressureWriteInfo,
																		  descDivergencePressureTempWriteInfo,
																		  descBCWriteInfo,
																		  descVorticityWriteInfo
																	    };

			// It accepts two kinds of arrays as parameters: an array of VkWriteDescriptorSet and an array of VkCopyDescriptorSet. 
			// The latter can be used to copy descriptors to each other, as its name implies.
			vkUpdateDescriptorSets(this->device,
				                   static_cast<uint32_t>(descriptorWriteInfos.size()), descriptorWriteInfos.data(),
				                   0, nullptr);

		}

	}
	
	void createPipelineLayout() {

		VkPipelineLayoutCreateInfo pplineLayout{};
		pplineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pplineLayout.setLayoutCount = 1;
		pplineLayout.pSetLayouts = &(this->descriptorSetLayout);
		pplineLayout.pushConstantRangeCount = 0;	// optional.
		pplineLayout.pPushConstantRanges = nullptr;	// optional.

		if (vkCreatePipelineLayout(this->device, &pplineLayout, nullptr, &this->gctPipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layouts.");
		}

		//VkPipelineLayoutCreateInfo ppline2Layout{};
		//pplineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		//pplineLayout.setLayoutCount = 1;
		//pplineLayout.pSetLayouts = //&(this->descriptorSetLayout);
		//pplineLayout.pushConstantRangeCount = 0;	// optional.
		//pplineLayout.pPushConstantRanges = nullptr;	// optional.

		//if (vkCreatePipelineLayout(this->device, &pplineLayout, nullptr, &graphics2PipelineLayout) != VK_SUCCESS) {
		//	throw std::runtime_error("failed to create pipeline layouts.");
		//}
	}
	void createComputePipelines() {

		const uint32_t consecutiveStagesNum = 8;
		//(this->computeShaderModules).resize(consecutiveStagesNum);

		//VkShaderModule compShaderModule3_0 = createShaderModule("shader_Sourcing.comp", "comp");
		auto compShaderCode3_0 = readFile("shaders_out/shader_Sourcing_comp.spv");
		VkShaderModule compShaderModule3_0 = createShaderModule(compShaderCode3_0);
		this->computeShaderModules.push_back(compShaderModule3_0);
		auto compShaderCode3_1 = readFile("shaders_out/shader_Divergence_comp.spv"); 
		VkShaderModule compShaderModule3_1 = createShaderModule(compShaderCode3_1);
		this->computeShaderModules.push_back(compShaderModule3_1);
		auto compShaderCode3_2 = readFile("shaders_out/shader_Jacobi_1_comp.spv");
		VkShaderModule compShaderModule3_2 = createShaderModule(compShaderCode3_2);
		this->computeShaderModules.push_back(compShaderModule3_2);
		auto compShaderCode3_3 = readFile("shaders_out/shader_Jacobi_2_comp.spv");
		VkShaderModule compShaderModule3_3 = createShaderModule(compShaderCode3_3);
		this->computeShaderModules.push_back(compShaderModule3_3);
		auto compShaderCode3_4 = readFile("shaders_out/shader_ApplyPressure_comp.spv");
		VkShaderModule compShaderModule3_4 = createShaderModule(compShaderCode3_4);
		this->computeShaderModules.push_back(compShaderModule3_4);
		auto compShaderCode3_5 = readFile("shaders_out/shader_ComputeVorticity_comp.spv");
		VkShaderModule compShaderModule3_5 = createShaderModule(compShaderCode3_5);
		this->computeShaderModules.push_back(compShaderModule3_5);
		auto compShaderCode3_6 = readFile("shaders_out/shader_ApplyVorticity_comp.spv");
		VkShaderModule compShaderModule3_6 = createShaderModule(compShaderCode3_6);
		this->computeShaderModules.push_back(compShaderModule3_6);
		auto compShaderCode3_7 = readFile("shaders_out/shader_Advection_comp.spv");
		VkShaderModule compShaderModule3_7 = createShaderModule(compShaderCode3_7);
		this->computeShaderModules.push_back(compShaderModule3_7);

		std::vector<VkComputePipelineCreateInfo> cPipeCreateInfos;
		for (size_t i = 0; i < consecutiveStagesNum; i++)
		{
			VkPipelineShaderStageCreateInfo compShaderStateInfo{};
			compShaderStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			compShaderStateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
			compShaderStateInfo.module = computeShaderModules[i];
			compShaderStateInfo.pName = "main";

			VkComputePipelineCreateInfo cPipelinesInfo{};
			cPipelinesInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			cPipelinesInfo.stage = compShaderStateInfo;
			cPipelinesInfo.layout = this->gctPipelineLayout;
			 
			cPipeCreateInfos.push_back(cPipelinesInfo);
		}
		


		(this->computePipelines).resize(consecutiveStagesNum);

		if (vkCreateComputePipelines(this->device, nullptr,
			static_cast<uint32_t>(cPipeCreateInfos.size()), cPipeCreateInfos.data(),
			nullptr, (this->computePipelines).data())
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create Compute Pipelines.");
		}	std::cout << "Compute Pipelines are created." << std::endl;

	}
	void createGraphicsPipeline() {

		//VkViewport viewport{};	// to member variable.	// �븻������ ���� ��ǥ�� ũ�� �״�ξ�. �ٸ� int(800�ȼ� ��) ��� float(800.0 ��)�� ǥ���ϴ� �� ����.
		this->viewport.width = (float)(this->swapChainExtent).width;
		this->viewport.height = (float)(this->swapChainExtent).height;
		this->viewport.x = 0.0f;
		this->viewport.y = 0.0f;
		this->viewport.minDepth = 0.0f;
		this->viewport.maxDepth = 1.0f;

		//VkRect2D scissor{};		// to member variable.
		this->scissor.extent = this->swapChainExtent;
		this->scissor.offset = VkOffset2D{ 0, 0 };

		VkPipelineViewportStateCreateInfo viewportStateInfo{};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.viewportCount = 1;
		//viewportStateInfo.pViewports = &viewport;		// With dynamic state on, you only need to specify their count at pipeline creation time
		viewportStateInfo.scissorCount = 1;
		//viewportStateInfo.pScissors = &scissor;

		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		VkDynamicState dStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		dynamicStateInfo.dynamicStateCount = 2;
		dynamicStateInfo.pDynamicStates = dStates;



		VkVertexInputBindingDescription vertexInputBindingInfo{};
		vertexInputBindingInfo.binding = 0;
		vertexInputBindingInfo.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputBindingInfo.stride = sizeof(BoxParticle);

		std::array<VkVertexInputAttributeDescription, 2> vertexInputAttribInfos;
		vertexInputAttribInfos[0].binding = 0;
		vertexInputAttribInfos[0].location = 0;
		vertexInputAttribInfos[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttribInfos[0].offset = offsetof(BoxParticle, position);
		vertexInputAttribInfos[1].binding = 0;
		vertexInputAttribInfos[1].location = 1;
		vertexInputAttribInfos[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttribInfos[1].offset = offsetof(BoxParticle, texCoord);

		VkPipelineVertexInputStateCreateInfo vertInputInfo{};
		vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertInputInfo.vertexBindingDescriptionCount = 1;
		vertInputInfo.pVertexBindingDescriptions = &vertexInputBindingInfo;
		vertInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttribInfos.size());
		vertInputInfo.pVertexAttributeDescriptions = vertexInputAttribInfos.data();



		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		// If you set the primitiveRestartEnable member to VK_TRUE, 
		// then it's possible to break up lines and triangles in the _STRIP topology modes by using a special index of 0xFFFF or 0xFFFFFFFF.
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;



		// Vertex Shader...



		// Rasterization
		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
		rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationInfo.lineWidth = 1.0f;
		//
		rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		//
		// If depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes are clamped to them 
		//		as opposed to discarding them. This is useful in some special cases like shadow maps. 
		//		Using this requires enabling a GPU feature.
		rasterizationInfo.depthClampEnable = VK_FALSE;
		// If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage. 
		//		This basically disables any output to the framebuffer.
		rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		//
		rasterizationInfo.depthBiasEnable = VK_FALSE;
		rasterizationInfo.depthBiasSlopeFactor = 0.0f;
		rasterizationInfo.depthBiasConstantFactor = 0.0f;
		rasterizationInfo.depthBiasClamp = 0.0f;



		// Depth Test stage
		VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
		depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilCreateInfo.depthTestEnable = VK_TRUE;
		depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
		depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilCreateInfo.minDepthBounds = 0.0f;
		depthStencilCreateInfo.maxDepthBounds = 1.0f;
		depthStencilCreateInfo.stencilTestEnable = VK_FALSE;
		// The last three fields configure stencil buffer operations, which we also won't be using in this tutorial
		//depthStencilCreateInfo.front = {};
		//depthStencilCreateInfo.back = {};



		// Fragment Shader...



		// MultiSampling stage
		VkPipelineMultisampleStateCreateInfo multiSampleStateInfo{};
		multiSampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multiSampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multiSampleStateInfo.pSampleMask = nullptr;
		multiSampleStateInfo.sampleShadingEnable = VK_FALSE;
		multiSampleStateInfo.minSampleShading = 1.0f;
		multiSampleStateInfo.alphaToOneEnable = VK_FALSE;
		multiSampleStateInfo.alphaToCoverageEnable = VK_FALSE;



		// demonstrating pseudocode for color blending:
		// finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
		// finalColor.a   = (srcAlphaBlendFactor * newColor.a)   <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
		// finalColor     = newColor;
		// src �� ���� pipeline���� ���γ������ִ� image�̰�, dst �� ������ ���� attachment �̹����� ��.
		VkPipelineColorBlendAttachmentState colorBlendAttachmentInfo{};
		colorBlendAttachmentInfo.blendEnable = VK_TRUE;
		colorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;		// or VK_BLEND_FACTOR_SRC_ALPHA;	// ���⼭ ���� BELND_FACTOR ��, �ٷ� �Ʒ��� ������, VkPipelineColorBlendStateCreateInfo::blendConstants (float��) �� �ǹ��Ѵ�.
		colorBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;	// or VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
			| VK_COLOR_COMPONENT_G_BIT
			| VK_COLOR_COMPONENT_B_BIT
			| VK_COLOR_COMPONENT_A_BIT;

		// This structure below references the array of structures for all of the framebuffers.
		VkPipelineColorBlendStateCreateInfo colorBlendStateInfo{};
		colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateInfo.attachmentCount = 1;
		colorBlendStateInfo.pAttachments = &colorBlendAttachmentInfo;
		// Below enables second method of blending. (bitwise combination)
		colorBlendStateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;
		//float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		colorBlendStateInfo.blendConstants[0] = 0.0f;
		colorBlendStateInfo.blendConstants[1] = 0.0f;
		colorBlendStateInfo.blendConstants[2] = 0.0f;
		colorBlendStateInfo.blendConstants[3] = 0.0f;



		//std::vector<VkShaderModule> shaderModules;	// �ɹ������� ��.
		// VkShaderModule vertShaderModule = createShaderModule("shader.vert", "vert");
		// this->graphicsShaderModules.push_back(vertShaderModule);
		// VkShaderModule fragShaderModule = createShaderModule("shader.frag", "frag");
		// this->graphicsShaderModules.push_back(fragShaderModule);
		auto vertShaderCode = readFile("shaders_out/shader_vert.spv"); 
		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		this->graphicsShaderModules.push_back(vertShaderModule);
		auto fragShaderCode = readFile("shaders_out/shader_frag.spv"); 
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
		this->graphicsShaderModules.push_back(fragShaderModule);

		std::vector<VkPipelineShaderStageCreateInfo> graphicsShaderStages;

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;	// VK_STRUCTURE_TYPE_''SHADER_''CREATE_INFO_'EXT'; �� �ް����� ����!
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = this->graphicsShaderModules[0];
		vertShaderStageInfo.pName = "main";
		// vertShaderStageInfo.pSpecializationInfo;	// Entry Point �� �����ϴ� �߰���ɰ� ���õ� �׸�.
		// vertShaderStageInfo.flags;					// subgroup ���õ� �߰���� flag��.
		graphicsShaderStages.push_back(vertShaderStageInfo);

		VkPipelineShaderStageCreateInfo fragShaderStateInfo{};
		fragShaderStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStateInfo.module = this->graphicsShaderModules[1];
		fragShaderStateInfo.pName = "main";
		graphicsShaderStages.push_back(fragShaderStateInfo);



		VkGraphicsPipelineCreateInfo gPipelineInfo{};
		gPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		gPipelineInfo.renderPass = this->renderPass;
		// the index of the subpass in the render pass where this pipeline will 'be used'!
		gPipelineInfo.subpass = 0;
		gPipelineInfo.layout = this->gctPipelineLayout;
		//
		gPipelineInfo.pViewportState = &viewportStateInfo;		// dynamic state�� �����ߴ���, �̺κ��� �ݵ�� ��־�� �Ѵ�.
		gPipelineInfo.pDynamicState = &dynamicStateInfo;
		//
		gPipelineInfo.pVertexInputState = &vertInputInfo;
		gPipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		gPipelineInfo.stageCount = static_cast<uint32_t>(graphicsShaderStages.size());
		//gPipelineInfo.pTessellationState;
		gPipelineInfo.pRasterizationState = &rasterizationInfo;
		gPipelineInfo.pStages = graphicsShaderStages.data();
		gPipelineInfo.pDepthStencilState = &depthStencilCreateInfo;		// optional.
		gPipelineInfo.pMultisampleState = &multiSampleStateInfo;
		gPipelineInfo.pColorBlendState = &colorBlendStateInfo;
		//
		//gPipelineInfo.flags;
		gPipelineInfo.basePipelineHandle = nullptr;
		gPipelineInfo.basePipelineIndex = -1;	// basePipelineIndex is an index into the pCreateInfos parameter(vkCreateGraphicsPipelines�Լ������� ����) to use as a pipeline to derive from.

		if (vkCreateGraphicsPipelines(this->device, nullptr,
			1, &gPipelineInfo,
			nullptr, &(this->graphicsPipeline))
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create Graphics Pipeline.");
		}	std::cout << "Graphics Pipeline is created." << std::endl;

	}
	void createGraphicsPipeline2() {

		//VkViewport viewport{};	// to member variable.	// �븻������ ���� ��ǥ�� ũ�� �״�ξ�. �ٸ� int(800�ȼ� ��) ��� float(800.0 ��)�� ǥ���ϴ� �� ����.
		this->viewport.width = (float)(this->swapChainExtent).width;
		this->viewport.height = (float)(this->swapChainExtent).height;
		this->viewport.x = 0.0f;
		this->viewport.y = 0.0f;
		this->viewport.minDepth = 0.0f;
		this->viewport.maxDepth = 1.0f;

		//VkRect2D scissor{};		// to member variable.
		this->scissor.extent = this->swapChainExtent;
		this->scissor.offset = VkOffset2D{ 0, 0 };

		VkPipelineViewportStateCreateInfo viewportStateInfo{};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.viewportCount = 1;
		//viewportStateInfo.pViewports = &viewport;		// With dynamic state on, you only need to specify their count at pipeline creation time
		viewportStateInfo.scissorCount = 1;
		//viewportStateInfo.pScissors = &scissor;

		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		VkDynamicState dStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		dynamicStateInfo.dynamicStateCount = 2;
		dynamicStateInfo.pDynamicStates = dStates;



		VkVertexInputBindingDescription vertexInputBindingInfo{};
		vertexInputBindingInfo.binding = 0;
		vertexInputBindingInfo.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputBindingInfo.stride = sizeof(BoxParticle);

		std::array<VkVertexInputAttributeDescription, 2> vertexInputAttribInfos;
		vertexInputAttribInfos[0].binding = 0;
		vertexInputAttribInfos[0].location = 0;
		vertexInputAttribInfos[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttribInfos[0].offset = offsetof(BoxParticle, position);
		vertexInputAttribInfos[1].binding = 0;
		vertexInputAttribInfos[1].location = 1;
		vertexInputAttribInfos[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputAttribInfos[1].offset = offsetof(BoxParticle, texCoord);

		VkPipelineVertexInputStateCreateInfo vertInputInfo{};
		vertInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertInputInfo.vertexBindingDescriptionCount = 1;
		vertInputInfo.pVertexBindingDescriptions = &vertexInputBindingInfo;
		vertInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttribInfos.size());
		vertInputInfo.pVertexAttributeDescriptions = vertexInputAttribInfos.data();



		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		// If you set the primitiveRestartEnable member to VK_TRUE, 
		// then it's possible to break up lines and triangles in the _STRIP topology modes by using a special index of 0xFFFF or 0xFFFFFFFF.
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;



		// Vertex Shader...



		// Rasterization
		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
		rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationInfo.polygonMode = VK_POLYGON_MODE_LINE;
		rasterizationInfo.lineWidth = 1.0f;
		//
		rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		//rasterizationInfo.cullMode = VK_CULL_MODE__BACK;
		//
		// If depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes are clamped to them 
		//		as opposed to discarding them. This is useful in some special cases like shadow maps. 
		//		Using this requires enabling a GPU feature.
		rasterizationInfo.depthClampEnable = VK_FALSE;
		// If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage. 
		//		This basically disables any output to the framebuffer.
		rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		//
		rasterizationInfo.depthBiasEnable = VK_FALSE;
		rasterizationInfo.depthBiasSlopeFactor = 0.0f;
		rasterizationInfo.depthBiasConstantFactor = 0.0f;
		rasterizationInfo.depthBiasClamp = 0.0f;



		// Depth Test stage
		VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
		depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilCreateInfo.depthTestEnable = VK_TRUE;
		depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
		depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
		depthStencilCreateInfo.minDepthBounds = 0.0f;
		depthStencilCreateInfo.maxDepthBounds = 1.0f;
		depthStencilCreateInfo.stencilTestEnable = VK_FALSE;
		// The last three fields configure stencil buffer operations, which we also won't be using in this tutorial
		//depthStencilCreateInfo.front = {};
		//depthStencilCreateInfo.back = {};



		// Fragment Shader...



		// MultiSampling stage
		VkPipelineMultisampleStateCreateInfo multiSampleStateInfo{};
		multiSampleStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multiSampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multiSampleStateInfo.pSampleMask = nullptr;
		multiSampleStateInfo.sampleShadingEnable = VK_FALSE;
		multiSampleStateInfo.minSampleShading = 1.0f;
		multiSampleStateInfo.alphaToOneEnable = VK_FALSE;
		multiSampleStateInfo.alphaToCoverageEnable = VK_FALSE;



		// demonstrating pseudocode for color blending:
		// finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
		// finalColor.a   = (srcAlphaBlendFactor * newColor.a)   <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
		// finalColor     = newColor;
		// src �� ���� pipeline���� ���γ������ִ� image�̰�, dst �� ������ ���� attachment �̹����� ��.
		VkPipelineColorBlendAttachmentState colorBlendAttachmentInfo{};
		colorBlendAttachmentInfo.blendEnable = VK_TRUE;
		colorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;		// or VK_BLEND_FACTOR_SRC_ALPHA;	// ���⼭ ���� BELND_FACTOR ��, �ٷ� �Ʒ��� ������, VkPipelineColorBlendStateCreateInfo::blendConstants (float��) �� �ǹ��Ѵ�.
		colorBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;	// or VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
			| VK_COLOR_COMPONENT_G_BIT
			| VK_COLOR_COMPONENT_B_BIT
			| VK_COLOR_COMPONENT_A_BIT;

		// This structure below references the array of structures for all of the framebuffers.
		VkPipelineColorBlendStateCreateInfo colorBlendStateInfo{};
		colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateInfo.attachmentCount = 1;
		colorBlendStateInfo.pAttachments = &colorBlendAttachmentInfo;
		// Below enables second method of blending. (bitwise combination)
		colorBlendStateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;
		//float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		colorBlendStateInfo.blendConstants[0] = 0.0f;
		colorBlendStateInfo.blendConstants[1] = 0.0f;
		colorBlendStateInfo.blendConstants[2] = 0.0f;
		colorBlendStateInfo.blendConstants[3] = 0.0f;



		//std::vector<VkShaderModule> shaderModules;	// �ɹ������� ��.
		auto vertShader2Code = readFile("shaders_out/shader2_vert.spv"); 
		VkShaderModule vertShaderModule2 = createShaderModule(vertShader2Code);
		this->graphicsShaderModules2.push_back(vertShaderModule2);
		auto fragShader2Code = readFile("shaders_out/shader2_frag.spv"); 
		VkShaderModule fragShaderModule2 = createShaderModule(fragShader2Code);
		this->graphicsShaderModules2.push_back(fragShaderModule2);

		std::vector<VkPipelineShaderStageCreateInfo> graphics2ShaderStages;

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;	// VK_STRUCTURE_TYPE_''SHADER_''CREATE_INFO_'EXT'; �� �ް����� ����!
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = this->graphicsShaderModules2[0];
		vertShaderStageInfo.pName = "main";
		// vertShaderStageInfo.pSpecializationInfo;	// Entry Point �� �����ϴ� �߰���ɰ� ���õ� �׸�.
		// vertShaderStageInfo.flags;					// subgroup ���õ� �߰���� flag��.
		graphics2ShaderStages.push_back(vertShaderStageInfo);

		VkPipelineShaderStageCreateInfo fragShaderStateInfo{};
		fragShaderStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStateInfo.module = this->graphicsShaderModules2[1];
		fragShaderStateInfo.pName = "main";
		graphics2ShaderStages.push_back(fragShaderStateInfo);



		VkGraphicsPipelineCreateInfo g2PipelineInfo{};
		g2PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		g2PipelineInfo.renderPass = this->renderPass;
		// the index of the subpass in the render pass where this pipeline will 'be used'!
		g2PipelineInfo.subpass = 0;
		g2PipelineInfo.layout = this->gctPipelineLayout;
		//
		g2PipelineInfo.pViewportState = &viewportStateInfo;		// dynamic state�� �����ߴ���, �̺κ��� �ݵ�� ��־�� �Ѵ�.
		g2PipelineInfo.pDynamicState = &dynamicStateInfo;
		//
		g2PipelineInfo.pVertexInputState = &vertInputInfo;
		g2PipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		g2PipelineInfo.stageCount = static_cast<uint32_t>(graphics2ShaderStages.size());
		//gPipelineInfo.pTessellationState;
		g2PipelineInfo.pRasterizationState = &rasterizationInfo;
		g2PipelineInfo.pStages = graphics2ShaderStages.data();
		g2PipelineInfo.pDepthStencilState = &depthStencilCreateInfo;		// optional.
		g2PipelineInfo.pMultisampleState = &multiSampleStateInfo;
		g2PipelineInfo.pColorBlendState = &colorBlendStateInfo;
		//
		//gPipelineInfo.flags;
		g2PipelineInfo.basePipelineHandle = nullptr;
		g2PipelineInfo.basePipelineIndex = -1;	// basePipelineIndex is an index into the pCreateInfos parameter(vkCreateGraphicsPipelines�Լ������� ����) to use as a pipeline to derive from.

		if (vkCreateGraphicsPipelines(this->device, nullptr,
			1, &g2PipelineInfo,
			nullptr, &(this->graphicsPipeline2))
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create Graphics Pipeline 2.");
		}	std::cout << "Graphics Pipeline 2 is created." << std::endl;
	}

	void createCommandBuffer() {

		this->gctCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo gctComBuffAllocInfo{};
		gctComBuffAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		gctComBuffAllocInfo.commandPool = gctCommandPool;
		gctComBuffAllocInfo.commandBufferCount = static_cast<uint32_t>(this->gctCommandBuffers.size());
		// VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted to a queue for execution, but cannot be called from other command buffers.
		// VK_COMMAND_BUFFER_LEVEL_SECONDARY: Cannot be submitted directly, but can be called from primary command buffers.
		gctComBuffAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		if (vkAllocateCommandBuffers(this->device, &gctComBuffAllocInfo, this->gctCommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics command buffers.");
		}	std::cout << "Graphics & Compute & Transfer - command buffer is created." << std::endl;

	}

	void createSyncObject() {

		this->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		this->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		this->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);



		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		// � semaphore ����ųĴ� ������ �ȳ־��ֹǷ�, vkCreateSemaphore()�Լ� �ѹ� �����Ҷ�����, �Ѱ��� semaphore��ü�� ������ִµ�?
		// ���� �迭�� �����͸� ���ڷ� �־��ָ� �ȵǰ�, vkCreateSemaphore()�Լ��� ������ �����Ͽ�, �� ��ü�� ���� ���� �ϳ����� �־���� �ҵ�.

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore( this->device, &semaphoreInfo, nullptr, &((this->renderFinishedSemaphores)[i]) ) != VK_SUCCESS) {
				throw std::runtime_error("failed to create renderFinishedSemaphores.");
			}	std::cout << "renderFinishedSemaphores are created." << std::endl;
			if (vkCreateSemaphore( this->device, &semaphoreInfo, nullptr, &((this->imageAvailableSemaphores)[i]) ) != VK_SUCCESS) {
				throw std::runtime_error("failed to create imageAvailableSemaphores.");
			}	std::cout << "imageAvailableSemaphores are created." << std::endl;
		}
		



		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		// � semaphore ����ųĴ� ������ �ȳ־��ֹǷ�, vkCreateSemaphore()�Լ� �ѹ� �����Ҷ�����, �Ѱ��� semaphore��ü�� ������ִµ�?
		// ���� �迭�� �����͸� ���ڷ� �־��ָ� �ȵǰ�, vkCreateSemaphore()�Լ��� ������ �����Ͽ�, �� ��ü�� ���� ���� �ϳ����� �־���� 
		
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateFence( this->device, &fenceInfo, nullptr, &((this->inFlightFences)[i]) ) != VK_SUCCESS) {
				throw std::runtime_error("failed to create inFlightFence.");
			}	std::cout << "inFlightFence is created." << std::endl;
		}

		std::cout << std::endl;
		std::cout << std::endl;

	}





//2////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// help functions //////////////////////////////////////////////////////////////////////////////////////////////////
private:
	void getUserRequWindowRequExtensions(std::vector<std::string>& userRequWindowRequExtensions) {

		uint32_t windowRequExtension_count = 0;
		const char** windowRequExtension_charArrays = nullptr;
		windowRequExtension_charArrays = glfwGetRequiredInstanceExtensions(&windowRequExtension_count);
		for (uint32_t i = 0; i < windowRequExtension_count; i++) {
			std::string string_temp(*(windowRequExtension_charArrays + i));
			userRequWindowRequExtensions.push_back(string_temp);
		}

		for (uint32_t i = 0; i < gl_userRequExtensions.size(); i++) {
			userRequWindowRequExtensions.push_back(std::string(gl_userRequExtensions[i]));
		}

		return;
	}
	bool checkExtensions(const std::vector<std::string>& userRequWindowRequExtensions,
						 std::vector<const char*>& matchedExtensions) {
		
		uint32_t supportedInstExtension_count = 0;
		std::vector<VkExtensionProperties> supportedInstExtensions;
		vkEnumerateInstanceExtensionProperties(nullptr, &supportedInstExtension_count, nullptr);
		supportedInstExtensions.resize(supportedInstExtension_count);
		vkEnumerateInstanceExtensionProperties(nullptr, &supportedInstExtension_count, supportedInstExtensions.data());

		std::cout << "Required and User-requested Extensions: " 
				  << userRequWindowRequExtensions.size() << std::endl;
		uint32_t matchedCount = 0;
		for (const auto& uRwRExtension : userRequWindowRequExtensions) {
			std::cout << uRwRExtension << std::endl;
			for (const auto& supportedExtension : supportedInstExtensions) {
				if ( !strcmp(uRwRExtension.c_str(), supportedExtension.extensionName) ) {
					matchedCount++;
					std::cout << "\t\t\t\t" << "Supported by Vulkan Instnace." << std::endl;
					matchedExtensions.push_back(uRwRExtension.c_str());		// supportedExtension.extensionName���� ���� ������ �ȵȴ�! supportedExtension�� �� �Լ��������� ��ȿ�ϰ� ����� ���̱� �����̴�.
					continue;
				}
			}
		}
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;

		std::cout << "Supported Extensions by Vulkan instance: " << supportedInstExtension_count << std::endl;
		uint32_t howManySupportedExtensionsFulfilled = 0;
		for (const auto& supportedExtension : supportedInstExtensions) {
			std::cout << supportedExtension.extensionName << std::endl;
			for (const auto& uRwRExtension : userRequWindowRequExtensions) {
				if ( !strcmp(supportedExtension.extensionName, uRwRExtension.c_str()) ) {
					std::cout << "\t\t\t\t" << "Required by window." << std::endl;
					howManySupportedExtensionsFulfilled++;
					continue;
				}
			}
		}
		std::cout << std::endl;
		std::cout << std::endl;
		std::cout << std::endl;

		bool isExtensionsFulfilled = false;
		if (matchedCount == howManySupportedExtensionsFulfilled) {
			isExtensionsFulfilled = true;
		}

		return isExtensionsFulfilled;
	}
	bool checkValidationLayers(std::vector<const char*>& matchedValidationLayers)
	{
		uint32_t availableValidationLayerCount = 0;
		vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, nullptr);
		std::vector<VkLayerProperties> availableValidationLayers(availableValidationLayerCount);
		vkEnumerateInstanceLayerProperties(&availableValidationLayerCount, availableValidationLayers.data());

		//std::vector<const char*> matchedValidationLayers;

		std::cout << "Available Validation Layers: " << availableValidationLayerCount << std::endl;
		uint32_t howManyValidationLayersMatched = 0;
		for (const auto& availableValidationLayer : availableValidationLayers) {
			std::cout << availableValidationLayer.layerName << std::endl;
			//bool isThisLayerAvailable = false;
			for (const auto& validationLayer : gl_validationLayers) {
				if (!strcmp(availableValidationLayer.layerName, validationLayer)) {
					std::cout << "\t\t\t\t" << "User-Required & vulkanKHR-Supported" << std::endl;
					howManyValidationLayersMatched++;
					matchedValidationLayers.push_back(validationLayer);			// availableValidationLayer.layerName�� ������ �־��ָ� �ȵȴ�! ���Լ��ۿ����� ������ ��ü�� �������̱� �����̴�!
					continue;
				}
			}
		}
		std::cout << std::endl;
		std::cout << std::endl;


		bool isUserDefinedValidationLayersFulfilled = false;
		if (static_cast<uint32_t>(matchedValidationLayers.size()) == howManyValidationLayersMatched) {
			isUserDefinedValidationLayersFulfilled = true;
		}

		return isUserDefinedValidationLayersFulfilled;
	}
	void populateDebugCallbackInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCallbackInfo) {
		debugCallbackInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCallbackInfo.pfnUserCallback = &debugCallbackFunc;
		debugCallbackInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
		debugCallbackInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			; //| VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
		debugCallbackInfo.pUserData = nullptr;
		//debugCallbackInfo.flags = 0;
	}
	VkResult createDebugUtilsMessengerEXT(VkInstance instance, 
										  VkDebugUtilsMessengerCreateInfoEXT* pDebugMessengerCreateInfo,
										  const VkAllocationCallbacks* pAllocator,
										  VkDebugUtilsMessengerEXT* pDebugMessenger) 
	{	// �ƽ��Ե� �� vkCreateDebugUtilsMessengerEXT ��� �Լ���, 'Ȯ���Լ�(~EXT�� ������ �Լ�)'�̱� ������, 
		// �ڵ����� �ε������� �ʽ��ϴ�. (�� Instance�� callback�Լ���) 
		// �츮�� vkGetInstanceProcAddr()�� ����Ͽ�, Instnace�� ���ε���Ű��, 
		// �� ���ε��� �Լ��� �ּҸ�, ���� ã�ƾ� �մϴ�.
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance, 
																			  "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pDebugMessengerCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}
	void destroyDebugUtilsMessengerEXT(VkInstance instance,
									   VkDebugUtilsMessengerEXT DebugMessenger,
									   const VkAllocationCallbacks* pAllocator)
	{	// �ƽ��Ե� �� vkCreateDebugUtilsMessengerEXT ��� �Լ���, 'Ȯ���Լ�(~EXT�� ������ �Լ�)'�̱� ������, 
		// �ڵ����� �ε������� �ʽ��ϴ�. (�� Instance�� callback�Լ���) 
		// �츮�� vkGetInstanceProcAddr()�� ����Ͽ�, Instnace�� ���ε���Ű��, 
		// �� ���ε��� �Լ��� �ּҸ�, ���� ã�ƾ� �մϴ�.
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance,
																			   "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, DebugMessenger, pAllocator);		// vkDestroy�Լ����� ���ϰ��� ����.
		}
		//else {
		//	return VK_ERROR_EXTENSION_NOT_PRESENT;
		//}
	}
	// VkResult lookInside_glfwCreateWindowSurface(VkInstance& instance, GLFWwindow* window,
	// 											const VkAllocationCallbacks* pAllocCallbacks,
	// 											VkSurfaceKHR* pSurface) 
	// {
	// 	// glfwCreateWindowSurface() �Լ��� ���α����� ������ ���� �Լ��� ����� ���Ҵ�.

	// 	// VkWin32SurfaceCreateInfoKHR �� ����Ϸ���, #define VK_USE_PLATFORM_WIN32_KHR �� ��������� �Ѵ�.
	// 	//VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	// 	VkXcbSurfaceCreateInfoKHR surfaceCreateInfo{};
	// 	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	// 	// glfwGetWin32Window ����Ϸ���, 
	// 	// #define GLFW_EXPOSE_NATIVE_WIN32
	// 	// #include <GLFW/glfw3native.h> �� ���� include �������.
	// 	//surfaceCreateInfo.hwnd = glfwGetWin32Window(window);		//(HWND)window; �̷��� �ᵵ �ǳ�?
	// 	surfaceCreateInfo.connection = glfwGetWindowUserPointer(window);
	// 	// GetModuleHandle()�Լ��� ������API�� �Լ��ε�.	
	// 	surfaceCreateInfo.window = GetModuleHandle(nullptr);		//(HINSTANCE)(this->instance); �̷��� �ᵵ �ǳ� ?

	// 	// ����������, vkCreateWin32SurfaceKHR()�Լ��� ������, #define VK_USE_PLATFORM_WIN32_KHR �� ��������� �Ѵ�.
	// 	//return vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, pAllocCallbacks, pSurface);
	// 	return vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, pAllocCallbacks, pSurface);
	// }

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsQueueFamilyIndex;
		// It's actually possible that the queue families supporting drawing commands and the ones supporting presentation do not overlap
		// But my GPU's graphicsQueueFamily and presentationQueueFamily does overlap.
		// However, in any case(Ȥ�ø𸣴�), graphicsQueueFamily�� presentationQueueFamily�� �����ؼ� Ž���غ���.
		std::optional<uint32_t> presentQueueFamilyIndex;
		std::optional<uint32_t> computeQueueFamilyIndex;

		bool all_has_value() {
			return graphicsQueueFamilyIndex.has_value() && presentQueueFamilyIndex.has_value() && computeQueueFamilyIndex.has_value();
		}
	};
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& phyDevice) {
		//QueueFamilyIndices QFIndices;		// to member variable.

		uint32_t QFCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &QFCount, nullptr);
		std::vector<VkQueueFamilyProperties> QFProperties(QFCount);
		vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &QFCount, QFProperties.data());

		std::cout << std::endl;
		std::cout<< QFCount << std::endl;	// ����ϸ� 6����.	(�� �׷���ī�忡�� �� 6���� QueueFamily�� ����.
		
		uint32_t i = 0;
		VkBool32 isPresentSupported = false;
		for (const auto& QFProperty : QFProperties)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(phyDevice, i, this->surface, &isPresentSupported);

			if (QFProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				QFIndices.graphicsQueueFamilyIndex = i;
				std::cout << "graphicsQueueFamilyIndex: " << i << std::endl;
			}
			if ((QFProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) && isPresentSupported) {		//
				isPresentSupported = false;
				QFIndices.presentQueueFamilyIndex = i;
				std::cout << "presentQueueFamilyIndex: " << i << std::endl;
			}
			if ((QFProperty.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
				QFIndices.computeQueueFamilyIndex = i;
				std::cout << "computeQueueFamilyIndex: " << i << std::endl;
			}

			if (QFIndices.all_has_value()) {		//
				break;
			}

			i++;
		}


		// ������¿� for loop.
		uint32_t j = 0;
		for (const auto& QFProperty : QFProperties)
		{
			std::bitset<32> x(QFProperty.queueFlags);
			std::cout << "QFIndex: " << j << "\t" << "QFProperty.queueFlags: " << x << std::endl;

			j++;
		}
		//
		// ����: �� �׷���ī�� QueueFamily ���.		
		// QFIndex: 0      QFProperty.queueFlags: 00000000000000000000000000001111	
		//										( VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT )
		// QFIndex: 1      QFProperty.queueFlags: 00000000000000000000000000001100
		//										( VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT )
		// QFIndex: 2      QFProperty.queueFlags: 00000000000000000000000000001110
		//										( VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT )
		// QFIndex: 3      QFProperty.queueFlags: 00000000000000000000000000101100
		//										( VK_QUEUE_VIDEO_DECODE_BIT_KHR ) | ( VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT )
		// QFIndex: 4      QFProperty.queueFlags: 00000000000000000000000001001100
		//										( VK_QUEUE_VIDEO_ENCODE_BIT_KHR ) | ( VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT )
		// QFIndex: 5      QFProperty.queueFlags: 00000000000000000000000100001100
		//										( VK_QUEUE_OPTICAL_FLOW_BIT_NV ) | ( VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT )
		// (�� �׷���ī�忡�� VK_QUEUE_PROTECTED_BIT �� ���� QueueFamily�� ����. ������ QFproperty�� ���ؼ���, ���� QueueFamily�� ������ ����.)
		// 
		// According to Vulkan specifications,
		// typedef enum VkQueueFlagBits {
		//	 VK_QUEUE_GRAPHICS_BIT = 0x00000001,
		//	 VK_QUEUE_COMPUTE_BIT = 0x00000002,
		//	 VK_QUEUE_TRANSFER_BIT = 0x00000004,
		//	 VK_QUEUE_SPARSE_BINDING_BIT = 0x00000008,
		//	 // Provided by VK_VERSION_1_1
		//	 VK_QUEUE_PROTECTED_BIT = 0x00000010,
		//	 // Provided by VK_KHR_video_decode_queue
		//	 VK_QUEUE_VIDEO_DECODE_BIT_KHR = 0x00000020,
		//	 // Provided by VK_KHR_video_encode_queue
		//	 VK_QUEUE_VIDEO_ENCODE_BIT_KHR = 0x00000040,
		//	 // Provided by VK_NV_optical_flow
		//	 VK_QUEUE_OPTICAL_FLOW_BIT_NV = 0x00000100,
		// } VkQueueFlagBits;
		//
		std::cout << std::endl;

		return QFIndices;
	}
	bool checkUserRequDeviceExtensionsSupported(VkPhysicalDevice phyDevice) {
		
		uint32_t deviceExtension_count = 0;
		vkEnumerateDeviceExtensionProperties(phyDevice, nullptr, &deviceExtension_count, nullptr);
		std::vector<VkExtensionProperties> deviceExtensions(deviceExtension_count);
		vkEnumerateDeviceExtensionProperties(phyDevice, nullptr, &deviceExtension_count, deviceExtensions.data());

		std::cout << std::endl;
		std::cout << "Device Extensions: " << deviceExtension_count << std::endl;
		for (const auto& deviceExtension : deviceExtensions)
		{
			std::cout << deviceExtension.extensionName << std::endl;
			for (const auto& userRequDeviceExtension : userRequDeviceExtensions)
			{
				if (!strcmp(deviceExtension.extensionName, userRequDeviceExtension)) {
					std::cout << "\t\t\t\t" << "Requested by User." << std::endl;
					continue;
				}
			}
		}
		std::cout << std::endl;

		int matched_count = 0;
		int userRequ_count = 0;
		for (const auto& userRequDeviceExtension : userRequDeviceExtensions)
		{
			for (const auto& deviceExtension : deviceExtensions)
			{
				if (!strcmp(deviceExtension.extensionName, userRequDeviceExtension)) {
					matched_count++;
					continue;
				}
			}

			userRequ_count++;
		}
	
		if (userRequ_count == matched_count) {
			return true;
		}
		return false;
	}
	bool isDeviceSuitable(VkPhysicalDevice phyDevice) {
		
		VkPhysicalDeviceProperties PhyDeProperties{};
		vkGetPhysicalDeviceProperties(phyDevice, &PhyDeProperties);

		VkPhysicalDeviceFeatures PhyDeFeatures{};
		vkGetPhysicalDeviceFeatures(phyDevice, &PhyDeFeatures);

		//QueueFamilyIndices QFIndices;		// // to member variable.
		QFIndices = findQueueFamilies(phyDevice);		// this->physicalDevice ����ϸ� �ȵ�!	this->physicalDevice�� pick �Ǳ� ���� �� �Լ��� ���� �� �����Ƿ�!

		bool isSuitable = false;
		if ( (PhyDeProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			 &&  (PhyDeFeatures.geometryShader) 
			 &&  (PhyDeFeatures.samplerAnisotropy)
			 &&  (QFIndices.graphicsQueueFamilyIndex.has_value()) 
			 &&  (QFIndices.presentQueueFamilyIndex.has_value() )
			 &&  (QFIndices.computeQueueFamilyIndex.has_value() ) )
		{
			isSuitable = true;	
			std::cout << "is SuitablePhysicalDevice found: (bool)" << isSuitable << std::endl;
		}
		else {		// for test.
			std::cout << "false" << std::endl;
		}

		bool isDeviceExtensionsSupported = checkUserRequDeviceExtensionsSupported(phyDevice);		// for checking if the swapchain deviceExtension is supported.
		std::cout << "is user requested DeviceExtensions Supported?: (bool)" << isDeviceExtensionsSupported << std::endl;

		return isSuitable && isDeviceExtensionsSupported;
	}

	// VkShaderModule createShaderModule(const char* fileName, const char* shaderType) {
		
	// 	std::ifstream inputFile;
	// 	inputFile.open(fileName, std::ios::ate);
	// 	if (inputFile.fail() || (!inputFile.is_open())) {
	// 		throw std::runtime_error("failed to open a shader file.");
	// 	}

	// 	uint32_t fileSize = static_cast<uint32_t>(inputFile.tellg());
	// 	std::vector<char> sourceTextBuffer(fileSize, 0);
	// 	//char sourceTextBuffer[3000] = {""};			// �� �迭�� ����� ũ�� �����ָ�, "syntax error, unexpected end of file" ��� �ϸ鼭, ������ �����ϸ� ������.

	// 	inputFile.seekg(0);		// ������ ��ó�� ��ġ�� �̵�.
	// 	inputFile.read(sourceTextBuffer.data(), SOURCE_TEXT_MAX_SIZE);
		
	// 	shaderc::Compiler compiler;
	// 	shaderc::CompileOptions options;
	// 	//shaderc::CompilationResult compileResult;
		
	// 	//shaderc_shader_kind vertex_shader = shaderc_glsl_default_vertex_shader;
	// 	//if (gl_enableValidationLayers == true) {
	// 		options.SetGenerateDebugInfo();
	// 	//}

	// 	//CompileGlslToSpv(const std::string & source_text,
	// 	//				 shaderc_shader_kind shader_kind,
	// 	//				 const char* input_file_name,
	// 	//				 const char* entry_point_name,
	// 	//				 const CompileOptions & options)
	// 	// compiler.PreprocessGlsl() �����ְ�, �ٷ� compiler.CompileGlslToSpv() ���൵ ��.
	// 	// SPIR-V makes use of shader and program objects. Because SPIR-V is a binary format.
	// 	shaderc::SpvCompilationResult compileResult;
	// 	if (shaderType == "vert") {
	// 		compileResult = compiler.CompileGlslToSpv(sourceTextBuffer.data(), shaderc_glsl_vertex_shader, fileName, "main", options);
	// 		std::cout << "Vertex shader is created." << std::endl;
	// 	} 
	// 	else if (shaderType == "frag") {
	// 		compileResult = compiler.CompileGlslToSpv(sourceTextBuffer.data(), shaderc_glsl_fragment_shader, fileName, "main", options);
	// 		std::cout << "Fragment shader is created." << std::endl;
	// 	}
	// 	else if (shaderType == "comp") {
	// 		compileResult = compiler.CompileGlslToSpv(sourceTextBuffer.data(), shaderc_glsl_compute_shader, fileName, "main", options);
	// 		std::cout << "Compute shader is created." << std::endl;
	// 	}
	// 	else {
	// 		throw std::runtime_error("shader type is wrong.");
	// 	}

	// 	if (compileResult.GetCompilationStatus() != shaderc_compilation_status_success) {
	// 		std::cerr << compileResult.GetErrorMessage() << std::endl;
	// 	}

	// 	// shaderc::CompilationResult �� shaderc::SpvCompilationResult �� ���̴�, 
	// 	// using SpvCompilationResult = CompilationResult<uint32_t>; �� �� �����ش�.
	// 	if ((compileResult.cbegin() == nullptr)  ||  (compileResult.cbegin() == nullptr)) {
	// 		throw std::runtime_error("failed to create SPIR-V compiled files.");
	// 	}
		
	// 	// �� ������� compile�� ���̳ʸ� ��������͸� �����ϴ� ����� �˾Ƴ��� ����, �� ���̺귯���� �������� �����ڵ带 ���� ������, �˱� ���� ������.
	// 	std::vector<uint32_t> shaderModuleCode{compileResult.cbegin(), compileResult.cend()};

	// 	VkShaderModuleCreateInfo shaderModuleCreateInfo{};		// �� struct �����Ҷ� �ʱ�ȭ �ݵ�� ����� ��.
	// 	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	// 	shaderModuleCreateInfo.codeSize = shaderModuleCode.size() * size_t(sizeof(uint32_t) / sizeof(char));
	// 	shaderModuleCreateInfo.pCode = shaderModuleCode.data();
	// 	//shaderModuleCreateInfo.flags = 0;		

	// 	VkShaderModule resultShaderModule;
	// 	if (vkCreateShaderModule(this->device, &shaderModuleCreateInfo, nullptr, &resultShaderModule) != VK_SUCCESS) {
	// 		throw std::runtime_error("failed to create shader modules.");
	// 	}
		
	// 	return resultShaderModule;
	// }
	VkShaderModule createShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }



	void cleanupSwapChain() {

		vkDestroyImageView(this->device, this->depthImageView, nullptr);
		vkFreeMemory(this->device, this->depthImageMemory, nullptr);
		vkDestroyImage(this->device, this->depthImage, nullptr);
		for (size_t i = 0; i < (this->framebuffers).size(); i++)
		{
			vkDestroyFramebuffer(device, (this->framebuffers)[i], nullptr);
		}
		for (auto& swapChainImageView : this->swapChainImageViews) {
			vkDestroyImageView(this->device, swapChainImageView, nullptr);
		}
		vkDestroySwapchainKHR(this->device, this->swapChain, nullptr);

	}
	void recreateSwapChain() {
		
		// for handling minimization of window.
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		while (width == 0 || height == 0) {
			glfwWaitEvents();		// pausing until the window is in the foreground again
			glfwGetFramebufferSize(window, &width, &height);
		}
		vkQueueWaitIdle(this->Queues[0]);

		//cleanupSwapChain();
		vkDestroyImageView(this->device, this->depthImageView, nullptr);
		vkFreeMemory(this->device, this->depthImageMemory, nullptr);
		vkDestroyImage(this->device, this->depthImage, nullptr);
		for (size_t i = 0; i < (this->framebuffers).size(); i++)
		{
			vkDestroyFramebuffer(device, (this->framebuffers)[i], nullptr);
		}
		for (auto& swapChainImageView : this->swapChainImageViews) {
			vkDestroyImageView(this->device, swapChainImageView, nullptr);
		}
		vkDestroySwapchainKHR(this->device, this->swapChain, nullptr);

		createSwapChain();
		createSwapChainImageViews();
		createDepthResources();
		createFrameBuffers();
	}

	// �����ϰų� ����Ҹ��� �Լ���. �׷��� �����Լ����� �����ִ� �Լ��� ////////////////////////////
	void copyBuffer_(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		
		VkCommandBuffer tempCommandBuffer;
		
		VkCommandBufferAllocateInfo comBuffAllocInfo{};
		comBuffAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		comBuffAllocInfo.commandPool = this->gctCommandPool;
		comBuffAllocInfo.commandBufferCount = 1;
		comBuffAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		vkAllocateCommandBuffers(this->device, &comBuffAllocInfo, &tempCommandBuffer);



		VkCommandBufferBeginInfo comBeginInfo{};
		comBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		comBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(tempCommandBuffer, &comBeginInfo);

		VkBufferCopy bufferCopyInfo{};
		bufferCopyInfo.srcOffset = 0;
		bufferCopyInfo.dstOffset = 0;
		bufferCopyInfo.size = size;
		vkCmdCopyBuffer(tempCommandBuffer, srcBuffer, dstBuffer, 1, &bufferCopyInfo);

		vkEndCommandBuffer(tempCommandBuffer);



		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &tempCommandBuffer;

		vkQueueSubmit(this->Queues[0], 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(this->Queues[0]);



		vkFreeCommandBuffers(device, this->gctCommandPool, 1, &tempCommandBuffer);
	}
	VkFormat findSupportedDepthImageFormat(const std::vector<VkFormat>& candidates, 
									  VkImageTiling tiling, VkFormatFeatureFlags requestedFormatFeatures) {
		for (VkFormat format : candidates)
		{
			VkFormatProperties props{};
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
			if (tiling == VK_IMAGE_TILING_OPTIMAL) {
				if ((props.optimalTilingFeatures & requestedFormatFeatures) == requestedFormatFeatures) {
					return format;
				}
			} 
			else if (tiling == VK_IMAGE_TILING_LINEAR) {
				if ((props.linearTilingFeatures & requestedFormatFeatures) == requestedFormatFeatures) {
					return format;
				}
			} 
			else {
				throw std::runtime_error("tiling argument is wrong.");
			}
		}

		throw std::runtime_error("There is no such image format that has such feature.");
	}
	////////////////////////////////////////

	uint32_t findMemoryTypeIndex(uint32_t memTypeBits, VkMemoryPropertyFlags requiringMemProperties) {

		VkPhysicalDeviceMemoryProperties memProperties{};
		vkGetPhysicalDeviceMemoryProperties(this->physicalDevice, &memProperties);

		uint32_t memTypeIndex = 0;
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((memTypeBits & (1 << i))
				&& ((memProperties.memoryTypes[i].propertyFlags & requiringMemProperties) == requiringMemProperties)
				)
			{
				memTypeIndex = i;
			}
		}

		return memTypeIndex;

	}
	void createAllocBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory, const VkDeviceSize& dataSize,
		const VkMemoryPropertyFlags& userRequMemProperties,
		const VkBufferUsageFlags& usage) 
	{
		VkBufferCreateInfo buffCreateInfo{};
		buffCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffCreateInfo.usage = usage;		// VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		buffCreateInfo.size = static_cast<VkDeviceSize>(dataSize);
		buffCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(this->device, &buffCreateInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create a buffer.");
		}



		VkMemoryRequirements memRequirements{};
		vkGetBufferMemoryRequirements(this->device, buffer, &memRequirements);

		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memRequirements.size;
		memAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits,
			userRequMemProperties);

		if (vkAllocateMemory(this->device, &memAllocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate staging buffer moemory for density.");
		}
	}
	void bindBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		if (vkBindBufferMemory(this->device, buffer, bufferMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate staging buffer moemory for density.");
		}
	}
	void createAllocBindBuffer(VkBuffer& buffer, VkDeviceMemory& bufferMemory, const VkDeviceSize& dataSize,
					  const VkMemoryPropertyFlags& userRequMemProperties,
					  const VkBufferUsageFlags& usage) {
		
		VkBufferCreateInfo buffCreateInfo{}; 
		buffCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffCreateInfo.usage = usage;		// VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		buffCreateInfo.size = static_cast<VkDeviceSize>(dataSize);
		buffCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(this->device, &buffCreateInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create a buffer.");
		}	



		VkMemoryRequirements memRequirements{};
		vkGetBufferMemoryRequirements(this->device, buffer, &memRequirements);

		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memRequirements.size;
		memAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits,
														   userRequMemProperties);														   

		if (vkAllocateMemory(this->device, &memAllocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate staging buffer moemory for density.");
		}	



		if (vkBindBufferMemory(this->device, buffer, bufferMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate staging buffer moemory for density.");
		}	



		std::cout << "A buffer is created, allocated, and bound." << std::endl;

	}

	VkFormat findSupportedImageFormat(const std::vector<VkFormat>& userRequFormats,
											 VkFormatFeatureFlags userRequFormatFeatures,
											 VkImageType imageType, VkImageUsageFlags usage) {
		
		VkFormat possibleFormat;
		for (const VkFormat& format : userRequFormats)
		{
			std::cout << std::endl;
			std::cout << "user-requesting ImageFormat: " << format;



			//VkPhysicalDeviceImageFormatInfo2 info2{};
			//info2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGE_FORMAT_INFO_2;
			//info2.format = format;
			//info2.tiling = VK_IMAGE_TILING_OPTIMAL;
			//info2.usage = usage;
			//info2.type = imageType;

			//VkImageFormatProperties2 props2{};
			//props2.sType = VK_STRUCTURE_TYPE_IMAGE_FORMAT_PROPERTIES_2;

			//vkGetPhysicalDeviceImageFormatProperties2(physicalDevice, &info2, &props2);
			//std::cout << "FormatProperties.maxExtent.width: " << props2.imageFormatProperties.maxExtent.width << std::endl;
			//std::cout << "FormatProperties.maxExtent.height: " << props2.imageFormatProperties.maxExtent.height << std::endl;
			//std::cout << "FormatProperties.maxExtent.depth: " << props2.imageFormatProperties.maxExtent.depth << std::endl;
			////props.imageFormatProperties.

			VkImageFormatProperties imgProps{};
			vkGetPhysicalDeviceImageFormatProperties(physicalDevice,
				format, imageType, VK_IMAGE_TILING_OPTIMAL,
				usage, 0,
				&imgProps);

				// According to Vulkan spec says,
				//typedef enum VkImageCreateFlagBits {
				//	VK_IMAGE_CREATE_SPARSE_BINDING_BIT = 0x00000001,
				//	VK_IMAGE_CREATE_SPARSE_RESIDENCY_BIT = 0x00000002,
				//	VK_IMAGE_CREATE_SPARSE_ALIASED_BIT = 0x00000004,
				//	VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT = 0x00000008,
				//	VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT = 0x00000010,
				//	���
				//}



			VkFormatProperties props{};
			vkGetPhysicalDeviceFormatProperties(physicalDevice,
				format, &props);

				// According to Vulkan spec says,
				// VkFormatFeatureFlagBits - Bitmask specifying features supported by a buffer
				// Bits which can be set in the VkFormatProperties features linearTilingFeatures, optimalTilingFeatures,
				//typedef enum VkFormatFeatureFlagBits {
				//	VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT = 0x00000001,
				//	VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT = 0x00000002,
				//	VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT = 0x00000004,
				//	VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT = 0x00000008,
				//	VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT = 0x00000010,
				//	VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT = 0x00000020,
				//	VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT = 0x00000040,
				//	VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT = 0x00000080,
				//	VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT = 0x00000100,
				//	VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT = 0x00000200,
				//	VK_FORMAT_FEATURE_BLIT_SRC_BIT = 0x00000400,
				//	VK_FORMAT_FEATURE_BLIT_DST_BIT = 0x00000800,
				//	VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT = 0x00001000
				//}



			if (imgProps.maxExtent.width > 0) {
				possibleFormat = format;
				std::cout << ": Possible Format." << std::endl;
			}
			else {
				std::cout << std::endl;
			}

			std::cout << "FormatProperties.maxExtent.width: " << imgProps.maxExtent.width << std::endl;
			std::cout << "FormatProperties.maxExtent.height: " << imgProps.maxExtent.height << std::endl;
			std::cout << "FormatProperties.maxExtent.depth: " << imgProps.maxExtent.depth << std::endl;
			std::cout << "FormatProperties.maxArrayLayers: " << imgProps.maxArrayLayers << std::endl;
			std::cout << "FormatProperties.maxMipLevels: " << imgProps.maxMipLevels << std::endl;
			std::cout << "FormatProperties.maxResourceSize: " << imgProps.maxResourceSize << std::endl;
			std::cout << "FormatProperties.sampleCounts: " << imgProps.sampleCounts << std::endl;

			if (imgProps.maxExtent.width > 0) {
				return possibleFormat;
			}
		}



		throw std::runtime_error("There is no such image format supported with such feature.");
	}
	void createAllocBindImage(VkImage& image, VkDeviceMemory& imageMemory,
					 const VkImageType& imageType, const VkExtent3D& extent, 
					 const std::vector<VkFormat>& userRequFormats, const VkFormatFeatureFlags& userRequFormatFeatures,
					 const VkImageUsageFlags& usage) {
		
		VkFormat possibleFormat;	
		possibleFormat = findSupportedImageFormat(userRequFormats, userRequFormatFeatures, imageType, usage);
		this->storageImageFormat = possibleFormat;
		


		VkImageCreateInfo imgCreateInfo{};
		imgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imgCreateInfo.usage = usage;			//VK_IMAGE_USAGE_STORAGE_BIT;
		imgCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imgCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imgCreateInfo.mipLevels = 1;
		imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imgCreateInfo.imageType = imageType;		// VK_IMAGE_TYPE_3D;
		imgCreateInfo.format = possibleFormat;	// or VK_FORMAT_R32G32B32A32_SFLOAT;
		//VkExtent3D Extent = { (uint32_t)STORAGE_IMAGE_WIDTH, (uint32_t)STORAGE_IMAGE_HEIGHT, (uint32_t)STORAGE_IMAGE_DEPTH };
		imgCreateInfo.extent = extent;
		imgCreateInfo.arrayLayers = 1;	// Cube maps, Animated textures, and texture arrays of shadow maps and other similar collections. � ���̴� �ĸ����ʹ�.
		//storageImgCreateInfo.flags = VK_IMAGE_CREATE_SPARSE_BINDING_BIT;	// image���� sparse��� ��������.

		if (vkCreateImage(this->device, &imgCreateInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create an image.");
		}	



		VkMemoryRequirements memRequirements{};
		vkGetImageMemoryRequirements(this->device, image, &memRequirements);

		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memRequirements.size;
		memAllocInfo.memoryTypeIndex = findMemoryTypeIndex(memRequirements.memoryTypeBits,
														   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// memory property > memType > heapType
		// heapType: GPU ���� heap�޸�����(= DEVICE_LOCAL), �ý���(CPU) ���� heap�޸� ����(= not DEVICE_LOCAL).
		// memType: DEVICE_LOCAL, HOST_VISIBLE, HOST_CACHED, HOST_COHERENT �����, �Ҵ�� �޸��� Ư���� ����, �з��Ǵ� �޸�����. 
		// ���� ū ���ش� �޸� ������ ���������� �и��Ǿ� �ִٴ� ���̴�. �׷��� �ʴ�. �޸��� '������ �и�'�� '������ �и�'�� �����ؾ� �Ѵ�.
		// '�޸� type'��, ����ڰ� ���� �޸� �Ҵ����� ���뵵�� ����, '���������� �и�'�ϴ� �Ϳ� �ش��ϰ�,
		// 'heap type'��, ���� �޸��� '������������ �и�'�ϴ� �Ϳ� �ش��Ѵ�.
		// �޸� type��, ����̹��� ���޾�ü�� ���� �ٸ�, �׵鸸�� �˰��ִ�, ���� ���� �����̸�, ������/���޾�ü���� Ȯ���� �˰� �ִ� �����̴�.
		// ���� �츮�� vulkan�� query�Լ���, query�Ͽ�, �޸�typeIndex�� �˾Ƴ��� �� �ۿ� ����.
		VkPhysicalDeviceMemoryProperties PhyDeMemProperty{};
		vkGetPhysicalDeviceMemoryProperties(this->physicalDevice, &PhyDeMemProperty);
		uint32_t heapIndex = PhyDeMemProperty.memoryTypes[memAllocInfo.memoryTypeIndex].heapIndex;
		std::cout << "PhyDeMemProperty.memoryHeaps[heapIndex].size: " << PhyDeMemProperty.memoryHeaps[heapIndex].size << std::endl;
		// ��ü heap�޸𸮸� �����ִ� ��ü���� 12703498240����Ʈ�ε�,
		// �� image�� �����ִ� heap�޸� ���� 224395264�ε�. 
		// ���� �뵵, �޸�Ÿ���� storage image��, �� GPU�޸𸮿�����, �� 56.612�� ���������� �Ҵ簡���ϳ�.

		if (vkAllocateMemory(this->device, &memAllocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image Memory.");
		}	std::cout << "image memory is allocated" << std::endl;



		if (vkBindImageMemory(this->device, image, imageMemory, 0) != VK_SUCCESS) {
			throw std::runtime_error("failed to bind image and imageMemory.");
		}	



		std::cout << "An image is created, allocated, and bound." << std::endl;

	}
	void createImageView(VkImageView& imageView,
						 VkImage& image, const VkFormat& imageFormat, const VkImageViewType& imageViewType) {

		VkImageViewCreateInfo storageImgViewCreateInfo{};
		storageImgViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		storageImgViewCreateInfo.image = image;
		storageImgViewCreateInfo.format = imageFormat;		// VK_FORMAT_R32_SFLOAT;
		storageImgViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		storageImgViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		storageImgViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		storageImgViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		storageImgViewCreateInfo.viewType = imageViewType;		// VK_IMAGE_VIEW_TYPE_3D;
		storageImgViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		storageImgViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		storageImgViewCreateInfo.subresourceRange.layerCount = 1;
		storageImgViewCreateInfo.subresourceRange.baseMipLevel = 0;
		storageImgViewCreateInfo.subresourceRange.levelCount = 1;

		if (vkCreateImageView(this->device, &storageImgViewCreateInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create density image view.");
		}	std::cout << "density image view is created." << std::endl;

	}
	void createImageSampler(VkSampler& sampler) {

		VkSamplerCreateInfo samplerCreateInfo{};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;		// = 0;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		samplerCreateInfo.anisotropyEnable = VK_FALSE;
		//storageSamplerCreateInfo.maxAnisotropy = ;
		// �Ʒ��� depth buffer sampler�� ����Ǵ� ����.
		//samplerInfo.compareEnable = ;
		//samplerInfo.compareOp = ;

		if (vkCreateSampler(this->device, &samplerCreateInfo, nullptr, &sampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create a sampler.");
		}	std::cout << "A sampler is created." << std::endl;

	}

	void allocBeginComBuff(VkCommandBuffer& tempComBuff) {

		VkCommandBufferAllocateInfo temComBuffCreateInfo{};
		temComBuffCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		temComBuffCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		temComBuffCreateInfo.commandPool = this->gctCommandPool;
		temComBuffCreateInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(this->device, &temComBuffCreateInfo, &tempComBuff) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate temp command buffer.");
		}



		VkCommandBufferBeginInfo comBuffBeginInfo{};							// ���⼭ {} �̰� ���ߴٰ� vkBeginCommandBuffer���� ����������. �̶� �����޽����� �ȳ����� �׳� ���α׷��̲����� ���� ã�µ� �������. struct�� �� ���������� �� ������.
		comBuffBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		comBuffBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(tempComBuff, &comBuffBeginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin command buffer.");
		}

	}
	void cmdBuffMemBarrier(VkCommandBuffer& tempComBuff, VkBuffer& accessedBuffer,
						   const VkDeviceSize& offset, const VkDeviceSize& size,
						   const VkAccessFlags& srcAccessMask, const VkAccessFlags& dstAccessMask,
						   const VkPipelineStageFlags& srcStage, const VkPipelineStageFlags& dstStage) {

		VkBufferMemoryBarrier buffMemBarrier{};
		buffMemBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		buffMemBarrier.buffer = accessedBuffer;
		buffMemBarrier.offset = offset;
		buffMemBarrier.size = size;
		buffMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		buffMemBarrier.srcAccessMask = srcAccessMask;
		buffMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		buffMemBarrier.dstAccessMask = dstAccessMask;

		vkCmdPipelineBarrier(tempComBuff,
			srcStage, dstStage,
			0,
			0, nullptr,
			1, &buffMemBarrier,
			0, nullptr);

	}
	void cmdImgMemBarrier(VkCommandBuffer& tempComBuff, VkImage& accessedImage,
						  const VkImageLayout& oldLayout, const VkImageLayout& newLayout,
						  const VkAccessFlags& srcAccessMask, const VkAccessFlags& dstAccessMask,
						  const VkPipelineStageFlags& srcStage, const VkPipelineStageFlags& dstStage) {

		VkImageMemoryBarrier imgMemBarrier{};
		imgMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imgMemBarrier.image = accessedImage;
		// shared memory ��� ����, ���� �Ķ�����ε�.
		imgMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imgMemBarrier.oldLayout = oldLayout;
		imgMemBarrier.srcAccessMask = srcAccessMask;
		imgMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imgMemBarrier.newLayout = newLayout;
		imgMemBarrier.dstAccessMask = dstAccessMask;
		// VK_IMAGE_ASPECT_METADATA_BIT specifies the metadata aspect used for sparse resource operations
		imgMemBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imgMemBarrier.subresourceRange.baseArrayLayer = 0;
		imgMemBarrier.subresourceRange.layerCount = 1;
		imgMemBarrier.subresourceRange.baseMipLevel = 0;
		imgMemBarrier.subresourceRange.levelCount = 1;

		vkCmdPipelineBarrier(tempComBuff,
			srcStage, dstStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &imgMemBarrier);

	}	
	void cmdCopyBuffer(VkCommandBuffer& tempComBuff,
		VkBuffer& srcBuffer, VkBuffer& dstBuffer,
		const VkDeviceSize& srcOffset, const VkDeviceSize& dstOffset,
		const VkDeviceSize& bufferDataSize)
	{

		VkBufferCopy bufferCopyInfo{};
		bufferCopyInfo.srcOffset = srcOffset;
		bufferCopyInfo.dstOffset = dstOffset;
		bufferCopyInfo.size = bufferDataSize;
		
		vkCmdCopyBuffer(tempComBuff, srcBuffer, dstBuffer, 1, &bufferCopyInfo);

	}
	void cmdCopyImage(VkCommandBuffer& tempComBuff,
		VkImage& srcImage, VkImage& dstImage,
		const VkImageLayout& srcImageLayout, const VkImageLayout& dstImageLayout,
		const VkExtent3D& imageDataExtent) 
	{
		VkOffset3D offset = { (uint32_t)0, (uint32_t)0, (uint32_t)0};

		VkImageCopy copyRegion{};
		copyRegion.extent = imageDataExtent;
		copyRegion.srcOffset = offset;
		copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.srcSubresource.baseArrayLayer = 0;
		copyRegion.srcSubresource.layerCount = 1;
		copyRegion.srcSubresource.mipLevel = 0;
		copyRegion.dstOffset = offset;
		copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.dstSubresource.baseArrayLayer = 0;
		copyRegion.dstSubresource.layerCount = 1;
		copyRegion.dstSubresource.mipLevel = 0;

		vkCmdCopyImage(tempComBuff, 
			srcImage, srcImageLayout,
			dstImage, dstImageLayout,
			1, &copyRegion);
	}
	void cmdCopyBufferToImage(VkCommandBuffer tempComBuff,
						      VkBuffer& buffer, VkImage& image,	//const VkDeviceSize& bufferDataSize, 
							  const VkDeviceSize& bufferOffset,
							  const VkOffset3D& imageOffset, const VkExtent3D& imageDataExtent) 
	{
			VkBufferImageCopy copyRegion{};
			copyRegion.bufferOffset = 0;
			// bufferRowLength�� bufferImageHeight �Ķ���ʹ�, �̹��������� ũ�Ⱑ ����ũ�⺸�� ���� ��, �׷��� ���� �޸� ������ ��������, �ۼ��ϴ� ������ ��.
			// �� �Ķ���Ͱ� 0�̸�, ���۰����� �����������, �̹��������ͷ� �� ���ִٴ� �ǹ�.
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;
			copyRegion.imageOffset = imageOffset;
			copyRegion.imageExtent = imageDataExtent;
			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageSubresource.mipLevel = 0;

			vkCmdCopyBufferToImage(tempComBuff, buffer, image,
								   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
								   1, &copyRegion);

	}
	void endSubmitFreeComBuff(VkCommandBuffer& tempComBuff) {

		if (vkEndCommandBuffer(tempComBuff) != VK_SUCCESS) {
			throw std::runtime_error("failed to end command buffer.");
		}

		VkSubmitInfo queueSubmitInfo{};
		queueSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		queueSubmitInfo.commandBufferCount = 1;
		queueSubmitInfo.pCommandBuffers = &tempComBuff;
		queueSubmitInfo.signalSemaphoreCount = 0;
		queueSubmitInfo.pSignalSemaphores = nullptr;
		queueSubmitInfo.waitSemaphoreCount = 0;
		queueSubmitInfo.pWaitSemaphores = nullptr;
		queueSubmitInfo.pWaitDstStageMask = nullptr;

		if (vkQueueSubmit(this->Queues[0], 1, &queueSubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit tempComBuff to Queue.");
		}

		vkQueueWaitIdle(this->Queues[0]);

		vkFreeCommandBuffers(this->device, this->gctCommandPool, 1, &tempComBuff);

	}
	
	





//3////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// member variables /////////////////////////////////////////////////////////////////////////////////////////////
private:
	void cleanup() {

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroyFence(this->device, this->inFlightFences[i], nullptr);
			vkDestroySemaphore(this->device, this->imageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(this->device, this->renderFinishedSemaphores[i], nullptr);
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkFreeCommandBuffers(this->device, gctCommandPool, 1, &gctCommandBuffers[i]);
		}

		vkDestroyPipeline(this->device, graphicsPipeline2, nullptr);
		for (size_t i = 0; i < graphicsShaderModules2.size(); i++)
		{
			vkDestroyShaderModule(this->device, this->graphicsShaderModules2[i], nullptr);
		}
		vkDestroyPipeline(this->device, graphicsPipeline, nullptr);
		for (size_t i = 0; i < graphicsShaderModules.size(); i++)
		{
			vkDestroyShaderModule(this->device, this->graphicsShaderModules[i], nullptr);
		}
		for (size_t i = 0; i < this->computePipelines.size(); i++)
		{
			vkDestroyPipeline(this->device, this->computePipelines[i], nullptr);
		}
		for (size_t i = 0; i < computeShaderModules.size(); i++)
		{
			vkDestroyShaderModule(this->device, computeShaderModules[i], nullptr);
		}
		vkDestroyPipelineLayout(this->device, this->gctPipelineLayout, nullptr);

		for (size_t i = 0; i < framebuffers.size(); i++)
		{
			vkDestroyFramebuffer(device, (this->framebuffers)[i], nullptr);
		}

		vkDestroyRenderPass(this->device, this->renderPass, nullptr);

		//
		// You don't need to explicitly clean up descriptor sets, because they will be automatically freed 
		// when the descriptor pool is destroyed.
		//vkDestroyDescriptorSet�� pool�� destroy�ɶ� �Բ� �ڵ����� destory�Ǵµ�. descriptor�� ��������.
		vkDestroyDescriptorPool(this->device, this->descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(this->device, this->descriptorSetLayout, nullptr);
		//
		vkFreeMemory(this->device, this->indexBuffer2Memory, nullptr);
		vkDestroyBuffer(this->device, this->indexBuffer2, nullptr);
		vkFreeMemory(this->device, this->indexBufferMemory, nullptr);
		vkDestroyBuffer(this->device, this->indexBuffer, nullptr);
		vkFreeMemory(this->device, this->vertexBufferMemory, nullptr);
		vkDestroyBuffer(this->device, this->vertexBuffer, nullptr);
		//////
		// BC
		for (size_t i = 0; i < this->vorticity.size(); i++)
		{
			vkDestroyImageView(this->device, this->vorticityViews[i], nullptr);
			vkFreeMemory(this->device, this->vorticityMemories[i], nullptr);
			vkDestroyImage(this->device, this->vorticity[i], nullptr);
		}
		vkDestroyImageView(this->device, this->BCViews, nullptr);
		vkFreeMemory(this->device, this->BCMemories, nullptr);
		vkDestroyImage(this->device, this->BC, nullptr);
		//
		vkDestroySampler(this->device, this->velocityDensitySampler, nullptr);
		//
		for (size_t i = 0; i < this->velocityDensity.size(); i++)
		{	
			// divergencePressureTemp
			vkDestroyImageView(this->device, this->divergencePressureTempViews[i], nullptr);
			vkFreeMemory(this->device, this->divergencePressureTempMemories[i], nullptr);
			vkDestroyImage(this->device, this->divergencePressureTemp[i], nullptr);
			// divergencePressure
			vkDestroyImageView(this->device, this->divergencePressureViews[i], nullptr);
			vkFreeMemory(this->device, this->divergencePressureMemories[i], nullptr);
			vkDestroyImage(this->device, this->divergencePressure[i], nullptr);
			// velocityDensityTemp
			vkDestroyImageView(this->device, this->velocityDensityTempViews[i], nullptr);
			vkFreeMemory(this->device, this->velocityDensityTempMemories[i], nullptr);
			vkDestroyImage(this->device, this->velocityDensityTemp[i], nullptr);
			// velocityDensity
			//vkDestroySampler(this->device, this->velocityDensitySampler, nullptr);
			vkDestroyImageView(this->device, this->velocityDensityViews[i], nullptr);
			vkFreeMemory(this->device, this->velocityDensityMemories[i], nullptr);
			vkDestroyImage(this->device, this->velocityDensity[i], nullptr);
		}
		//////
		// densityCopy and densityCopySampler
		vkDestroySampler(this->device, this->densityCopySampler, nullptr);
		vkDestroyImageView(this->device, this->densityCopyView, nullptr);
		vkFreeMemory(this->device, this->densityCopyMemory, nullptr);
		vkDestroyImage(this->device, this->densityCopy, nullptr);
		// density
		vkDestroyImageView(this->device, this->densityView, nullptr);
		vkFreeMemory(this->device, this->densityMemory, nullptr);
		vkDestroyImage(this->device, this->density, nullptr);
		//
		vkDestroyImageView(this->device, this->depthImageView, nullptr);
		vkFreeMemory(this->device, this->depthImageMemory, nullptr);
		vkDestroyImage(this->device, this->depthImage, nullptr);
		//
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkUnmapMemory(this->device, this->uniformBufferMemories[i]);
			vkFreeMemory(this->device, this->uniformBufferMemories[i], nullptr);
			vkDestroyBuffer(this->device, this->uniformBuffers[i], nullptr);
		}
		//

		//cleanupSwapChain();
		for (auto& swapChainImageView : swapChainImageViews) {		// MAX_FRAMES_IN_FLIGHT
			vkDestroyImageView(this->device, swapChainImageView, nullptr);
		}
		vkDestroySwapchainKHR(this->device, this->swapChain, nullptr);

		vkDestroyCommandPool(this->device, gctCommandPool, nullptr);

		//vkDestroyQueue�� ���� ���� ��!
		vkDestroyDevice(this->device, nullptr);
		//vkDestroyPhysicalDevice �Լ��� ���� ����.	// vkEnumeratePhysicalDevices()�Լ� ����Ҷ�, �̹� PhysicalDevice�� instance�� ��ũ�Ǿ, instance�� destroy�ɶ�, PhysicalDevice�� ���� �ڵ����� ��ȯ�Ǵµ�.

		vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
		if (gl_enableValidationLayers) {
			this->destroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, nullptr);
			// The vkCreateDebugUtilsMessengerEXT call requires a valid instance to have been created 
			// and vkDestroyDebugUtilsMessengerEXT must be called before the instance is destroyed.
			// This currently leaves us unable to debug any issues in the vkCreateInstance and vkDestroyInstance calls.
			// It requires you to simply pass a pointer to a VkDebugUtilsMessengerCreateInfoEXT struct in the pNext extension field of VkInstanceCreateInfo.
		}
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(this->window);
		glfwTerminate();
	}



private:
	GLFWwindow* window;

	//std::vector<const char*> gl_userRequestingExtensions;		// move this variable to the global scale.
	///std::vector<const char*> gl_ValidationLayers;			// move this variable to the global scale.
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;

	QueueFamilyIndices QFIndices;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	std::vector<VkQueue> Queues;

	VkCommandPool gctCommandPool;

	VkFormat swapChainFormat;
	VkExtent2D swapChainExtent;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	
	//
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBufferMemories;
	std::vector<void*> uniformBuffersMapped;
	VkImage depthImage;
	VkFormat depthFormat;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
	//////
	VkFormat storageImageFormat;
	VkImage density;		// density = storageImage
	VkDeviceMemory densityMemory;
	VkImageView densityView;
	VkImage densityCopy;
	VkDeviceMemory densityCopyMemory;
	VkImageView densityCopyView;
	VkSampler densityCopySampler;
	//////
	std::vector<VkImage> velocityDensity;
	std::vector<VkDeviceMemory> velocityDensityMemories;
	std::vector<VkImageView> velocityDensityViews;
	VkSampler velocityDensitySampler;
	std::vector<VkImage> velocityDensityTemp;
	std::vector<VkDeviceMemory> velocityDensityTempMemories;
	std::vector<VkImageView> velocityDensityTempViews;
	std::vector<VkImage> divergencePressure;
	std::vector<VkDeviceMemory> divergencePressureMemories;
	std::vector<VkImageView> divergencePressureViews;
	std::vector<VkImage> divergencePressureTemp;
	std::vector<VkDeviceMemory> divergencePressureTempMemories;
	std::vector<VkImageView> divergencePressureTempViews;
	VkImage BC;
	VkDeviceMemory BCMemories;
	VkImageView BCViews;
	std::vector<VkImage> vorticity;
	std::vector<VkDeviceMemory> vorticityMemories;
	std::vector<VkImageView> vorticityViews;
	//////
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkBuffer indexBuffer2;
	VkDeviceMemory indexBuffer2Memory;
	//
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
	//

	VkRenderPass renderPass;
	
	std::vector<VkFramebuffer> framebuffers;
	
	VkPipelineLayout gctPipelineLayout;
	std::vector<VkShaderModule> computeShaderModules;
	std::vector<VkPipeline> computePipelines;
	VkViewport viewport{};
	VkRect2D scissor{};
	std::vector<VkShaderModule> graphicsShaderModules;
	VkPipeline graphicsPipeline;
	std::vector<VkShaderModule> graphicsShaderModules2;
	VkPipeline graphicsPipeline2;

	std::vector<VkCommandBuffer> gctCommandBuffers;

	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkFence> inFlightFences;
	uint32_t inFlightFrame = 0;
	// bool framebufferResized = false;			// public memeber ������ �ű�.

};



static void framebufferResizeCallbackFunc(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<vulkanStart2*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}



// main function //////////
int main(void) {

	vulkanStart2 app;

	//try {
		app.run(); 
	//}
	//catch (const std::exception& e) {
	//	std::cerr << e.what() << std::endl;
	//	return EXIT_FAILURE;
	//}
	
	return 0;
}