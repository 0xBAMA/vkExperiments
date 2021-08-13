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
	std::optional<uint32_t> graphics_family;
  std::optional<uint32_t> present_family;
	bool found(){ return graphics_family.has_value() && present_family.has_value(); }
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
		init_glfw();
		init_Vulkan();
		main_loop();
		cleanup();
	}
private:
	// setting up a window to display + input callbacks
	GLFWwindow* window;
	void init_glfw();

	// setting up the graphics API
	VkInstance instance;
	void init_Vulkan() {
		// startup sequence
		create_instance();
		init_debug_callback();
		create_surface();
		pick_physical_device();
		create_logical_device();
		create_swapchain();
		create_image_views();
		create_graphics_pipeline();
	}

//  ╦ ╦┌─┐┬  ┌─┐┌─┐┬─┐  ╔═╗┬ ┬┌┐┌┌─┐┌┬┐┬┌─┐┌┐┌┌─┐
//  ╠═╣├┤ │  ├─┘├┤ ├┬┘  ╠╣ │ │││││   │ ││ ││││└─┐
//  ╩ ╩└─┘┴─┘┴  └─┘┴└─  ╚  └─┘┘└┘└─┘ ┴ ┴└─┘┘└┘└─┘
	// creates a Vulkan instance
	void create_instance();

	// vulkan driver capabilities - checks instance extensions
	void list_extensions();

	// debug callback
	VkDebugUtilsMessengerEXT debugMessenger;
	void init_debug_callback();

	// window surface, used to present results
	VkSurfaceKHR surface;
	void create_surface();

	// physical device selection
	VkPhysicalDevice physical_device = VK_NULL_HANDLE;
	void pick_physical_device();

	// logical device
	VkDevice device;
	VkQueue graphics_queue;
	VkQueue present_queue;
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	bool is_device_suitable(VkPhysicalDevice device);
	void create_logical_device();

	// swapchain
	std::vector<VkImage> swapchainImages;
	VkSwapchainKHR swapchain;
	void create_swapchain();
	SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	// image views
	std::vector<VkImageView> swapchainImageViews;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;
	void create_image_views();

	// graphics pipeline
	void create_graphics_pipeline();
	VkShaderModule createShaderModule(const std::vector<char>& code);

	// contains program main loop behavior
	void main_loop();

	// just handling escape event to close the window more easily right now
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	// destroying vk objects and shutting down glfw
	void cleanup();
};
