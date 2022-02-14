/**
 * Instance Level functions for VkTablet, such as making the instance quicker and easier to make
 */
#ifndef __VK_TABLET_INSTANCE_H__
#define __VK_TABLET_INSTANCE_H__
#include "VkClay/VkClay.h"
#include "VkTabletEnums.h"

/**
 * @brief Retrieves the instance extension properties list
 * @param extCount How many instance extensions there are
 * @returns Pointer to the list of instance extension properties
 */
VkExtensionProperties* vkt_InstanceGetExtensionProps(uint32_t* extCount);

/**
 * @brief Retrieves all the instance layers properties
 * @param pLayerCount Pass a pointer to uin32_t to retrieve how many layers are found
 * @returns Pointer to the list of instance layer properties
 */
VkExtensionProperties* vkt_InstanceGetLayerProps(uint32_t* pLayerCount);

/**
 * @brief Makes a basic VkInstanceCreateInfo, mallocs space for the appinfo
 * @param pInstanceCreateInfo Pointer to the instance create info you want to fill
 * @returns vkt success code, 0 on success
 */
vkt_enum vkt_InstanceFactoryBase(VkInstanceCreateInfo* pInstanceCI);

/**
 * @brief Makes an instance create info that is ready to start raytracing, mallocs space for the app info and
 * layers and extensions
 * @param pInstanceCI Pointer to a blank vkInstanceCreateInfo that will get filled
 * @returns vkt success code, 0 on success
 */
vkt_enum vkt_InstanceFactoryRT(VkInstanceCreateInfo* pInstanceCI);

/**
 * @brief Attaches the required instance layers to the create info, mallocs the instance layers
 * @param pInstanceCI The VkInstanceCreateInfo to be appended to
 * @returns vkt success code, 0 on success
 */
vkt_enum vkt_InstanceAttachDebugUtils(VkInstanceCreateInfo* pInstanceCI);

/**
 * @brief Tries to free the instance create info created by tablet methodologies
 * @param pInstanceCI Pointer to the create info to free
 * @returns vkt success code, 0 on success
 */
vkt_enum vkt_InstanceFactoryFreeCreateInfo(VkInstanceCreateInfo* pInstanceCI);

/**
 * @brief Fills the contents of a Debug Utils Messenger create info which has default behaviour. Does no heap
 * allocations
 * @param pMessengerCI Pointer to the create info to fill
 * @returns vkt success code, 0 on success. vkt_nomessenger if messenger is not supported
 */
vkt_enum vkt_DebugMessengerFactory(VkDebugUtilsMessengerCreateInfoEXT* pMessengerCI);

/**
 * @brief Automatically creates a debug messenger using the default create info in tablet, and cleans up the
 * create info
 * @param instance Vulkan handle to the instance
 * @param pMessenger Pointer to the Debug Utils Messenger to be created
 * @returns vkt success code, 0 on success. vkt_nomessenger if messenger is not supported
 */
vkt_enum vkt_DebugMessengerCreateDefault(VkInstance instance, VkDebugUtilsMessengerEXT* pMessenger);
#endif  // !__VK_TABLET_INSTANCE_H__
