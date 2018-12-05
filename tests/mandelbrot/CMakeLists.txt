
project(Mandelbrot)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Mandelbrot "mandelbrot.cpp")

target_link_libraries(Mandelbrot VulkanDevUtility)

target_link_libraries(Mandelbrot ${LIB_VK_LIBRARY})
target_link_libraries(Mandelbrot
    debug ${LIB_SHADERC_DEBUG}
    optimized ${LIB_SHADERC_RELEASE})

set_target_properties(Mandelbrot PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")