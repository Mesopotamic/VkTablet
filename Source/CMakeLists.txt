cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

target_sources(VkTablet PRIVATE ${CMAKE_CURRENT_LIST_DIR}/VkTabletCommon.h)
target_include_directories(VkTablet PRIVATE ${CMAKE_CURRENT_LIST_DIR})
add_subdirectory(Instance)