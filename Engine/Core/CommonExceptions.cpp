#include "CommonExceptions.hpp"
#include <cstdio>

VulkanExceptions::VKCallFailure::VKCallFailure(const char* funcName, VkResult errCode)
{
    std::snprintf(this->msgBuffer.data(), this->msgBuffer.size(),
        "Vulkan API Error: %s call returned %d", funcName, errCode);
}

const char* VulkanExceptions::VKCallFailure::what() const throw() { return this->msgBuffer.data(); }