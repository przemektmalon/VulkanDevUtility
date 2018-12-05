In less than 300 lines, this test:
- Creates a vulkan instance with validation layers enabled
- Chooses a vulkan capable physical device
- Creates a logical device with a compute and transfer queue
- Sets up some debugging/error-reporting callbacks
- Compiles a glsl shader
- Creates a device local texture
- Creates descriptor pool, layout, and set
- Creates a texture sampler
- Updates a descriptor set
- Creates command pool for each queue
- Allocates and writes to a command buffer
- Creates a compute pipeline and layout
- Creates a semaphore
- Creates a buffer for data transfer from device local to host visible memory
- Submits two command buffers (drawing and data transfer)
- Writes the generated texture to a BMP
- Cleans up everything

![Engine Image](https://github.com/przemektmalon/VulkanDevUtility/blob/master/tests/mandelbrot/mandelbrot.png)
