#include "VkTablet/VkTablet.h"
#include "VkTabletCommon.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Validation layer name
const char* validationLayerName = "VK_LAYER_KHRONOS_validation";

// The variables used to cache the instance extensions properties
VkExtensionProperties* m_instanceExtProps = NULL;
uint32_t m_instanceExtPropsCount = 0;

// Variables used to cache the layer properties
VkLayerProperties* m_layerProps = NULL;
uint32_t m_layerPropsCount = 0;

VkExtensionProperties* vkt_InstanceGetExtensionProps(uint32_t* pExtCount)
{
    if (m_instanceExtProps == NULL || m_instanceExtProps == 0) {
        // Try to init vkclay has been initialised first
        if (ensureVkClay() != vkt_success) {
            return NULL;
        }

        // VkClay has been successful, but we haven't checked any instance extensions yet
        // So initialise the list of extension properties with those that don't require a layer
        // Dosen't matter if there is a fail since we return this pointer anyway
        vkEnumerateInstanceExtensionProperties(NULL, &m_instanceExtPropsCount, NULL);
        m_instanceExtProps = malloc(m_instanceExtPropsCount * sizeof(VkExtensionProperties));
        if (m_instanceExtProps == NULL) {
            return NULL;
        }
        vkEnumerateInstanceExtensionProperties(NULL, &m_instanceExtPropsCount, m_instanceExtProps);
    }

    if (pExtCount != NULL) {
        *pExtCount = m_instanceExtPropsCount;
    }
    return m_instanceExtProps;
}

VkExtensionProperties* vkt_InstanceGetLayerProps(uint32_t* pLayerCount)
{
    if (m_layerProps == NULL || m_layerPropsCount == 0) {
        // Try to ensure vkclay
        if (ensureVkClay() != vkt_success) {
            return NULL;
        }

        // Haven't ever searched layer props and vkclay initilised okay
        vkEnumerateInstanceLayerProperties(&m_layerPropsCount, NULL);
        m_layerProps = malloc(m_layerPropsCount * sizeof(VkLayerProperties));
        if (m_layerProps == NULL) {
            return NULL;
        }
        vkEnumerateInstanceLayerProperties(&m_layerPropsCount, m_layerProps);
    }

    *pLayerCount = m_layerPropsCount;
    return m_layerProps;
}

vkt_enum vkt_InstanceFactoryBase(VkInstanceCreateInfo* pInstanceCI)
{
    CLAY_INIT
    // Null pointer check
    if (pInstanceCI == NULL) return vkt_nullpointer;

    // Allocate a pointer on the heap for the app info
    VkApplicationInfo* pAppInfo = malloc(sizeof(VkApplicationInfo));
    if (pAppInfo == NULL) {
        return vkt_unknownfail;
    }

    // So we have a pointer to instance create info, lets reset it
    memset(pInstanceCI, 0, sizeof(VkInstanceCreateInfo));
    pInstanceCI->sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    memset(pAppInfo, 0, sizeof(VkApplicationInfo));
    pAppInfo->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    pInstanceCI->pApplicationInfo = pAppInfo;

    // Set minimal requirements for a 1.0 Vulkan instance
    pAppInfo->apiVersion = VK_MAKE_VERSION(1, 0, 0);
    pAppInfo->applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    pAppInfo->engineVersion = VK_MAKE_VERSION(0, 0, 1);
    pAppInfo->pEngineName = "VkTablet";
    pAppInfo->pApplicationName = "Application";

    return vkt_success;
}

vkt_enum vkt_InstanceFactoryRT(VkInstanceCreateInfo* pInstanceCI)
{
    // Fill the basic instance create info, and then exit out early on a fail
    vkt_enum err = vkt_InstanceFactoryBase(pInstanceCI);
    if (err != vkt_success) return err;

    err = vkt_InstanceAttachDebugUtils(pInstanceCI);
    if (err != vkt_success) return err;

    return vkt_success;
}

