#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
using std::endl, std::cout, std::cin, std::cerr;
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <cstdint> // for UINT32_MAX
#include <algorithm>

// these will be done away with eventually, I want to reimplement the parts that use these headers
#include <optional> // for the vulkan-tutorial style handling of the QueueFamilyIndices
#include <set> // also used for the QueueFamilyIndices stuff
// this will probably meet the same fate, static arrays are going to be able to do everything I need
#include <vector>

constexpr uint32_t width  = 720;
constexpr uint32_t height = 480;

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

#define DEBUG
#ifdef DEBUG
constexpr bool enableValidationLayers = true;
#else
constexpr bool enableValidationLayers = false;
#endif

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,	void* pUserData) {
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) // Message is important enough to show
		cerr << "validation layer: " << pCallbackData->pMessage << endl;
	return VK_FALSE;
}

// used to determine a suitable devices in the system (at least a graphics+present queue)
struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
	bool found(){ return graphicsFamily.has_value() && presentFamily.has_value(); }
};

// simplifies the passing of swapchain details
struct SwapchainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class app {
public:
  	void run() { // high level program structure
		initGLFW();
		initVulkan();
		mainLoop();
		cleanup();
	}
private:
	// setting up a window to display + input callbacks
	GLFWwindow* window;
	void initGLFW();

	// setting up the graphics API
	VkInstance instance;
	void initVulkan() {
		// startup sequence
		createInstance();
		initDebugCallback();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapchain();
		createImageViews();
		createRenderPass();
		createGraphicsPipeline();
		createFramebuffers();
		createCommandPool();
		createCommandBuffers();
		createSyncObjects();
	}

//  ╦ ╦┌─┐┬  ┌─┐┌─┐┬─┐  ╔═╗┬ ┬┌┐┌┌─┐┌┬┐┬┌─┐┌┐┌┌─┐
//  ╠═╣├┤ │  ├─┘├┤ ├┬┘  ╠╣ │ │││││   │ ││ ││││└─┐
//  ╩ ╩└─┘┴─┘┴  └─┘┴└─  ╚  └─┘┘└┘└─┘ ┴ ┴└─┘┘└┘└─┘
	// creates a Vulkan instance
	void createInstance();

	// vulkan driver capabilities - checks instance extensions
	void listExtensions();

	// debug callback
	VkDebugUtilsMessengerEXT debugMessenger;
	void initDebugCallback();

	// window surface, used to present results
	VkSurfaceKHR surface;
	void createSurface();

	// physical device selection
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	void pickPhysicalDevice();

	// logical device
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	void createLogicalDevice();

	// swapchain
	std::vector<VkImage> swapchainImages;
	VkSwapchainKHR swapchain;
	void createSwapchain();
	SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	// image views
	std::vector<VkImageView> swapchainImageViews;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	void createImageViews();

	// graphics pipeline
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	void createGraphicsPipeline();
	VkShaderModule createShaderModule(const std::vector<char>& code);

	// render pass
	VkRenderPass renderPass;
	void createRenderPass();

	// framebuffers
	std::vector<VkFramebuffer> swapchainFramebuffers;
	void createFramebuffers();

	// command pool/buffers
	std::vector<VkCommandBuffer> commandBuffers;
	VkCommandPool commandPool;
	void createCommandPool(); // pool manages the memory that is used by buffers
	void createCommandBuffers(); // allocated out of the pool

	// synchronization objects
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	void createSyncObjects();
	size_t currentFrame = 0;

	// contains program main loop behavior
	void drawFrame();
	void mainLoop();

	// just handling escape event to close the window more easily right now
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	// resize utilities
	bool framebufferResized = false;
	void recreateSwapchain();

	// destroying vk objects and shutting down glfw
	void cleanupSwapchain(); // broken out for swapchain recreation
	void cleanup();
};
