#ifndef ENGINE_HPP
#define ENGINE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <exception>
#include <vector>
#include <string>
#include <array>

#ifndef ENGINE_DEBUG
	#define ENGINE_DEBUG 0
#endif

namespace EngineExceptions
{
	class GlfwInitFailure: public std::exception
	{
		const char* what() const throw();
	};
	class VKCallFailure: public std::exception
	{
        private:
            std::array<char, 128> msgBuffer;
        public:
            VKCallFailure(const char* funcName, VkResult errCode);
		    const char* what() const throw();
	};
    class VkInitFailure: public std::exception
	{
		const char* what() const throw();
	};
}

typedef struct SLayersCheckResult
{
    bool status;
    std::vector<const char*> unsupportedLayers;
} LayersCheckResult;

class Engine
{
	private:
		bool glfwInitalized;
		bool vkInitalized;
		GLFWwindow* window;
		VkInstance vkInstance;
		std::vector<const char*> layers;
		void initGlfw();
		void initVulkan();
		void cleanupGlfw();
		void cleanupVulkan();
		void cleanup();
		LayersCheckResult checkLayersSupport();
	public:
		Engine();
		~Engine();
		void loop();
};

#endif