vkt_enum vkt_InstanceAttachDebugUtils(VkInstanceCreateInfo* pInstanceCI)
{
    // Do a null pointer check
    if (pInstanceCI == NULL) return vkt_nullpointer;

#ifdef NDEBUG
    // If we're in Release mode then return early with success
    return vkt_success;
#endif

    // So now check all of the instance layers for the validation layers
    uint32_t propsCount = 0;
    VkLayerProperties* props = vkt_InstanceGetLayerProps(&propsCount);

    bool found = false;
    for (uint32_t i = 0; i < propsCount; i++) {
        if (!strcmp(props[i].layerName, validationLayerName)) {
            found = true;
            break;
        }
    }

    if (!found) {
        // Validation layers requested but not supported
        // They're not required, so bail out without reporting
        return vkt_success;
    }

    // We found the layer, so realloc the instance create info to make room for the layer
    char** ppLayerNames =
      realloc(pInstanceCI->ppEnabledLayerNames, (pInstanceCI->enabledLayerCount + 1) * sizeof(char*));
    if (ppLayerNames == NULL) {
        return vkt_unknownfail;  // failed to allocate memory for the instance layers
    }

    // Successfully realloced space for the new layer, allocate enough space for the string and append it
    ppLayerNames[pInstanceCI->enabledLayerCount] = malloc(sizeof(char) * (strlen(validationLayerName) + 1));
    if (ppLayerNames[pInstanceCI->enabledLayerCount] == NULL) return vkt_unknownfail;

    strcpy(ppLayerNames[pInstanceCI->enabledLayerCount], validationLayerName);
    pInstanceCI->enabledLayerCount++;

    // Attempt to free the current layers and then replace the current list of layers with the new realloced
    // pointer
    if (pInstanceCI->ppEnabledLayerNames != NULL) {
        free(pInstanceCI->ppEnabledLayerNames);
    }
    pInstanceCI->ppEnabledLayerNames = ppLayerNames;

    /**
     * We have now enabled the validation layer in the instance create info, this layer might provide us with
     * more instance extensions, so due to this, we have to add the list of extensions provided by the layer
     * into our internal cached list of instance extensions
     */
    uint32_t validationExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(validationLayerName, &validationExtensionCount, NULL);
    if (validationExtensionCount != 0) {
        // We have found extra extension properties to append to the local list of extensions, so we can
        // reallocate the instance extensions list
        vkt_InstanceGetExtensionProps(NULL);
        VkExtensionProperties* extProps =
          realloc(m_instanceExtProps,
                  sizeof(VkExtensionProperties) * (m_instanceExtPropsCount + validationExtensionCount));

        // If the alloc was successful, append the newly found extensions to our list. We can save ourselves a
        // memcopy, by feeding in the first free array index
        if (extProps != NULL) {
            vkEnumerateInstanceExtensionProperties(validationLayerName, &validationExtensionCount,
                                                   extProps + m_instanceExtPropsCount);
            // Now we store our new result as the cache
            m_instanceExtProps = extProps;
            m_instanceExtPropsCount += validationExtensionCount;
        }
    }

    /**
     * Now that we have enabled validation layers, let's also try to enable best practices as well. we do this
     * with the VK_EXT_validation_features instance extension, we'll later be adding another instance
     * extension, so we make room for two potential extensions
     */
    char* requestedInstanceExtensions[2] = {"VK_EXT_validation_features", NULL};
    uint32_t validatedExtCount = 0;

    // Look for the validation features instance extension in the cache
    found = false;
    for (uint32_t i = 0; i < m_instanceExtPropsCount; i++) {
        if (!strcmp(m_instanceExtProps[i].extensionName, requestedInstanceExtensions[0])) {
            found = true;
            break;
        }
    }

    // If we found VK_EXT_validation_features then enable best practices and debug printf
    if (found) {
        // Push the validated extCount forward, since we found this extension
        validatedExtCount++;

        VkValidationFeatureEnableEXT enables[2] = {VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
                                                   VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT};
        // The validation features are enabled by pushing things onto the pNext chain, once again that means
        // we have to malloc
        VkValidationFeaturesEXT* features = malloc(sizeof(VkValidationFeaturesEXT));
        if (features == NULL) {
            // we failed on a malloc
            return vkt_unknownfail;
        }
        memset(features, 0, sizeof(VkValidationFeaturesEXT));
        features->sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        features->enabledValidationFeatureCount = 2;
        features->pEnabledValidationFeatures = malloc(2 * sizeof(VkValidationFeatureEnableEXT));
        if (features->pEnabledValidationFeatures != NULL) {
            memcpy(features->pEnabledValidationFeatures, enables, sizeof(enables));
        }

        // Push the enable features onto the pNext chain
        features->pNext = pInstanceCI->pNext;
        pInstanceCI->pNext = features;
    }

    /**
     * Now that we have validation layers, now lets check if we can control how those layers are printed out
     * with a debug messenger which requires the VK_EXT_debug_utils instance extension
     */

    // Check that each of the found instance extensions, have we found VK_EXT_debug_utils?
    for (uint32_t i = 0; i < m_instanceExtPropsCount; i++) {
        if (!strcmp(m_instanceExtProps[i].extensionName, "VK_EXT_debug_utils")) {
            // Found the debug utils extension, so append it to the requested extensions
            requestedInstanceExtensions[validatedExtCount] = "VK_EXT_debug_utils";
            validatedExtCount++;
            break;
        }
    }

    /**
     * Finally we can add the validated requested instance extensions to the instance create info
     */

    if (validatedExtCount == 0) {
        // Exit early, but we were still successful
        return vkt_success;
    }

    // Make enough room in the instance extensions for the confirmed availble requested extensions, otherwise
    // known as the validated instance extensions
    char** validatedInstanceExtensions =
      realloc(pInstanceCI->ppEnabledExtensionNames,
              (pInstanceCI->enabledExtensionCount + validatedExtCount) * sizeof(char*));
    if (validatedInstanceExtensions == NULL) return vkt_success;

    for (uint32_t i = 0; i < validatedExtCount; i++) {
        // Get how large the current instance extension name is in bytes
        uint32_t extNameSize = sizeof(char) * (strlen(requestedInstanceExtensions[i]) + 1);

        // malloc enough space for the extension name
        validatedInstanceExtensions[pInstanceCI->enabledExtensionCount + i] = malloc(extNameSize);

        // Now copy it into the array
        memcpy(validatedInstanceExtensions[pInstanceCI->enabledExtensionCount + i],
               requestedInstanceExtensions[i], extNameSize);
    }
    // Free and replace the old instance extensions list
    if(pInstanceCI->ppEnabledExtensionNames != NULL) {
        free(pInstanceCI->ppEnabledExtensionNames);
    }
    pInstanceCI->ppEnabledExtensionNames = validatedInstanceExtensions;
    pInstanceCI->enabledExtensionCount += validatedExtCount;

    return vkt_success;
}

