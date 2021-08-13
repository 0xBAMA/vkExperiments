#include "app.h"

void app::init_glfw() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE,  GLFW_FALSE);
	window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);

	glfwSetKeyCallback(window, key_callback); // set up keyboard input callback function
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
			if (!layerFound) return false;
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
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
	appInfo.pEngineName = "No Engine Specified";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

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
	uint32_t extension_count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
	std::vector<VkExtensionProperties> extensions(extension_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
	cout << "available extensions(" << extension_count << "):\n";
	for (const auto& extension : extensions) // report
		cout << '\t' << extension.extensionName << '\n';
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

void app::create_surface() {
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface!");
}


QueueFamilyIndices app::findQueueFamilies(VkPhysicalDevice device) {
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
    	VkBool32 presentSupport = false;
    	vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if (presentSupport)
			indices.present_family = i;
		if (indices.found()) break;
    	i++;
	}
  	return indices;
}

// if you have multiple devices, you might want to check for some hardware capabilities -
// this can also be useful for confirming a device is capable of what you want from it,
// interesting idea of using a scoring methodology to choose from multiple devices here:
//   https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Physical_devices_and_queue_families#page_Base-device-suitability-checks
bool app::is_device_suitable(VkPhysicalDevice device) {
	// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPhysicalDeviceProperties.html
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);

	// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPhysicalDeviceFeatures.html
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	// making sure that we have at least one graphics queue and one present queue
	QueueFamilyIndices indices = findQueueFamilies(device);

	// need to make sure we have appropriate swapchain support
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	// check surface/swapchain properties to make sure that this device has the ability to present
	bool swapchainAdequate = false;
	if (extensionsSupported) {
  		SwapchainSupportDetails swapchainSupport = querySwapchainSupport(device);
  		swapchainAdequate = !swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
	}

	// report that a suitable device was found and return result
	if (indices.found() && extensionsSupported && swapchainAdequate) {
		cout << endl << "Located suitable device: [" << deviceProperties.deviceName << "]" << endl; // report deviceName string
		return true;
	}
	return false;
}

bool app::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	// this is directly from vulkan-tutorial
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
   std::vector<VkExtensionProperties> availableExtensions(extensionCount);
   vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	// doing this with an array will be a little different
	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
	for (const auto& extension : availableExtensions)
		requiredExtensions.erase(extension.extensionName);

	return requiredExtensions.empty();
}


void app::pick_physical_device() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) throw std::runtime_error("No GPUs with Vulkan support found!");
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
	for (const auto& device : devices)
	if (is_device_suitable(device)) {
		physical_device = device;
		break;
	}
	if (physical_device == VK_NULL_HANDLE) throw std::runtime_error("Failed to find a suitable GPU!");
}

void app::create_logical_device() {
	QueueFamilyIndices indices = findQueueFamilies(physical_device);

	// this thing using <set> is how it's being handled in vulkan-tutorial, but I think this is
	//   just really superfluous for what is *at most* two ints? will return to this later
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphics_family.value(), indices.present_family.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};	// currently empty
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
   createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.pNext = nullptr;

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physical_device, &createInfo, nullptr, &device) != VK_SUCCESS)
		throw std::runtime_error("Failed to create logical device!");

	// creating the queue objects
	vkGetDeviceQueue(device, indices.graphics_family.value(), 0, &graphics_queue);
	vkGetDeviceQueue(device, indices.present_family.value(), 0, &present_queue);
}

SwapchainSupportDetails app::querySwapchainSupport(VkPhysicalDevice device) {
    SwapchainSupportDetails details;

	// get the capabilities of the current surface
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	// get the list of formats which are supported by the surface
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0) {
    	details.formats.resize(formatCount);
    	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	// very similar operation to get the presentation modes from the surface
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0) {
    	details.presentModes.resize(presentModeCount);
    	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}
	return details;
}

VkSurfaceFormatKHR app::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat; // pick 8bpc RGBA using an SRGB color space
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR app::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			// similar to the fifo mode, but will overwrite queued frames instead of blocking when the queue is full
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR; // else just do fifo mode
}

VkExtent2D app::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != UINT32_MAX) { // UINT32_MAX is a special value used by the driver to express an unitialized surface
		return capabilities.currentExtent; // so this case would be handling an initialized surface
	} else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
		// clamp the values of width and height to the suface's capabilities
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actualExtent;
	}
}

void app::create_swapchain() {
	SwapchainSupportDetails swapchainSupport = querySwapchainSupport(physical_device);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapchainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapchainSupport.capabilities);

	// at least one more than the minimum required to function
	uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

	// maxImageCount has a special value 0, which indicates there is no maxImageCount on this surface
	if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount)
    	imageCount = swapchainSupport.capabilities.maxImageCount;

	// start filling out parameters for swapchain creation
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1; // this would change for e.g. stereoscopic 3d
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physical_device);
	uint32_t queueFamilyIndices[] = {indices.graphics_family.value(), indices.present_family.value()};

	// check to see if the queue indices are distinct from one another
	if (indices.graphics_family != indices.present_family) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	// used for e.g. 90 degree rotations, flips, etc, find supported in capabilities.supportedTransforms
	createInfo.preTransform = swapchainSupport.capabilities.currentTransform;

	// This is something which can be used with the OS compositor to do transparent/partially transparent windows, acccording
	// to the spec, the value must be one of the bits present in VkSurfaceCapabilitiesKHR.supportedCompositeAlpha which you
	// can query with the function vkGetPhysicalDeviceSurfaceCapabilitiesKHR.
	//   I definitely want to look into this at some point in the future. Thinking about using it for floating or maybe
	//   non-rectangular apps like old media player skins...
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;

	// don't care about the color of pixels that are obscured e.g. behind another window, so they will be clipped
	// you will want to disable this flag if your application needs consistent readback of those pixels' results
	createInfo.clipped = VK_TRUE;

	// this is used in the case of swapchain recreation, when a new swapchain is created it must give a reference to the old one
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS)
    	throw std::runtime_error("failed to create swapchain!");

	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
	swapchainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());

	swapchainImageFormat = surfaceFormat.format;
	swapchainExtent = extent;
}

void app::create_image_views() {
	swapchainImageViews.resize(swapchainImages.size());
	for (size_t i = 0; i < swapchainImages.size(); i++) {
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.image = swapchainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapchainImageFormat;

		// remapping the color channels if desired e.g. monochrome or constant value for a given channel
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		// describing image purpose - here, just a color target with no mip levels and a single layer
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		// create the image views
		if (vkCreateImageView(device, &createInfo, nullptr, &swapchainImageViews[i]) != VK_SUCCESS)
		    throw std::runtime_error("failed to create image views!");
	}
}

// main loop for runtime operations (input, etc)
void app::main_loop() {
	while( !glfwWindowShouldClose( window ) ) {
		glfwPollEvents(); // handle all the events off the queue
	}
}

// called with the information on key events
void app::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1); // hit escape to close the app
}

// called on program shutdown
void app::cleanup() {
	for (auto imageView : swapchainImageViews)
		vkDestroyImageView(device, imageView, nullptr); // delete each of the swapchain image views
	vkDestroySwapchainKHR(device, swapchain, nullptr); // delete the current swapchain

	if( enableValidationLayers ) // delete debug callback
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

	vkDestroyDevice(device, nullptr); // destroy the logical device associated with the GPU
	vkDestroySurfaceKHR(instance, surface, nullptr); // destroy the window surface
	vkDestroyInstance(instance, nullptr); // destroy the created instance

	glfwDestroyWindow(window); // close the window and end the program
	glfwTerminate();
}
