#version 460

// In GLSL, we usually combine the Texture and Sampler into a single sampler2D
layout(binding = 0, set = 2) uniform sampler2D uTexture;

// These 'in' variables must match the 'out' locations from your Vertex Shader
layout(location = 0) in vec2 inTexCoord;
layout(location = 1) in vec4 inColor;

// The output variable replaces SV_Target0
layout(location = 0) out vec4 outFragColor;

void main()
{
    // Texture.Sample(Sampler, Coord) becomes texture(sampler, coord)
    outFragColor = inColor * texture(uTexture, inTexCoord);
}