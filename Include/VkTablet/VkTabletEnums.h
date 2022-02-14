/**
 * List all of the VkTablet enums, primarily success codes. The naming rule for enums is vkt_(alllowercasse),
 * this makes it (hopefully) easier to intellisense since vkt_(CamelCase) is a function
 */
#ifndef __VK_TABLET_ENUMS_H__
#define __VK_TABLET_ENUMS_H__

typedef enum vkt_enum {
    vkt_success = 0,  // Had a successful running
    vkt_unknownfail,  // A fail that we haven't associated a code to yet
    vkt_clayfail,     // vkclay has not been initalised
    vkt_nullpointer,  // Passed a null pointer in a place it doesn't belong
    vkt_nomessenger,  // The instance does not support debug messengers, not a fatal error.
} vkt_enum;

#endif  // !__VK_TABLET_ENUMS_H__
