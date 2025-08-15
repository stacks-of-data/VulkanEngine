#ifndef ENGINE_HPP
#define ENGINE_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <exception>
#include <vector>
#include <array>
#include "DebugMessenger.hpp"

#ifndef DEBUG
	#define DEBUG 0
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
            std::array<char, 256> msgBuffer;
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
		bool m_glfwInitialized;
		bool m_vkInitialized;
		GLFWwindow* m_window;
		VkInstance m_instance;
		std::vector<const char*> m_layers;
		std::vector<const char*> m_extensions;
		DebugMessenger m_debugMessenger;
		void initGlfw();
		void initVulkan();
		void cleanupGlfw();
		void cleanupVulkan();
		void cleanup();
		void selectPhysicalDevice();
	public:
		Engine();
		~Engine();
		void loop();
};

#endif