#pragma once

#include <array>
#include <exception>
#include <vulkan/vulkan.h>

namespace VulkanExceptions {
class VKCallFailure : public std::exception {
private:
    std::array<char, 1024> msgBuffer;

public:
    VKCallFailure(const char* funcName, VkResult errCode);
    const char* what() const throw();
};
}