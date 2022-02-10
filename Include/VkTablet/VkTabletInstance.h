/**
 * Instance Level functions for VkTablet, such as making the instance quicker and easier to make
 */
#ifndef __VK_TABLET_INSTANCE_H__
#define __VK_TABLET_INSTANCE_H__
#include "VkClay/VkClay.h"
#include "VkTabletEnums.h"

/**
 * @breif Makes an instance create info that is ready to start raytracing
 * @param pInstanceCI Pointer to an vkInstanceCreateInfo that will get filled
 * @returns vkt success code, 0 on success
 */
vkt_enum vkt_InstanceFactoryRT(VkInstanceCreateInfo *pInstanceCI);
#endif  // !__VK_TABLET_INSTANCE_H__
