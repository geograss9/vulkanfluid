//#define VK_USE_PLATFORM_WIN32_KHR	// surface 만들때, platform-specific한 방법을 사용하든, glfw로 바로생성하는 방법을 사용하든, 해줘야함.
#include <vulkan/vulkan.hpp>
//#include </usr/include/vulkan/vulkan.hpp>

#define GLFW_INCLUDE_VULKAN		// 지금까지, 이 메크로 정의 안해도, 잘 되었음.
#include <GLFW/glfw3.h>
//#include </usr/include/GLFW/glfw3.h>
//#define GLFW_EXPOSE_NAIVE_WIN32	// surface 만들때, platform-specific한 방법을 사용하든, glfw로 바로생성하는 방법을 사용하든, 해줘야함.
//#include <GLFW/glfw3native.h>		// surface 만들때, platform-specific한 방법을 사용하든, glfw로 바로생성하는 방법을 사용하든, 해줘야함.
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
	"VK_LAYER_KHRONOS_validation"			// 동일한 대문자만으로 구성된 enum은 안만든듯.
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



#include <optional>			// 이 자료형은 surface에 호환되는 queueFamilyIndex를 찾기 위해 거의 사용하게 될수밖에없는 자료형이다.
#include <bitset>
#include <set>				// queueFamilyIndex가 겹치지 않게, queueCreateInfo를 생성하기위한, 자료형.

#include <fstream>
// #include <shaderc/shaderc.hpp>
#define SOURCE_TEXT_MAX_SIZE (10000)



const int MAX_FRAMES_IN_FLIGHT = 2;
static void framebufferResizeCallbackFunc(GLFWwindow* window, int width, int height);



//#include <array>
#define GLM_FORCE_RADIANS			// 함수들의 인수로, radian을 받도록하는 메크로인듯.
#define GLM_FORCE_DEPTH_ZERO_TO_ONE	// GLM will use the OpenGL depth range of -1.0 to 1.0 by default. We need to configure it to use the Vulkan range of 0.0 to 1.0 using the GLM_FORCE_DEPTH_ZERO_TO_ONE definition.
#include <glm/glm.hpp>
struct Vertex {
	alignas(16) glm::vec3 pos;		// alignas(16)에서 16은 byte를 의미함.
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



// uniform buffer 업데이트에 필요한 헤더파일들.
#include <glm/gtc/matrix_transform.hpp>		// glm::rotate()함수 쓰려면, 이거 include 해야함.	// we can use GLM_FORCE_RADIANS to force all GLM functions to adopt radians.
#include <glm/gtx/string_cast.hpp>			// mat4 문자로 출력해주는 함수 include 하기 위함.
#include <chrono>



#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>



// for generating particles.
#include <random> 		// #include "/usr/include/c++/13/random"
#define PARTICLE_COUNT (256)
struct Particle {
	alignas(16)	glm::vec3 position;		// 반드시 padding을 맞출것. 맞추니까 모든 particle의 position 업데이트가 다 잘되네. (패딩안했을때는 일부 파티클만 이상하게 움직였음.)
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
		// DebugCallbackFunc를 만들고,
		// 이 함수를, instance와 링크시킬 함수를 만든다. (vkCreateDebugUtilsMessenterEXT가 확장함수라, instance와 바로 연결시켜주면서 생성해주는 함수가 아니다.)
		setupDebugCallback();
		// Vulkan객체생성 방식으로, Surface를 만들기 위해서는, 추가로 header files를 더 include하는 방법도 있다.
		//			platform-specific extension (OS환경에 맞는, Vulkan의 '확장기능', 즉 'VK_KHR_win32_surface')을 이용하여 suface를 생성하는 방법도 있으나, ( 이경우, 'glfwGetRequiredInstanceExtensions' (instance생성할때 받았던 extension정보).를 이용하고, 더 include해야할 header파일이 있다. )
		//			SO환경 상관없이 구애받지않고, 어디서든, glfwCreateWindowSurface를 이용해 생성할수도 있다.
		//			platform-specific extension을 사용해, surface를 생성하는 방식을, glfw가, 자동으로 처리해주는 개념이다.
		createSurface();
		// createSurface()하는 것 자체가, instance에 의해, surface가 생성되는 것이므로, instnace가 당연히 surface를 지원하는 것을 의미함.
		// 후에 physicalDevice가 pick되면, Physical Device에 의해, surface가 지원되는지도 확인해보아야 한다. (presentation support by Queue Families)

		// Enumerate Physical Devices.
		//			vkEnumeratePhysicalDevices()함수 사용할때, instance가 링크가능한 PhysicalDevice를 찾아 나열하는 것이므로, 
		//			그때 이미 PhysicalDevice가 instance와 링크되어서, instance가 destroy될때, PhysicalDevice도 같이 자동으로 반환되는듯.
		// get properties and features of each physical devices.
		pickPhysicalDevice();
		// find QueueFamil'ies' on the picked physical device. 
		//			create struct for Queue Family index. 'findQueueFamilies' function's returnType is 'struct'.
		// 			find QueueFamilyIndices()함수에서는, queueFamilyIndex값을 중복으로 찾아, QFIndices에 넣을수있으나,
		// vkDeviceQueueCreateInfo에서는, 반면, 각 queue는, 그 queueFamilyIndex값이 서로 달라야 한다. 중복된 queueFamilyIndex값을 가지면 안된다. (Each queue must have a unique queueFamilyIndex within pQueueCreateInfos.) 
		// vkDeviceCreateInfo  (QueueCreateInfo들이 각각의 원소인 배열의 포인터를 여기에 넣어준다.)
		//			device만들때, device안에 있을 Queue들도 같이 한꺼번에 정해주고, 만들게된다.
		createDevice();
		// get a queue handle.
		// Physical Device에 의해, surface가 지원되는지도 확인해보아야 한다. (presentation support by Queue Families)
		// 후에, 스왑체인 만들때, device extension info 넣는 자리에, 스왑체인 extension 추가.
		
		createCommandPool();
		// command pool이 만들어지게 됨으로써, resource나 data를 만들때, 잠깐잠깐씩 임시로 사용할 command buffer를 만들어 사용하고 회수하도록 핧수있게되었다.
				
		// Checking for if swapchain device extension is supported.
		// if so, then, enable(turn on) the swapchain device extension, at the createDeviceInfo
		// Querying details of swap chain support.
		// 위에서 query해서 얻은 데이터를 swchCreateInfo에 넣은 후,
		createSwapChain();
		// Retrieve the handles of the VkImages in the Swapchain.
		createSwapChainImageViews();	// 각 image마다, 그에 대응하는 imageView를 만든다.
		
