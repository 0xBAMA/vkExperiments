#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

constexpr uint32_t width = 800;
constexpr uint32_t height = 600;

constexpr bool enableValidationLayers = true;
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

class app {
public:
    void run() {
        init_window();
        init_Vulkan();
        main_loop();
        cleanup();
    }
private:
    GLFWwindow* window;
	void init_window();
	VkInstance instance;
    void init_Vulkan();

    void main_loop();
    void cleanup();
};
