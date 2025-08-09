#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <exception>

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
        void initGlfw();
        void initVulkan();
        void cleanupGlfw();
        void cleanupVulkan();
        void cleanup();
    public:
        Engine();
        ~Engine();
        void loop();
};