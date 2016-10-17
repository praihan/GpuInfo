project(GpuInfo)
cmake_minimum_required(VERSION 3.0)

# local cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# link_libraries needs to propertly handle relative path
cmake_policy(SET CMP0015 NEW)

# NVAPI_ROOT_DIR should default to the provided NVAPI
set(NVAPI_ROOT_DIR deps/NVAPI/)
find_package(nvapi REQUIRED)

set(SOURCES
    main.cc
    include/GpuInfo.h
    src/GpuInfo.cc)
add_executable(${PROJECT_NAME} ${SOURCES})

# the headers provided by this library
target_include_directories(${PROJECT_NAME} PUBLIC include)

# link nvapi and add include its dirs
target_include_directories(${PROJECT_NAME} PRIVATE ${NVAPI_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE nvapi)
