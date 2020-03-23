cmake_minimum_required(VERSION 3.5.1)

################################################################
################    SET SOME DIRECTORY CONSTANTS
################################################################

set(LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/vdu")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")


################################################################
################    CREATE PROJECT AND GET ALL SOURCE FILES
################################################################

project(vdu)
set(CMAKE_CXX_STANDARD 14)

file(GLOB INCLUDES ${INCLUDE_DIR}/*.hpp)
file(GLOB SOURCES ${SRC_DIR}/*.cpp)

find_package(Vulkan REQUIRED)


################################################################
################    SET SHADERC INCLUDE AND LIBRARY
################################################################


if(WIN32)
  set(LIB_SHADERC_DEBUG "$ENV{VULKAN_SDK}/shaderc/build/libshaderc/Debug/shaderc_combined.lib")
  set(LIB_SHADERC_RELEASE "$ENV{VULKAN_SDK}/shaderc/build/libshaderc/Release/shaderc_combined.lib")
endif()
if(UNIX)
  set(LIB_SHADERC_DEBUG "$ENV{VULKAN_SDK}/lib/libshaderc_combined.a")
  set(LIB_SHADERC_RELEASE "$ENV{VULKAN_SDK}/lib/libshaderc_combined.a")
endif()


################################################################
################    SET SOME CMAKE VARIABLES
################################################################

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)


################################################################
################    SET GCC FLAGS
################################################################

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -pthread")
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -pthread")
endif()



################################################################
################    SPECIFY INCLUDE DIRECTORIES
################################################################

include_directories(${INCLUDE_DIR} ${LIBS_DIR} ${Vulkan_INCLUDE_DIRS})


################################################################
################    SPECIFY EXECUTABLE AND LINK LIBRARIES
################################################################

add_library(vdu STATIC ${SOURCES} ${INCLUDES})

target_link_libraries(vdu ${Vulkan_LIBRARY})
target_link_libraries (vdu
    debug ${LIB_SHADERC_DEBUG}
    optimized ${LIB_SHADERC_RELEASE})


################################################################
################    SET MSVC STARTUP PROJECT AND WORKING DIR
################################################################

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