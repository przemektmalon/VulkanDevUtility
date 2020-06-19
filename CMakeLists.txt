cmake_minimum_required(VERSION 3.5.1)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/vdu")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

project(vdu)
set(CMAKE_CXX_STANDARD 14)

file(GLOB INCLUDES ${INCLUDE_DIR}/*.hpp)
file(GLOB SOURCES ${SRC_DIR}/*.cpp)

find_package(Vulkan REQUIRED)

if(WIN32)
  set(LIB_SHADERC "$ENV{VULKAN_SDK}/Lib/shaderc_combined.lib")
elseif(UNIX)
  set(LIB_SHADERC "$ENV{VULKAN_SDK}/lib/libshaderc_combined.a")
endif()

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -pthread")
endif()

include_directories(${INCLUDE_DIR} ${Vulkan_INCLUDE_DIRS})
add_library(vdu STATIC ${SOURCES} ${INCLUDES})
target_link_libraries(vdu ${Vulkan_LIBRARY})
target_link_libraries (vdu ${LIB_SHADERC})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT vdu)
set_target_properties(vdu PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")

install(
    TARGETS vdu
      EXPORT vdu-targets
      LIBRARY DESTINATION "./lib/"
	  ARCHIVE DESTINATION "./lib/"
	  RUNTIME DESTINATION 
      FRAMEWORK DESTINATION Library/Frameworks
      COMPONENT library)
  install(
    EXPORT vdu-targets
      DESTINATION "./lib/cmake/vdu"
      FILE vdu-config.cmake
	  COMPONENT headers)

if (VDU_BUILD_TESTS)
	add_subdirectory("./tests")
else()
	set(VDU_BUILD_TESTS CACHE BOOL OFF)
endif (VDU_BUILD_TESTS)
