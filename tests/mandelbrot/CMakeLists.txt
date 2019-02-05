
project(Mandelbrot)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Mandelbrot "mandelbrot.cpp")

target_link_libraries(Mandelbrot VulkanDevUtility)

target_link_libraries(Mandelbrot ${Vulkan_LIBRARY})
target_link_libraries(Mandelbrot
    debug ${LIB_SHADERC_DEBUG}
    optimized ${LIB_SHADERC_RELEASE})

if ( MSVC )
    set_target_properties(Mandelbrot PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
    set_target_properties(Mandelbrot PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/" )
    set_target_properties(Mandelbrot PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/" )
    set_target_properties(Mandelbrot PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/" )
endif ( MSVC )