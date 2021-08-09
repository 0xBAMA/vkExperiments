#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
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


class app {
public:
	// high level program structure
    void run() {
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
	}

	// init helper functions
	void create_instance();
	void list_extensions();
	VkDebugUtilsMessengerEXT debugMessenger;
	void init_debug_callback();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	    VkDebugUtilsMessageTypeFlagsEXT messageType,
	    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	    void* pUserData) {
		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) // Message is important enough to show
			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	    return VK_FALSE;
	}

	// contains program main loop behavior
    void main_loop();

	// destroying vk objects and shutting down glfw
    void cleanup();
};
