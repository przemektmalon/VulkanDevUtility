
project(Mandelbrot)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Mandelbrot "mandelbrot.cpp")

target_link_libraries(Mandelbrot vdu)
target_link_libraries(Mandelbrot ${Vulkan_LIBRARIES})
target_link_libraries(Mandelbrot ${LIB_SHADERC})

if(MSVC)
    set_target_properties(Mandelbrot PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
    set_target_properties(Mandelbrot PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
    set_target_properties(Mandelbrot PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/")
    set_target_properties(Mandelbrot PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/")
endif(MSVC)
