#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>

int main(int argc, char const *argv[]) {

	// create GLFW window
	glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

	// counting the number of supported Vulkan extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << extensionCount << " extensions supported\n";

	// check to see if GLM is working
    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

	// wait
    while(!glfwWindowShouldClose(window)){
		glfwPollEvents();
	}

	// quit
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