		//
		// 각종 버퍼를 만드는 과정에서, commandBuffer를 쓰기 때문에(특히 copyBuffer 명령어), 적어도 createCommandPool(); 이후에 아래 함수들이 위치해야 한다.
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
		// createAttachmentRef();	// attachment가 attachmentRef를 구성한다.
		// createSubPass();			// Subpass의 pAttachments 데이터로, attachment가 아닌, attachmentRef 데이터가 들어간다! 
		// SubpassDependency 작성.
		createRenderPass();			// pAttachments 데이터로, attachment 데이터가 들어간다.		// 여기 들어간 attachment array(pAttachment)의 원소순서 및 인덱스를 기준으로, attachmentRef가 attament를 참조한다.

		// 각 프레임마다 한장씩의 이미지 및 이미지뷰가 나오고, 그 각각의 이미지를 넣을수있도록, 프레임버퍼를 각각 따로 만든다.
		createFrameBuffers();

		//createDescriptorSetLayout();
		createPipelineLayout();		// pipelineLayout은 descriptorSetLayout과 호환되어야한다. 따라서 descriptorSetLayout이 만들어진후, pipelineLayout이 만들어져야할것이다.
		// compute pipeline은 여러단계의 compute shader stage 를 가질수 없고, 단 하나의 compute shader (stage)만을 가질수있다.
		createComputePipelines();	
		// viewport와 scissor 설정. viewportState 설정.
		// dynamic state 설정.
		// VertexInput State 설정.
		// InputAssembly State 설정.
		// Rasterization State 설정.
		// Multisample State 설정.
		// BlendAttachment State 및 BlendState(CreateInfo) 설정.
		// shaderc include 해서 사용하기. spir-v 컴파일하고 shaderModule 만들어 반환하는 함수 만들기.
		// shader Module 및 shaderStage 설정.
		// GraphicsPipelineCreateInfo에 위 정보들 모두 넣고, 생성.
		createGraphicsPipeline();
		createGraphicsPipeline2();

		createCommandBuffer();
		// commandBufferObject를 만들고, commandBufferMemory를 할당하고, Object와 Memory를 할당해야한다.
		// command buffer에 명령어를 입력하기 위해서는, vkBeginCommandBuffer()함수와 vkEndCommandBuffer()함수 사이에서, 명령어를 입력하는 함수를 실행시켜야한다.
		// vkCmd 로 시작하는 함수들을 통해, command buffer에, 명령어들을 입력하도록 한다.
		// compute pipeline을 수행하는 명령어는, 'BeginRenderPass와 BeginRenderPass 사이'에 위치하면 안되다. 오류난다.
		// graphics pipeline을 수행하는 명령어는, 'BeginRenderPass와 BeginRenderPass 사이'에 위치해야 하는 듯!
		// transfer pipeline stage를 수행하는 명령어는, 'BeginRenderPass와 BeginRenderPass 사이' 안이든 밖이든, 자유롭게 위치할수 있는 듯하다.
		//recordCommandBuffer		// drawFrame()함수의 보조함수로 위치시키고 작성.

		createSyncObject();

		//// 아래 함수는 mainLoop()함수 안에 들어가야함.
		////updateUniformBuffer();
		////recordCommandBuffer();
		//drawFrame();
		////

	}





// Functions in mainLoop() //////////////////////////////////////////////////////////////////////
private:	 
	void updateUniformBuffer(uint32_t inFlightFrame) {
		// 주로 uniformBuffer data를 업데이트 함.

		auto currentTime = std::chrono::high_resolution_clock::now();
		//std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
		
		//static std::chrono::steady_clock::time_point startTime = currentTime;
		static std::chrono::system_clock::time_point startTime = currentTime;		// 초기화 할때만 값이 들어감.
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
		//comBuffBeginInfo.pInheritanceInfo = nullptr;		// optional.	// 이 파라미터는, 해당 Begin 하는 버퍼가, second buffer 일때, 의미있는 정보.

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
			// clear해야할 attachment가, colorAttachment와 depthAttachment로, 이렇게 각각 2개 따로 있으므로,
			// VkClearValue객체도, 각각의 Attachment에 대응해, 따로 만들어 넣어주어야 한다.
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

			// 아래의 viewport 크기 정보입력하는 코드를 주석처리하면, window resize해도 화면크기가 안변하게되고, 
			// 주석처리 안하면, 매프레임마다 새로운 window resize 크기가 swapChain의 생성크기를 결정하여, 
			// 결국 그게 viewport 크기의 변화로 이어진다. 
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
			


			// vkCmdClearAttachments()함수는 renderPass 안에서 실행될수있음!
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
		// 0. 이전 프레임의 랜더링이 거의 끝나가면, 
		// 1. 다음(이번) 프레임의 랜더링을 준비(다음프레임의 imageIndex 받아오기, uniformBuffer 업데이트, commandBuffer의 작성, 제출)하고,
		//		이때, interrupt비슷하게, window가 resize되는 메시지가 오면, 이에 영향을 받는, swapChain에서 imageIndex를 받아오는 부분을, 잘 처리하는 부분 만들어야 함.
		// 2. 이전 프레임의 랜더링 결과image를, framebuffer? swapChain? 으로 present해야한다.
		//		이때, interrupt비슷하게, window가 resize되는 메시지가 오면, 이에 영향을 받는, presentation단계를, 잘 처리하는 부분 만들어야 함.
		std::cout << std::endl;		// 로그 출력용  한줄 띄어쓰기.



		// 0. 이 fence가 set되면(값이 1이되면), "이전 프레임의 랜더링이 거의 끝나가는 상황."을 의미하게됨. 즉, 이전프레임의 결과image가 present되기 시작하는 상황. 그러나 아직 swapChain이 화면에 출력을 시작하지는 않은 상황인듯.
		vkWaitForFences(this->device, 1, &(this->inFlightFences[this->inFlightFrame]), VK_TRUE, UINT64_MAX);



		// 1. 다음(이번) 프레임의 랜더링을 준비(다음프레임의 imageIndex 받아오기)
		uint32_t imageIndex;
		// The iamgeIndex refers to the VkImage in our swapChainImages array.
		// 정확히는, 이미지를 얻으려고 하는게 아니라, 다음 프레임에 image가 들어갈 index자리를 알아내기 위해, 하는거네.
		VkResult result = vkAcquireNextImageKHR(this->device, this->swapChain,
			UINT64_MAX, this->imageAvailableSemaphores[this->inFlightFrame],
			VK_NULL_HANDLE, &imageIndex);

		// window가 resize되면, swapChain을 recreate되도록 한 부분.
		// VK_ERROR_OUT_OF_DATE_KHR: The swap chain has become incompatible with the surface and can no longer be used for rendering. Usually happens after a window resize.
		// VK_SUBOPTIMAL_KHR: The swap chain can still be used to successfully present to the surface, but the surface properties are no longer matched exactly.
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;		// return함으로써, 이번 프레임을 다시 진행하기. (이번 프레임을 포기하지 않음.)
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to recreate swapChain and get images.");
		}