vkt_enum vkt_InstanceFactoryFreeCreateInfo(VkInstanceCreateInfo* pInstanceCI)
{
    // We have a very special way of dealing with create infos made in tablet.
    // To enable flexibility, we have to dynamically assign pointers inside create infos
    // So lets provide a nice easier way for users to free create infos

    // The Instance create info itself should be stack allocated, and incase it's not the user is responsible
    // for clean up

    // Free the app info
    free(pInstanceCI->pApplicationInfo);

    // Free the array of enabled instance extensions
    for (uint32_t i = 0; i < pInstanceCI->enabledExtensionCount; i++) {
        // This is a dynamically allocated array of strings
        // Free each string
        free(pInstanceCI->ppEnabledExtensionNames[i]);
    }
    // Free the array containing the strings
    free(pInstanceCI->ppEnabledExtensionNames);

    // Same for the layers
    for (uint32_t i = 0; i < pInstanceCI->enabledLayerCount; i++) {
        free(pInstanceCI->ppEnabledLayerNames[i]);
    }
    free(pInstanceCI->ppEnabledLayerNames);

    // Go through the pNext chain and free everything, Vulkan has a special struct for handelling pNext
    // elements, it lets us find the sType and pNext from the void pointer
    VkBaseInStructure* pNext = pInstanceCI->pNext;
    while (pNext) {
        // Get the next pNext chain
        VkBaseInStructure* next = pNext->pNext;

        // switch case all of the things we know usually end up on an instance create info pNext change
        switch (pNext->sType) {
            case (VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT): {
                VkValidationFeaturesEXT* validationEnables = (VkValidationFeaturesEXT*)(pNext);
                free(validationEnables->pEnabledValidationFeatures);
                break;
            }
            default:
                break;
        }

        free(pNext);
        pNext = next;
    }
}
