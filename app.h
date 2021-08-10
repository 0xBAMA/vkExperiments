#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
using std::endl, std::cout, std::cin, std::cerr;
#include <stdexcept>
#include <cstdlib>
#include <cstring>

// these will be done away with eventually, I want to reimplement the parts that use these headers
#include <optional> // for the vulkan-tutorial style handling of the QueueFamilyIndices
#include <set> // also used for the QueueFamilyIndices stuff
// this will probably meet the same fate, static arrays will be suitable instead
#include <vector>

constexpr uint32_t width = 800;
constexpr uint32_t height = 600;

#define DEBUG
#ifdef DEBUG
constexpr bool enableValidationLayers = true;
const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
#else
constexpr bool enableValidationLayers = false;
#endif

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) { // just defined here because the prototype is almost as long as the function
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

class app {
public:
    void run() { // high level program structure
        init_window();
        init_Vulkan();
        main_loop();
        cleanup();
    }
private:
	// setting up a window to display
	GLFWwindow* window;
	void init_window();

	// setting up the graphics API
	VkInstance instance;
    void init_Vulkan(){
		// startup sequence
		create_instance();
		list_extensions();
		init_debug_callback();
		create_surface();
		pick_physical_device();
		create_logical_device();
	}

//  ╦ ╦┌─┐┬  ┌─┐┌─┐┬─┐  ╔═╗┬ ┬┌┐┌┌─┐┌┬┐┬┌─┐┌┐┌┌─┐
//  ╠═╣├┤ │  ├─┘├┤ ├┬┘  ╠╣ │ │││││   │ ││ ││││└─┐
//  ╩ ╩└─┘┴─┘┴  └─┘┴└─  ╚  └─┘┘└┘└─┘ ┴ ┴└─┘┘└┘└─┘
	// creates a Vulkan instance
	void create_instance();

	// vulkan driver capabilities
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

	// contains program main loop behavior
    void main_loop();

	// destroying vk objects and shutting down glfw
    void cleanup();
};