		// 위 코드블럭 앞에 이 fence를 reset시키는 코드가 있으면, 
		// swapchain을 recreate한 후, 부과되는 일 없이, 바로 다시 fence를 wait하게 되기 때문에, 
		// 부과된 일이 없어서, 영원히 fence가 1로 set되지 않는 일이 발생한다.
		// 이 때문에, recreate swapChain을 한후, fence를 0으로 reset하게 코딩함으로써, 무한wait를 막도록 한것.
		vkResetFences(this->device, 1, &(this->inFlightFences[this->inFlightFrame]));



		// 1. 다음(이번) 프레임의 랜더링을 준비(각종 Buffer, Image data 업데이트)
		updateUniformBuffer(this->inFlightFrame);



		// 1. 다음(이번) 프레임의 랜더링을 준비(commandBuffer의 작성)
		vkResetCommandBuffer(this->gctCommandBuffers[this->inFlightFrame], 0);

		// command buffer 를 (recodrf 를) begin 시키기위해, CommandBufferBeginInfo를 작성한다.그 후 시작한다.
		// renderPass 를 Begin 한다.
		// vkCmd 로 시작하는 함수들로, command buffer에, 명령들을 입력한다.
		recordCommandBuffer(this->gctCommandBuffers[this->inFlightFrame], imageIndex);



		// 1. 다음(이번) 프레임의 랜더링을 준비(commandBuffer의 제출)
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &(this->gctCommandBuffers[this->inFlightFrame]);
		//
		// a pointer to an array of VkSemaphore handles upon which to wait before the command buffers for this batch begin execution.
		// submit된 queue작업들이 시작되기 전에, set되어있어야할 semaphore.
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &(this->imageAvailableSemaphores[this->inFlightFrame]);
		//// queue가 submit되기 전에, 작업이 끝나있어야할 stage.
		VkPipelineStageFlags WaitDstStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		//// VkPipelineStageFlags는 enum숫자값을 담는 변수, VkPipelineStageFlagBits는 enum숫자값(enum constant).
		submitInfo.pWaitDstStageMask = WaitDstStages;
		//
		// a pointer to an array of VkSemaphore handles which will be signaled when the command buffers for this batch have completed execution.
		// submit된 queue작업들이 다 완료되면, set할 semaphore.
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &(this->renderFinishedSemaphores[this->inFlightFrame]);	// 즉, submit된 command buffer 에 입력된 명령이 다 실행완료되면, signal set될 semaphore를 지정한다!

