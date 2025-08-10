#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <exception>
#include <vector>
#include <string>

#ifndef ENGINE_DEBUG
	#define ENGINE_DEBUG 0
#endif

namespace EngineExceptions
{
	class GlfwInitFailure: public std::exception
	{
		const char* what() const throw();
	};
	class VkInitFailure: public std::exception
	{
		const char* what() const throw();
	};
}

class Engine
{
	private:
		bool glfwInitalized;
		bool vkInitalized;
		GLFWwindow* window;
		VkInstance vkInstance;
		std::vector<std::string> layers;
		void initGlfw();
		void initVulkan();
		void cleanupGlfw();
		void cleanupVulkan();
		void cleanup();
		void checkLayersSupport();
	public:
		Engine();
		~Engine();
		void loop();
};