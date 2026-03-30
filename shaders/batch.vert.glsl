#version 460

// Data structures
struct SpriteData {
    vec3 Position;
    float Rotation;
    vec2 Scale;
    vec2 Padding;
    float TexU, TexV, TexW, TexH;
    vec4 Color;
};

// StructuredBuffer translates to an SSBO (Shader Storage Buffer Object)
layout(std430, binding = 0) readonly buffer DataBuffer {
    SpriteData sprites[];
};

// cbuffer translates to a UBO (Uniform Buffer Object)
layout(std140, binding = 1) uniform UniformBlock {
    mat4 ViewProjectionMatrix;
};

// GLSL requires explicit type constructors for arrays
const uint triangleIndices[6] = uint[](0, 1, 2, 3, 2, 1);
const vec2 vertexPos[4] = vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(1.0, 1.0)
);

// Outputs to the fragment shader
layout(location = 0) out vec2 outTexcoord;
layout(location = 1) out vec4 outColor;

void main()
{
    // Use gl_VertexIndex for Vulkan, or gl_VertexID for OpenGL
    uint id = gl_VertexIndex; 
    
    uint spriteIndex = id / 6;
    // Note: Changed from `spriteIndex % 6` to `id % 6` to correctly pull the vertex index
    uint vert = triangleIndices[id % 6]; 
    
    SpriteData sprite = sprites[spriteIndex];

    vec2 texcoord[4] = vec2[](
        vec2(sprite.TexU,               sprite.TexV              ),
        vec2(sprite.TexU + sprite.TexW, sprite.TexV              ),
        vec2(sprite.TexU,               sprite.TexV + sprite.TexH),
        vec2(sprite.TexU + sprite.TexW, sprite.TexV + sprite.TexH)
    );

    float c = cos(sprite.Rotation);
    float s = sin(sprite.Rotation);

    vec2 coord = vertexPos[vert];
    coord *= sprite.Scale;
    
    // GLSL matrices are column-major. 
    // mat2(c, s, -s, c) sets Column 0 to (c, s) and Column 1 to (-s, c).
    // Multiplying matrix * vector achieves the same math as your HLSL mul(vector, matrix).
    mat2 rotation = mat2(c, s, -s, c);
    coord = rotation * coord;

    vec3 coordWithDepth = vec3(coord + sprite.Position.xy, sprite.Position.z);

    // GLSL uses the * operator instead of mul()
    gl_Position = ViewProjectionMatrix * vec4(coordWithDepth, 1.0);
    outTexcoord = texcoord[vert];
    outColor = sprite.Color;
}