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
        void init_window() {
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE,  GLFW_FALSE);
            window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
        }

        void init_Vulkan() {
            create_instance();
            report_extensions();
        }

        VkInstance instance;
        void create_instance() {
            // application info
            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pNext = nullptr;
            appInfo.pApplicationName = "Basic Vulkan App";
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "No Engine Specified";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_0;

            // wrapper around application info
            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pNext = nullptr;
            createInfo.pApplicationInfo = &appInfo;

            // query required extensions from GLFW
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions;
            glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
            createInfo.enabledExtensionCount = glfwExtensionCount;
            createInfo.ppEnabledExtensionNames = glfwExtensions;
            createInfo.enabledLayerCount = 0;

            // create the instance with the specified info, report failure 
            if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
                throw std::runtime_error("Vulkan instance creation failed!");
            }
        }

        void report_extensions(){
            // get the total count
            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

            // list them out
            std::vector<VkExtensionProperties> extensions(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
            std::cout << "available extensions:\n";
            for (const auto& extension : extensions) {
                std::cout << '\t' << extension.extensionName << '\n';
            }
        }
        
        void main_loop() {
            while( !glfwWindowShouldClose( window ) ){
                glfwPollEvents();
            }
        }

        void cleanup() {
            glfwDestroyWindow(window);
            glfwTerminate();
        }
};


int main(int argc, char const *argv[]) {
    app vkApp;
    try{vkApp.run();}catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
