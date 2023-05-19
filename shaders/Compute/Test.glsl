#version 460 core
layout (local_size_x = 1, local_size_y = 1) in;

layout (std430, binding = 0) buffer lay0 { int inbuf[]; }
layout (std430, binding = 1) buffer lay1 { int outbuf[]; }

void main()
{
    const uint id = gl_GlobalInvocationID.x;
    outbuf[id] = inbuf[id] * inbuf[id];
}
