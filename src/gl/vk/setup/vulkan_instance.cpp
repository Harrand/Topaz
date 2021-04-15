#if TZ_VULKAN
#include "gl/vk/setup/vulkan_instance.hpp"
#include "core/assert.hpp"
namespace tz::gl::vk
{
    VulkanApplicationInfo::VulkanApplicationInfo(tz::GameInfo game_info):
    app_info(),
    engine_name(game_info.engine.to_string())
    {
        std::string engine_name = game_info.engine.to_string();
        auto ToVkVersion = [](tz::GameInfo::Version ver)->std::uint32_t
        {
            return VK_MAKE_VERSION(ver.major, ver.minor, ver.patch);
        };

        this->app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        this->app_info.pApplicationName = game_info.name;
        this->app_info.applicationVersion = ToVkVersion(game_info.version);
        
        this->app_info.pEngineName = engine_name.c_str();
        this->app_info.engineVersion = ToVkVersion(game_info.engine.version);
        this->app_info.apiVersion = VK_API_VERSION_1_0;
    }

    VkApplicationInfo VulkanApplicationInfo::get_info() const
    {
        return this->app_info;
    }

    VulkanInstance::VulkanInstance(VulkanApplicationInfo vk_app_info, ExtensionList extensions):
    instance()
    {
        VkInstanceCreateInfo create{};
        VkApplicationInfo app_info = vk_app_info.get_info();
        create.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create.pApplicationInfo = &app_info;

        // (Important) TODO: Assert on glfw being initialised. Otherwise UB.
        ExtensionList glfw_extensions = tz::gl::vk::get_glfw_required_extensions();
        // Add GLFW required extensions too.
        extensions.append(glfw_extensions);
        create.enabledExtensionCount = extensions.length();
        create.ppEnabledExtensionNames = extensions.data();

        // TODO: Validation Layers
        create.enabledLayerCount = 0;

        if(vkCreateInstance(&create, nullptr, &this->instance) != VK_SUCCESS)
        {
            tz_error("VulkanInstance::VulkanInstance(...): Failed to create instance.");
        }
    }
}

#endif