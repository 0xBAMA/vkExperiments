#include "app.h"

void app::init_window() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE,  GLFW_FALSE);
	window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
}

bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	for (const char* layerName : validationLayers) {
	    bool layerFound = false;
	    for (const auto& layerProperties : availableLayers)
	        if (strcmp(layerName, layerProperties.layerName) == 0) {
	            layerFound = true;
	            break;
	        }

	    if (!layerFound)
	        return false;
	}
	return true;
}

std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (enableValidationLayers) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

void app::create_instance() {
	// enable the validation layers if desired
	if (enableValidationLayers && !checkValidationLayerSupport())
        throw std::runtime_error("Validation layers are requested but not available!");

	// create the instance
	VkApplicationInfo appInfo{}; // application info
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
	auto glfw_extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(glfw_extensions.size());
	createInfo.ppEnabledExtensionNames = glfw_extensions.data();

	// special handling of the debug callback, in order to report any issues with instance creation
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = debugCallback;
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

	// create the instance with the specified info, report failure
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		throw std::runtime_error("Vulkan instance creation failed!");
}

void app::list_extensions() {
	if(!enableValidationLayers) return;

	// get the list of supported extensions
	uint32_t extension_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
	std::vector<VkExtensionProperties> extensions(extension_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
	std::cout << "available extensions(" << extension_count << "):\n";
	for (const auto& extension : extensions) // report
		std::cout << '\t' << extension.extensionName << '\n';
}

// functions related to the debug callback
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void app::init_debug_callback() {
	if (!enableValidationLayers) return; // use of callback not desired

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr; // Optional
	createInfo.pNext = nullptr;

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        throw std::runtime_error("Failed to set up debug messenger callback!");
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
        func(instance, debugMessenger, pAllocator);
}


struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family;
	bool found(){ return graphics_family.has_value(); }
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    // Assign index to queue families that could be found
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0; // iterate through queue families and determine if we have at least one graphics queue
	for (const auto& queueFamily : queueFamilies) {
	    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
	        indices.graphics_family = i;
		if (indices.found()) break;
	    i++;
	}
    return indices;
}

// if you have multiple devices, you might want to check for some hardware capabilities
// this can also be useful for confirming a device is capable of what you want from it
// interesting idea of using a scoring methodology to choose from multiple devices here:
//   https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families#page_Base-device-suitability-checks
bool is_device_suitable(VkPhysicalDevice device) {
	// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPhysicalDeviceProperties.html
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPhysicalDeviceFeatures.html
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	cout << endl << "Located " << deviceProperties.deviceName << endl; // report deviceName string

	// for now, we are just making sure that we have at least one graphics queue
	QueueFamilyIndices indices = findQueueFamilies(device);
	return indices.found();
}

void app::pick_physical_device() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) throw std::runtime_error("No GPUs with Vulkan support found!");
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	for (const auto& device : devices)
	    if (is_device_suitable(device)) {
	        physicalDevice = device;
	        break;
	    }

	if (physicalDevice == VK_NULL_HANDLE) throw std::runtime_error("failed to find a suitable GPU!");
}

void app::create_logical_device() {

}

// main loop for runtime operations (input, etc)
void app::main_loop() {
	while( !glfwWindowShouldClose( window ) ){
		glfwPollEvents();
	}
}

// called on program shutdown
void app::cleanup() {
	if( enableValidationLayers ) // delete debug callback
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

	// destroy the created instance
	vkDestroyInstance(instance, nullptr);

	// close the window and end the program
	glfwDestroyWindow(window);
	glfwTerminate();
}
