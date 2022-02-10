cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(VkTablet VERSION 0.1.0 LANGUAGES C)

# Prevent VkTablet from being added multiple times
if(TARGET VkTablet)
	return()
endif()

# Add VkTablet headers to the include directories
include_directories(${CMAKE_CURRENT_LIST_DIR}/Include)
set(VkTablet_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/Include CACHE PATH 
	"Path to the VkTablet headers")

# Build up a list of VkTablet headers, these are the declarations
# Used to interact with the library, they are all in include/VkTablet
set(HeaderPath ${CMAKE_CURRENT_LIST_DIR}/Include/VkTablet)
set(VkTabletHeaders
	${HeaderPath}/VkTablet.h
	${HeaderPath}/VkTabletEnums.h
	${HeaderPath}/VkTabletInstance.h)

# Create the library target, we need to add at lease one C file so that
# CMake can determine the linker language 
# We also add in the headers at the same time so they can be tracked inside Cmake
add_library(VkTablet Source/Main.c ${VkTabletHeaders})

# Now go into the source directory to get the rest of the library
add_subdirectory(Source)

# Finally as you may remember, VkTablet is built on top of VkClay
# So we also need to add in VkClay. The user can either add VK_CLAY_ROOT
# Or we can assume they're building Mesopatamic as a whole project
if(NOT DEFINED VK_CLAY_ROOT)
	set(VK_CLAY_ROOT "${CMAKE_CURRENT_LIST_DIR}../" 
		CACHE PATH "Path to the root of VkClay")
endif()

# Add in the VkClay cmake list
add_subdirectory(${VK_CLAY_ROOT} VkClay)

# Link and include VkClay
target_include_directories(VkTablet PUBLIC ${VkClay_INCLUDE_DIRS})
target_link_libraries(VkTablet PUBLIC VkClay)

######################################################################################

# Organise all source files, this is done last
# Get all of the source files added to the project
get_target_property(PROJ_SOURCES VkTablet SOURCES)

# Add a little function that sorts all of the files to be
# Yoinked from https://stackoverflow.com/a/31423421
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

assign_source_group(${PROJ_SOURCES})