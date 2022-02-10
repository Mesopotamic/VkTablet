/**
 * Common internally globally accessable objects in VkTablet
 */
#ifndef __VK_TABLET_INTERNAL_COMMON_H__
#define __VK_TABLET_INTERNAL_COMMON_H__
#include "VkClay/VkClay.h"
#include "VkTablet/VkTabletEnums.h"

/**
 * @brief Ensures that VkClay has been initialised. Put at the front of common entry points
 * @returns vkt_success in most cases, hopefully,
 */
vkt_enum ensureVkClay();

// Make a macro that helps us exit out early if clay fails
#define CLAY_INIT                        \
    if (ensureVkClay() != vkt_success) { \
        return vkt_clayfail;             \
    }

#endif  // !__VK_TABLET_INTERNAL_COMMON_H__
