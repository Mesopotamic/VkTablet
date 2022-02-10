/**
 * The role of main is to provide globally used internal functionality inside of VkTablet
 */
#include "VkTabletCommon.h"

#include <stdbool.h>

bool vkclayHasInit = false;

vkt_enum ensureVkClay()
{
    // Exit early if clay has already been opened
    if (vkclayHasInit) return vkt_success;

    // Have not inited tablet yet
    vkcenum err = vkc_LoadVulkan();
    if (err != vkc_success) {
        return vkt_clayfail;
    }

    // Tablet was successful to init
    vkclayHasInit = true;
    return vkt_success;
}
