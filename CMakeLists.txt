project(GpuInfo)
cmake_minimum_required(VERSION 3.0)

# Local CMake Modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# link_libraries needs to propertly handle relative path
cmake_policy(SET CMP0015 NEW)

set(NVAPI_ROOT_DIR deps/NVAPI/)
find_package(nvapi REQUIRED)

set(SOURCES
    main.cc
    include/GpuInfo.h
    src/GpuInfo.cc)
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_include_directories(${PROJECT_NAME} PRIVATE ${NVAPI_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE nvapi)
