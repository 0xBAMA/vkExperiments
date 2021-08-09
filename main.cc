#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

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
        void init_window(){
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
        }

        void init_Vulkan(){
            uint32_t extensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
            std::cout << extensionCount << " extensions supported\n";
        }

        void main_loop(){
            while(!glfwWindowShouldClose(window)){
                glfwPollEvents();
            }
        }

        void cleanup(){
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
