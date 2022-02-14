cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

set(SOURCES
	${CMAKE_CURRENT_LIST_DIR}/InstanceFactory.c)

target_sources(VkTablet PRIVATE ${SOURCES} ${HEADERS})