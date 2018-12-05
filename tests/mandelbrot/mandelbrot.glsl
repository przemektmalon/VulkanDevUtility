// Adapted from:
// https://rosettacode.org/wiki/Mandelbrot_set#GLSL

#version 450
layout(local_size_x=16, local_size_y=16, local_size_z=1) in;

layout(binding=0, rgba8) uniform writeonly image2D outColour;

layout(push_constant) uniform Data {
    uvec2 resolution;
    int iterations;
} data;

void main()
{
	const uvec2 res = data.resolution;

	uvec3 gid = gl_GlobalInvocationID;
	ivec2 pixel = ivec2(gid.xy);

	vec2 uv = vec2(gid.x, gid.y) / vec2(res.x, res.y);
	float scale = float(res.y) / float(res.x);
	uv = ((uv - 0.5) * 5.5);
	uv.x -= 0.5;
	uv.y *= scale;

	vec2 z = vec2(0.0, 0.0);
	vec3 c = vec3(0.0, 0.0, 0.0);
	float v;

	for(int i = 0; i < data.iterations; i++)
	{
		if(((z.x * z.x + z.y * z.y) >= 4.0)) break;
		z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.y * z.x) + uv;

		if((z.x * z.x + z.y * z.y) >= 2.0)
		{
			c.b = float(i) / 20.0;
			c.r = sin((float(i) / 5.0));
		}
	}

 	imageStore(outColour, pixel, vec4(c,1.f));
}