		// The last parameter references an optional fence that will be signaled when the command buffers finish execution.
		// This allows us to know when it is safe for the command buffer to be reused.
		if (vkQueueSubmit(this->Queues[0], 1, &submitInfo, this->inFlightFences[this->inFlightFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit the command buffer.");
		}	std::cout << "command buffer is submitted." << std::endl;
		// this->Queues[0]는 graphicsQueueFamily의 queue를 의미함.



		// 2. 이전 프레임의 랜더링 결과image를, framebuffer? swapChain? 으로 present하는 단계.
		// present 란, pipeline이 일 완료헀을때, submitting the result back to the swap chain. 하는 것을 말함.
		// 그런데, present 실행은, CPU가 GPU에 매프레임마다 따로 신호를 줘서, 화면에 출력하게 하는 거였어? GPU가 계속 알아서 출력하는게 아니고?
		// 그게 아니라면, 왜, 왜 매프레임 drawFrame()함수가 실행될때마다, 아래 코드가 실행되게, 코드를 작성하지?
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &(this->swapChain);
		presentInfo.pImageIndices = &imageIndex;
		// the number of semaphores to wait for before issuing the present request. 먼말인지 참...
		// 즉, 결과이미지를 swapChain으로 제출하기 전에, wait으로 설정되어있어야 할, semaphore들을 설정하는게 맞는 것 같네... 
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &(this->renderFinishedSemaphores[this->inFlightFrame]);
		presentInfo.pResults = nullptr;		// swapChain이 presentation 성공하면 VK_SUCCESS 로 설정할, VkResult형 변수를 넣어주면 됨. 필요없으면,  nullptr로.

		// window resizing 때문에, vkQueuePresentKHR 가 잘 수행되지 않았다면, 
		// swapChain recreate하고, 다음 프레임으로 넘어가기.
		result = vkQueuePresentKHR(this->Queues[0], &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR
			|| result == VK_SUBOPTIMAL_KHR
			|| framebufferResized)
		{
			framebufferResized = false;
			recreateSwapChain();
			// return 하지 않음으로써, 다음 프레임으로 넘어가기.
		}
		else if (result == VK_SUCCESS) {
			std::cout << "image is presented to swapChain" << std::endl;
			std::cout << "swapChain width = " << this->swapChainExtent.width << std::endl;
			std::cout << "swapChain height = " << this->swapChainExtent.height << std::endl;
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present the image to swapChain.");
		}



		// 다음 inFlight Frame 수(index) 계산.
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
		std::vector<float> queueFamilyPriorities;		// vector에 const 자료형은 원소의 자료형으로 할당 못해주네
		queueFamilyPriorities.resize(uniqueQueueFamilyIndices.size(), 1.0f);
		
		std::vector<VkDeviceQueueCreateInfo> dQueueCreateInfos;
		uint32_t i = 0;
		for (const auto& uniqueQueueFamilyIndex : uniqueQueueFamilyIndices) {
			VkDeviceQueueCreateInfo dQueueCreateInfo{};
			dQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			dQueueCreateInfo.queueFamilyIndex = uniqueQueueFamilyIndex;
			dQueueCreateInfo.queueCount = 1;
			// queueFamilyPriorities.push_back(1.0f);	// 이렇게 하면 for문 밖에서 할당했던 원소의 메모리가 사라지는듯. 자꾸 Priorities값 범위 넘었다고 오류 뜨네.
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
		// 결론: instance만들때 기입한 정보이니, device 만들때도 중복적으로 기입할 필요없는 듯?
		// 하지만, 겹치지않는 extension. VL는 추가해줘야겠지. 예) "VK_KHR_swapchain"
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(userRequDeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = userRequDeviceExtensions.data();
		//deviceCreateInfo.enabledLayerCount;			
		//deviceCreateInfo.ppEnabledLayerNames;
		//
		// Each queue must have a unique queueFamilyIndex within pQueueCreateInfos. (각 queue는, 그 queueFamilyIndex값이 서로 달라야 한다. 중복된 queueFamilyIndex값을 가지면 안된다. ) 
		// 즉, The 'queueFamilyIndex' member of each element of pQueueCreateInfos 'must be unique' within pQueueCreateInfos
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(dQueueCreateInfos.size());
		std::cout << std::endl << std::endl << "number of created unique queue family: " << dQueueCreateInfos.size() << std::endl;	// for test.
		deviceCreateInfo.pQueueCreateInfos = dQueueCreateInfos.data();
		//
		VkPhysicalDeviceProperties PhyDeProperties{};
		vkGetPhysicalDeviceProperties(this->physicalDevice, &PhyDeProperties);
		std::cout << "PhyDeProperties maxBoundDescriptorSets: " << PhyDeProperties.limits.maxBoundDescriptorSets << std::endl;
		// 내 그래픽카드에선 32개라고 나옴.
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
		// 내 그래픽카드에서는 queueFamily가 하나만 선택될거기 때문에, queue도 하나만 생성될 것임.
		// 이 하나의 queue로 graphics, compute, transit 다 함.
		Queues.resize(uniqueQueueFamilyIndices.size());
		uint32_t j = 0;
		for (const auto& uniqueQueueFamilyIndex : uniqueQueueFamilyIndices) {
			vkGetDeviceQueue(this->device, uniqueQueueFamilyIndex, 0, &Queues[j]);
			std::cout << j+1 << "th queue handle is acquired" << std::endl;
			j++;
		}
	}

	void createCommandPool() {
		// Queue의 경우와는 다르게, commandBuffer는, 서로다른 두 commandBuffer 간에도, 서로같은(중복되는) queueFamilyIndex를 가질수있다.

		VkCommandPoolCreateInfo gctCommandPoolInfo{};
		gctCommandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		gctCommandPoolInfo.queueFamilyIndex = QFIndices.graphicsQueueFamilyIndex.value();
		// 매프레임미다 command buffer 다시 record 해줄 거라면, 아래 플래그로 설정해줘야함. VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
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

			//swchCreateInfo.imageUsage = surfaceCapabilInfo.supportedUsageFlags; 하고 
			// VK_FORMAT_B8G8R8A8_SRGB 사용하면 오류남.
			// 그냥 VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT 쓸것.

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
			// recreate swapChain 시에, resizing이 잘 크기전환이 되게 하려면 이부분과, 기타 surface정보 얻어오는 부분을 잘 코딩히야 하는 듯?ㄴ
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
			// swapChainImageCount = 3 이 나옴.
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
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		// VK_IMAGE_LAYOUT_UNDEFINED  어짜피 clear될거니까 don't care임.
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
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;		// glsl에서 나오는 layout과는, 다른 개념의 image layout이다!



		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = this->depthFormat;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		// 혹은 VK_IMAGE_LAYOUT_UNDEFINED	// 처음에 attachment 만들때에는, 특별한 이유없으면, 무조건 VK_IMAGE_LAYOUT_UNDEFINED로 시작해야 하는 듯.  
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
		//subpass.depthStencilAttachmentCount는 무조건 1개씩 밖에 못들어가서, 애초에 Count가 있을 필요가 없는듯.
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
		//subpass.inputAttachmentCount;
		//subpass.pInputAttachments;
		//subpass.preserveAttachmentCount;
		//subpass.pPreserveAttachments;
		//subpass.pResolveAttachments;
		// flags is a bitmask of VkSubpassDescriptionFlagBits specifying usage of the subpass.
		//subpass.flags = 0;



		//VkDependencyInfo dependencyInfo{};		// 이거는 VkSubpassDependencyInfo 와는 다른것임! 이거는, specifying dependency information for a synchronization command.
		// SubpassDependency는 semaphor개념과 관련없는 것임을 유의할 것!
		VkSubpassDependency dependencyInfo{};
		dependencyInfo.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencyInfo.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
									  | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		// 동일한 메모리를 가진 attachment의 이미지에 대해서, src는 read를 헀든, write를 했든, 메모리에 대해 어떤일을 수행헀든 상관없다. 따라서 0이다. color attachment가 clear되도록 앞에서 설정했으므로, 따라서 src는 이 attachment를 read할 필요도 없다!
		// 지금 내가 만들려고하는 예제상황에서는, src(전) subPass가 dst(후,현재) subPass에 데이터를 전달하는 상황이 아니므로, 후SubPass가 전SubPass로부터 받을 input(srcAccess)이 없다는 의미로, 이 인자를 0으로 넣어도 되는듯.
		// src subPass가, 해당 메모리(attachment image data)에 접근해 어떤 operation을 수행했든, 
		// dst SubPass과정으로 넘어오면, 해당 memory는, attachment에 새로 load되면서, clear될 것이기 때문에, (load op설정에 의해 clear하라고 설정되었으므로)
		// 서로간에 종속성이 없다고 보는 듯.
		dependencyInfo.srcAccessMask = 0;	
		dependencyInfo.dstSubpass = 0;
		dependencyInfo.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
									  | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		// 동일한 메모리를 가진 attachment의 이미지에 대해서, dst는 draw한 이미지를, src가 건드렸던 메모리와 동일한 메모리인 color Attachment에, write하는 것을 일로서 가지고있는 Subpass이다. 따라서 그대로 적어주면 된다. 
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
		// 각 imageView(attachment될) 마다 framebuffer를 하나씩 만드네? 왜 이렇게 비효율적으로 만들지?
		// 하나의 framebuffer 가 여러 이미지 한꺼번에 담을수있게 만들면 안돼나?
		// 각 이미지가 서로 다른 프레임에 나온 이미지라서 그런가?

		(this->framebuffers).resize((this->swapChainImageViews).size());

		for (size_t i = 0; i < (this->swapChainImageViews).size(); i++)
		{
			std::array<VkImageView, 2> imageToAttachments = {(this->swapChainImageViews)[i], this->depthImageView};

			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = this->renderPass;
			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageToAttachments.size());
			framebufferCreateInfo.pAttachments = imageToAttachments.data();	// 이름은 pAttachments 로 되어있지만, a pointer to an array of 'VkImageView handles', 이다. 곧 attachment가 될 imageViews라는 의미로 이렇게 이름 정한듯.
			framebufferCreateInfo.width = swapChainExtent.width;
			framebufferCreateInfo.height = swapChainExtent.height;
			framebufferCreateInfo.layers = 1;								// 그냥 이미지 메모리공간의 3차원 인덱스라고 생각하면 된다.
		
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
		// Physical device에서, depth image format으로, 사용자가 지정한 format을 지원하는지, query해보아야 한다.
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
		// 아래는 서로다른 queueFamily가 image를 공유해서 쓸때 채워넣는 파라미터다.
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
		// 참고
		// Memory should be bound by calling vkBindImageMemory().
		// If image is non - sparse then it must be bound completely and contiguously to a single VkDeviceMemory object.



		// 참고
		// imageView를 생성하기 전에, 반드시 image가 할당된 memory에 bind 되어있어야 한다! 안그러면 validation layer error 메시지 뜸.
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
		VkDeviceSize image3Dsize = texWidth * texHeight * texDepth * texChannels * sizeof(float);	// 맨 마지막은 채널당 byte수.
		
		std::vector<float> pixels(image3Dsize, 0.0f);		// build를 clean안하면, 이전실행에서 초기호한 내용이 그대로 다음 ㅅ힐행에서 이어지네;;  std::vector 생성시, 할당된 메모리 주소가 같으면, 다음샐행에서 초기화를 생략하기 때문에 이전실행했던 데이터가 이어지는 듯!
		
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
		
		std::vector<Voxel> pixels(image3Dcount, {{0.1f, 0.0f, 0.0f}, 0.0f});		// build를 clean안하면, 이전실행에서 초기호한 내용이 그대로 다음 ㅅ힐행에서 이어지네;;  std::vector 생성시, 할당된 메모리 주소가 같으면, 다음샐행에서 초기화를 생략하기 때문에 이전실행했던 데이터가 이어지는 듯!

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
		// sharingMode = VK_SHARING_MODE_EXCLUSIVE 인 경우에는, 
		// 아래 vertexBufferInfo.queueFamilyIndexCount 와, vertexBufferInfo.pQueueFamilyIndices 를 안써도 된다.
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
		// sharingMode = VK_SHARING_MODE_EXCLUSIVE 인 경우에는, 
		// 아래 vertexBufferInfo.queueFamilyIndexCount 와, vertexBufferInfo.pQueueFamilyIndices 를 안써도 된다.
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
		// sharingMode = VK_SHARING_MODE_EXCLUSIVE 인 경우에는, 
		// 아래 vertexBufferInfo.queueFamilyIndexCount 와, vertexBufferInfo.pQueueFamilyIndices 를 안써도 된다.
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
		// sharingMode = VK_SHARING_MODE_EXCLUSIVE 인 경우에는, 
		// 아래 vertexBufferInfo.queueFamilyIndexCount 와, vertexBufferInfo.pQueueFamilyIndices 를 안써도 된다.
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
		// 아래 파라미터는, which pipeline shader stages can access a resource for this binding, 를 의미함.
		lastDensityDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding currentDensityDescSetLayoutBinding{};
		currentDensityDescSetLayoutBinding.binding = 2;
		currentDensityDescSetLayoutBinding.descriptorCount = 1;
		currentDensityDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		currentDensityDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// 아래 파라미터는, which pipeline shader stages can access a resource for this binding, 를 의미함.
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
		// 아래는 특정 sampler만 imageDescriptor에 종속시키고 싶을때, 쓰는 파라미터인듯. nullptr이면, 해당 descriptorSet안에 있는 모든 image descriptor에, sampler를 쓸수있는듯.
		densityCopySamplerDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// 아래 파라미터는, which pipeline shader stages can access a resource for this binding, 를 의미함.
		densityCopySamplerDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;



		VkDescriptorSetLayoutBinding lastVelocityDensityDescSetLayoutBinding{};
		lastVelocityDensityDescSetLayoutBinding.binding = 5;
		lastVelocityDensityDescSetLayoutBinding.descriptorCount = 1;
		lastVelocityDensityDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		lastVelocityDensityDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// 아래 파라미터는, which pipeline shader stages can access a resource for this binding, 를 의미함.
		lastVelocityDensityDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding currentVelocityDensityDescSetLayoutBinding{};
		currentVelocityDensityDescSetLayoutBinding.binding = 6;
		currentVelocityDensityDescSetLayoutBinding.descriptorCount = 1;
		currentVelocityDensityDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		currentVelocityDensityDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// 아래 파라미터는, which pipeline shader stages can access a resource for this binding, 를 의미함.
		currentVelocityDensityDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding velocityDensityTempDescSetLayoutBinding{};
		velocityDensityTempDescSetLayoutBinding.binding = 7;
		velocityDensityTempDescSetLayoutBinding.descriptorCount = 1;
		velocityDensityTempDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		velocityDensityTempDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// 아래 파라미터는, which pipeline shader stages can access a resource for this binding, 를 의미함.
		velocityDensityTempDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding lastDivergencePressureDescSetLayoutBinding{};
		lastDivergencePressureDescSetLayoutBinding.binding = 8;
		lastDivergencePressureDescSetLayoutBinding.descriptorCount = 1;
		lastDivergencePressureDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		lastDivergencePressureDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// 아래 파라미터는, which pipeline shader stages can access a resource for this binding, 를 의미함.
		lastDivergencePressureDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding currentDivergencePressureDescSetLayoutBinding{};
		currentDivergencePressureDescSetLayoutBinding.binding = 9;
		currentDivergencePressureDescSetLayoutBinding.descriptorCount = 1;
		currentDivergencePressureDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		currentDivergencePressureDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// 아래 파라미터는, which pipeline shader stages can access a resource for this binding, 를 의미함.
		currentDivergencePressureDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding divergencePressureTempDescSetLayoutBinding{};
		divergencePressureTempDescSetLayoutBinding.binding = 10;
		divergencePressureTempDescSetLayoutBinding.descriptorCount = 1;
		divergencePressureTempDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		divergencePressureTempDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// 아래 파라미터는, which pipeline shader stages can access a resource for this binding, 를 의미함.
		divergencePressureTempDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding bcDescSetLayoutBinding{};
		bcDescSetLayoutBinding.binding = 11;
		bcDescSetLayoutBinding.descriptorCount = 1;
		bcDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		bcDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// 아래 파라미터는, which pipeline shader stages can access a resource for this binding, 를 의미함.
		bcDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding vorticityDescSetLayoutBinding{};
		vorticityDescSetLayoutBinding.binding = 12;
		vorticityDescSetLayoutBinding.descriptorCount = 1;
		vorticityDescSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		vorticityDescSetLayoutBinding.pImmutableSamplers = nullptr;
		// 아래 파라미터는, which pipeline shader stages can access a resource for this binding, 를 의미함.
		vorticityDescSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;



		// 기본적으로 std::array = 는 복사해서 값을 넣네.
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

		VkDescriptorPoolSize poolSize0{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
		poolSize0.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize0.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);		// 여러 DescriptorSet에 걸쳐서, 그 각각의 Set이 가지는 Descriptor 갯수를 다 합해서, 정해지는 갯수다.

		VkDescriptorPoolSize poolSize1{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
		poolSize1.type = VK_DESCRIPTOR_TYPE_SAMPLER;
		poolSize1.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize2{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
		poolSize2.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize2.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize3{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
		poolSize3.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		poolSize3.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize4{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
		poolSize4.type = VK_DESCRIPTOR_TYPE_SAMPLER;
		poolSize4.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);



		VkDescriptorPoolSize poolSize5{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
		poolSize5.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize5.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize6{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
		poolSize6.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize6.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize7{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
		poolSize7.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize7.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize8{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
		poolSize8.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize8.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize9{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
		poolSize9.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize9.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize10{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
		poolSize10.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize10.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize11{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
		poolSize11.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSize11.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		VkDescriptorPoolSize poolSize12{};	// 각 binding영역마다, poolSize를 한번씩 지정한다.
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
		descPoolCreateInfo.pPoolSizes = poolSizeInfos.data();							// 쉽게 생각하면, pool안의 descriptor의 갯수.
		descPoolCreateInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);		// 쉽게 생각하면, pool안의 descriptorSet의 갯수.

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
			// sizeof(this->uniformBuffers[i]) 로 하면 안됌! 
			// 만들어진 버퍼객체(일종의변수)의 크기와, 버퍼객체가 관리하는(가리키는?) 메모리의 크기.는 서로 다른것이다!
			// 그리고 메모리의 크기와, 입력될 데이터의 크기 또한 다르다.
			// 여기서는 입력될 데이터의 크기로서, sizeof(UniforBufferObject)를 써야 정확하다.

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






			VkWriteDescriptorSet descUniforBuffWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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

			VkWriteDescriptorSet descLastDensityWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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

			VkWriteDescriptorSet descCurrentDensityWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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

			VkWriteDescriptorSet descDensityCopiesWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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

			VkWriteDescriptorSet descDensityCopySamplersWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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



			VkWriteDescriptorSet descLastVelocityDensityWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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

			VkWriteDescriptorSet descCurrentVelocityDensityWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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

			VkWriteDescriptorSet descVelocityDensityTempWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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

			VkWriteDescriptorSet descLastDivergencePressureWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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

			VkWriteDescriptorSet descCurrentDivergencePressureWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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

			VkWriteDescriptorSet descDivergencePressureTempWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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

			VkWriteDescriptorSet descBCWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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

			VkWriteDescriptorSet descVorticityWriteInfo{};				// descriptorSet에 descriptor를 write한다는 의미.
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

		//VkShaderModule compShaderModule3_0 = createShaderModule("shader_Resourcing.comp", "comp");
		//this->computeShaderModules.push_back(compShaderModule3_0);
		//VkShaderModule compShaderModule3_1 = createShaderModule("shader_Projection.comp", "comp");
		//this->computeShaderModules.push_back(compShaderModule3_1);
		// VkShaderModule compShaderModule3_0 = createShaderModule("shader_Sourcing.comp", "comp");
		// this->computeShaderModules.push_back(compShaderModule3_0);
		// VkShaderModule compShaderModule3_1 = createShaderModule("shader_Divergence.comp", "comp");
		// this->computeShaderModules.push_back(compShaderModule3_1);
		// VkShaderModule compShaderModule3_2 = createShaderModule("shader_Jacobi_1.comp", "comp");
		// this->computeShaderModules.push_back(compShaderModule3_2);
		// VkShaderModule compShaderModule3_3 = createShaderModule("shader_Jacobi_2.comp", "comp");
		// this->computeShaderModules.push_back(compShaderModule3_3);
		// VkShaderModule compShaderModule3_4 = createShaderModule("shader_ApplyPressure.comp", "comp");
		// this->computeShaderModules.push_back(compShaderModule3_4);
		// VkShaderModule compShaderModule3_5 = createShaderModule("shader_ComputeVorticity.comp", "comp");
		// this->computeShaderModules.push_back(compShaderModule3_5);
		// VkShaderModule compShaderModule3_6 = createShaderModule("shader_ApplyVorticity.comp", "comp");
		// this->computeShaderModules.push_back(compShaderModule3_6);
		// VkShaderModule compShaderModule3_7 = createShaderModule("shader_Advection.comp", "comp");
		// this->computeShaderModules.push_back(compShaderModule3_7);

		//VkShaderModule compShaderModule3_0 = createShaderModule("shader_Sourcing.comp", "comp");
		auto compShaderCode3_0 = readFile("shader_Sourcing_comp.spv");
		VkShaderModule compShaderModule3_0 = createShaderModule(compShaderCode3_0);
		this->computeShaderModules.push_back(compShaderModule3_0);
		auto compShaderCode3_1 = readFile("shader_Divergence_comp.spv"); 
		VkShaderModule compShaderModule3_1 = createShaderModule(compShaderCode3_1);
		this->computeShaderModules.push_back(compShaderModule3_1);
		auto compShaderCode3_2 = readFile("shader_Jacobi_1_comp.spv");
		VkShaderModule compShaderModule3_2 = createShaderModule(compShaderCode3_2);
		this->computeShaderModules.push_back(compShaderModule3_2);
		auto compShaderCode3_3 = readFile("shader_Jacobi_2_comp.spv");
		VkShaderModule compShaderModule3_3 = createShaderModule(compShaderCode3_3);
		this->computeShaderModules.push_back(compShaderModule3_3);
		auto compShaderCode3_4 = readFile("shader_ApplyPressure_comp.spv");
		VkShaderModule compShaderModule3_4 = createShaderModule(compShaderCode3_4);
		this->computeShaderModules.push_back(compShaderModule3_4);
		auto compShaderCode3_5 = readFile("shader_ComputeVorticity_comp.spv");
		VkShaderModule compShaderModule3_5 = createShaderModule(compShaderCode3_5);
		this->computeShaderModules.push_back(compShaderModule3_5);
		auto compShaderCode3_6 = readFile("shader_ApplyVorticity_comp.spv");
		VkShaderModule compShaderModule3_6 = createShaderModule(compShaderCode3_6);
		this->computeShaderModules.push_back(compShaderModule3_6);
		auto compShaderCode3_7 = readFile("shader_Advection_comp.spv");
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

		//VkViewport viewport{};	// to member variable.	// 노말라이즈 안한 좌표계 크기 그대로씀. 다만 int(800픽셀 등) 대신 float(800.0 등)로 표현하는 것 뿐임.
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
		// src 가 지금 pipeline에서 새로나오고있는 image이고, dst 가 이전에 나온 attachment 이미지인 듯.
		VkPipelineColorBlendAttachmentState colorBlendAttachmentInfo{};
		colorBlendAttachmentInfo.blendEnable = VK_TRUE;
		colorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;		// or VK_BLEND_FACTOR_SRC_ALPHA;	// 여기서 나온 BELND_FACTOR 가, 바로 아래에 나오는, VkPipelineColorBlendStateCreateInfo::blendConstants (float형) 를 의미한다.
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



		//std::vector<VkShaderModule> shaderModules;	// 맴버변수로 감.
		// VkShaderModule vertShaderModule = createShaderModule("shader.vert", "vert");
		// this->graphicsShaderModules.push_back(vertShaderModule);
		// VkShaderModule fragShaderModule = createShaderModule("shader.frag", "frag");
		// this->graphicsShaderModules.push_back(fragShaderModule);
		auto vertShaderCode = readFile("shader_vert.spv"); 
		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		this->graphicsShaderModules.push_back(vertShaderModule);
		auto fragShaderCode = readFile("shader_frag.spv"); 
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
		this->graphicsShaderModules.push_back(fragShaderModule);

		std::vector<VkPipelineShaderStageCreateInfo> graphicsShaderStages;

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;	// VK_STRUCTURE_TYPE_''SHADER_''CREATE_INFO_'EXT'; 와 햇갈리지 말것!
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = this->graphicsShaderModules[0];
		vertShaderStageInfo.pName = "main";
		// vertShaderStageInfo.pSpecializationInfo;	// Entry Point 를 맵핑하는 추가기능과 관련된 항목.
		// vertShaderStageInfo.flags;					// subgroup 관련된 추가기능 flag들.
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
		gPipelineInfo.pViewportState = &viewportStateInfo;		// dynamic state를 설정했더라도, 이부분은 반드시 써넣어야 한다.
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
		gPipelineInfo.basePipelineIndex = -1;	// basePipelineIndex is an index into the pCreateInfos parameter(vkCreateGraphicsPipelines함수에서의 인자) to use as a pipeline to derive from.

		if (vkCreateGraphicsPipelines(this->device, nullptr,
			1, &gPipelineInfo,
			nullptr, &(this->graphicsPipeline))
			!= VK_SUCCESS) {
			throw std::runtime_error("failed to create Graphics Pipeline.");
		}	std::cout << "Graphics Pipeline is created." << std::endl;

	}
	void createGraphicsPipeline2() {

		//VkViewport viewport{};	// to member variable.	// 노말라이즈 안한 좌표계 크기 그대로씀. 다만 int(800픽셀 등) 대신 float(800.0 등)로 표현하는 것 뿐임.
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
		// src 가 지금 pipeline에서 새로나오고있는 image이고, dst 가 이전에 나온 attachment 이미지인 듯.
		VkPipelineColorBlendAttachmentState colorBlendAttachmentInfo{};
		colorBlendAttachmentInfo.blendEnable = VK_TRUE;
		colorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;		// or VK_BLEND_FACTOR_SRC_ALPHA;	// 여기서 나온 BELND_FACTOR 가, 바로 아래에 나오는, VkPipelineColorBlendStateCreateInfo::blendConstants (float형) 를 의미한다.
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



		//std::vector<VkShaderModule> shaderModules;	// 맴버변수로 감.
		auto vertShader2Code = readFile("shader2_vert.spv"); 
		VkShaderModule vertShaderModule2 = createShaderModule(vertShader2Code);
		this->graphicsShaderModules2.push_back(vertShaderModule2);
		auto fragShader2Code = readFile("shader2_frag.spv"); 
		VkShaderModule fragShaderModule2 = createShaderModule(fragShader2Code);
		this->graphicsShaderModules2.push_back(fragShaderModule2);

		std::vector<VkPipelineShaderStageCreateInfo> graphics2ShaderStages;

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;	// VK_STRUCTURE_TYPE_''SHADER_''CREATE_INFO_'EXT'; 와 햇갈리지 말것!
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = this->graphicsShaderModules2[0];
		vertShaderStageInfo.pName = "main";
		// vertShaderStageInfo.pSpecializationInfo;	// Entry Point 를 맵핑하는 추가기능과 관련된 항목.
		// vertShaderStageInfo.flags;					// subgroup 관련된 추가기능 flag들.
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
		g2PipelineInfo.pViewportState = &viewportStateInfo;		// dynamic state를 설정했더라도, 이부분은 반드시 써넣어야 한다.
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
		g2PipelineInfo.basePipelineIndex = -1;	// basePipelineIndex is an index into the pCreateInfos parameter(vkCreateGraphicsPipelines함수에서의 인자) to use as a pipeline to derive from.

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
		// 몇개 semaphore 만들거냐는 정보를 안넣어주므로, vkCreateSemaphore()함수 한번 실행할때마다, 한개의 semaphore객체만 만들수있는듯?
		// 따라서 배열의 포인터를 인자로 넣어주면 안되고, vkCreateSemaphore()함수를 여러번 실행하여, 각 객체를 담을 변수 하나씩을 넣어줘야 할듯.

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
		// 몇개 semaphore 만들거냐는 정보를 안넣어주므로, vkCreateSemaphore()함수 한번 실행할때마다, 한개의 semaphore객체만 만들수있는듯?
		// 따라서 배열의 포인터를 인자로 넣어주면 안되고, vkCreateSemaphore()함수를 여러번 실행하여, 각 객체를 담을 변수 하나씩을 넣어줘야 
		
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
					matchedExtensions.push_back(uRwRExtension.c_str());		// supportedExtension.extensionName으로 값을 받으면 안된다! supportedExtension은 이 함수내에서만 유효하고 사라질 것이기 때문이다.
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
					matchedValidationLayers.push_back(validationLayer);			// availableValidationLayer.layerName을 값으로 넣어주면 안된다! 이함수밖에서는 없어질 객체의 데이터이기 때문이다!
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
	{	// 아쉽게도 이 vkCreateDebugUtilsMessengerEXT 라는 함수는, '확장함수(~EXT로 끝나는 함수)'이기 때문에, 
		// 자동으로 로딩되지는 않습니다. (내 Instance의 callback함수로) 
		// 우리는 vkGetInstanceProcAddr()를 사용하여, Instnace와 바인딩시키고, 
		// 그 바인딩한 함수의 주소를, 직접 찾아야 합니다.
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
	{	// 아쉽게도 이 vkCreateDebugUtilsMessengerEXT 라는 함수는, '확장함수(~EXT로 끝나는 함수)'이기 때문에, 
		// 자동으로 로딩되지는 않습니다. (내 Instance의 callback함수로) 
		// 우리는 vkGetInstanceProcAddr()를 사용하여, Instnace와 바인딩시키고, 
		// 그 바인딩한 함수의 주소를, 직접 찾아야 합니다.
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(this->instance,
																			   "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, DebugMessenger, pAllocator);		// vkDestroy함수들은 리턴값이 없다.
		}
		//else {
		//	return VK_ERROR_EXTENSION_NOT_PRESENT;
		//}
	}
	// VkResult lookInside_glfwCreateWindowSurface(VkInstance& instance, GLFWwindow* window,
	// 											const VkAllocationCallbacks* pAllocCallbacks,
	// 											VkSurfaceKHR* pSurface) 
	// {
	// 	// glfwCreateWindowSurface() 함수와 내부구조와 원리가 같은 함수를 만들어 보았다.

	// 	// VkWin32SurfaceCreateInfoKHR 를 사용하려면, #define VK_USE_PLATFORM_WIN32_KHR 를 정의해줘야 한다.
	// 	//VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	// 	VkXcbSurfaceCreateInfoKHR surfaceCreateInfo{};
	// 	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	// 	// glfwGetWin32Window 사용하려면, 
	// 	// #define GLFW_EXPOSE_NATIVE_WIN32
	// 	// #include <GLFW/glfw3native.h> 이 둘을 include 해줘야함.
	// 	//surfaceCreateInfo.hwnd = glfwGetWin32Window(window);		//(HWND)window; 이렇게 써도 되나?
	// 	surfaceCreateInfo.connection = glfwGetWindowUserPointer(window);
	// 	// GetModuleHandle()함수는 윈도우API의 함수인듯.	
	// 	surfaceCreateInfo.window = GetModuleHandle(nullptr);		//(HINSTANCE)(this->instance); 이렇게 써도 되나 ?

	// 	// 마찬가지로, vkCreateWin32SurfaceKHR()함수를 쓰려면, #define VK_USE_PLATFORM_WIN32_KHR 를 정의해줘야 한다.
	// 	//return vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, pAllocCallbacks, pSurface);
	// 	return vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, pAllocCallbacks, pSurface);
	// }

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsQueueFamilyIndex;
		// It's actually possible that the queue families supporting drawing commands and the ones supporting presentation do not overlap
		// But my GPU's graphicsQueueFamily and presentationQueueFamily does overlap.
		// However, in any case(혹시모르니), graphicsQueueFamily와 presentationQueueFamily를 구분해서 탐지해보자.
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
		std::cout<< QFCount << std::endl;	// 출력하면 6나옴.	(내 그래픽카드에는 총 6개의 QueueFamily가 있음.
		
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


		// 정보출력용 for loop.
		uint32_t j = 0;
		for (const auto& QFProperty : QFProperties)
		{
			std::bitset<32> x(QFProperty.queueFlags);
			std::cout << "QFIndex: " << j << "\t" << "QFProperty.queueFlags: " << x << std::endl;

			j++;
		}
		//
		// 참고: 내 그래픽카드 QueueFamily 목록.		
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
		// (내 그래픽카드에는 VK_QUEUE_PROTECTED_BIT 를 갖춘 QueueFamily는 없음. 나머지 QFproperty에 대해서는, 갖춘 QueueFamily를 가지고 있음.)
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
		QFIndices = findQueueFamilies(phyDevice);		// this->physicalDevice 사용하면 안됨!	this->physicalDevice가 pick 되기 전에 이 함수가 사용될 수 있으므로!

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
	// 	//char sourceTextBuffer[3000] = {""};			// 이 배열수 충분히 크게 안해주면, "syntax error, unexpected end of file" 라고 하면서, 쉐이터 컴파일링 오류뜸.

	// 	inputFile.seekg(0);		// 파일의 맨처음 위치로 이동.
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
	// 	// compiler.PreprocessGlsl() 안해주고, 바로 compiler.CompileGlslToSpv() 해줘도 됨.
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

	// 	// shaderc::CompilationResult 와 shaderc::SpvCompilationResult 의 차이는, 
	// 	// using SpvCompilationResult = CompilationResult<uint32_t>; 가 다 말해준다.
	// 	if ((compileResult.cbegin() == nullptr)  ||  (compileResult.cbegin() == nullptr)) {
	// 		throw std::runtime_error("failed to create SPIR-V compiled files.");
	// 	}
		
	// 	// 이 방식으로 compile된 바이너리 결과데이터를 전달하는 방법을 알아내는 것은, 이 라이브러리를 만든이의 예제코드를 보지 않으면, 알기 쉽지 않을듯.
	// 	std::vector<uint32_t> shaderModuleCode{compileResult.cbegin(), compileResult.cend()};

	// 	VkShaderModuleCreateInfo shaderModuleCreateInfo{};		// 이 struct 생성할때 초기화 반드시 해줘야 함.
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

	// 통합하거나 폐기할만한 함수들. 그러나 본문함수에서 쓰고있는 함수들 ////////////////////////////
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
				//	등등
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
		imgCreateInfo.arrayLayers = 1;	// Cube maps, Animated textures, and texture arrays of shadow maps and other similar collections. 등에 쓰이는 파리미터다.
		//storageImgCreateInfo.flags = VK_IMAGE_CREATE_SPARSE_BINDING_BIT;	// image에는 sparse기능 못쓰나봄.

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
		// heapType: GPU 전용 heap메모리인지(= DEVICE_LOCAL), 시스템(CPU) 전용 heap메모리 인지(= not DEVICE_LOCAL).
		// memType: DEVICE_LOCAL, HOST_VISIBLE, HOST_CACHED, HOST_COHERENT 등등의, 할당될 메모리의 특성에 따라, 분류되는 메모리유형. 
		// 가장 큰 오해는 메모리 유형이 물리적으로 분리되어 있다는 것이다. 그렇지 않다. 메모리의 '논리적 분리'와 '물리적 분리'를 구분해야 한다.
		// '메모리 type'은, 사용자가 정한 메모리 할당방법과 사용용도에 따라, '논리적으로 분리'하는 것에 해당하고,
		// 'heap type'은, 실제 메모리의 '물리적공간을 분리'하는 것에 해당한다.
		// 메모리 type은, 드라이버와 공급업체에 따라 다른, 그들만이 알고있는, 구현 세부 사항이며, 구현자/공급업체만이 확실히 알고 있는 내용이다.
		// 따라서 우리는 vulkan의 query함수로, query하여, 메모리typeIndex를 알아내는 수 밖에 없다.
		VkPhysicalDeviceMemoryProperties PhyDeMemProperty{};
		vkGetPhysicalDeviceMemoryProperties(this->physicalDevice, &PhyDeMemProperty);
		uint32_t heapIndex = PhyDeMemProperty.memoryTypes[memAllocInfo.memoryTypeIndex].heapIndex;
		std::cout << "PhyDeMemProperty.memoryHeaps[heapIndex].size: " << PhyDeMemProperty.memoryHeaps[heapIndex].size << std::endl;
		// 전체 heap메모리를 쓸수있는 전체양은 12703498240바이트인데,
		// 한 image당 쓸수있는 heap메모리 양은 224395264인듯. 
		// 같은 용도, 메모리타입의 storage image는, 내 GPU메모리에서는, 총 56.612장 정도까지만 할당가능하네.

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
		// 아래는 depth buffer sampler에 적용되는 내용.
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



		VkCommandBufferBeginInfo comBuffBeginInfo{};							// 여기서 {} 이거 안했다고 vkBeginCommandBuffer에서 오류났었음. 이때 오류메시지도 안나오고 그냥 프로그램이꺼져서 오류 찾는데 힘들었음. struct에 값 뺴놓지말고 다 넣을것.
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
		// shared memory 기능 쓸때, 적는 파라미터인듯.
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
			// bufferRowLength와 bufferImageHeight 파라미터는, 이미지데이터 크기가 버퍼크기보다 작을 때, 그래서 버퍼 메모리 공간이 남을때에, 작성하는 인자인 듯.
			// 두 파라미터가 0이면, 버퍼공간에 공간낭비없이, 이미지데이터로 꽉 차있다는 의미.
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
		//vkDestroyDescriptorSet은 pool이 destroy될때 함께 자동으로 destory되는듯. descriptor도 마찬가지.
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

		//vkDestroyQueue도 따로 없는 듯!
		vkDestroyDevice(this->device, nullptr);
		//vkDestroyPhysicalDevice 함수는 따로 없음.	// vkEnumeratePhysicalDevices()함수 사용할때, 이미 PhysicalDevice가 instance와 링크되어서, instance가 destroy될때, PhysicalDevice도 같이 자동으로 반환되는듯.

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
	// bool framebufferResized = false;			// public memeber 변수로 옮김.

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