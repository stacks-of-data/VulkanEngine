#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class GlfwContext {
private:
    GLFWwindow* m_window;

public:
    GlfwContext();
    ~GlfwContext();
    GLFWwindow* getWindow();
    void loop();
};