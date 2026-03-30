#version 460

layout (location = 0) in vec4 v_color;
layout (location = 0) out vec4 FragColor;

layout(std140, set = 3, binding = 0) uniform UniformBlock {
    float time;
};

void main()
{
    float pulse = 
    sin(time * 2.5 + gl_FragCoord.x * 0.09)                                                  * 0.025f + 
    sin(time * 2.9 - gl_FragCoord.y * 0.085)                                                 * 0.025f + 
    sin(time * 3.0 + (gl_FragCoord.y * 0.74f +  gl_FragCoord.x * 0.7f) * 0.075 + 6.1f)       * 0.025f + 
    sin(time * 3.0 - (gl_FragCoord.y * 0.6f +   gl_FragCoord.x * 0.8f) * 0.04 + 3.7f)        * 0.025f + 
    sin(time * 2.8 + (gl_FragCoord.y * 0.6f +   gl_FragCoord.x * 0.2f) * 0.045f)             * 0.025f + 
    sin(time * 2.3 - (gl_FragCoord.y * 0.54f +  gl_FragCoord.x * 0.7f) * 0.045 + 2.8f)       * 0.025f + 
    sin(time * 2.0 + (gl_FragCoord.y * 0.35 -   gl_FragCoord.x * 0.7f) * 0.045f)             * 0.025f + 
    sin(time * 3.0 + (gl_FragCoord.y * 0.4f -   gl_FragCoord.x * 0.03f) * 0.045f)            * 0.025f + 
    sin(time * 1.2 - (gl_FragCoord.y * 1.6f -   gl_FragCoord.x * 0.4f) + 3.0f)               * 0.025f + 
    sin(time * 2.0 - (gl_FragCoord.y * 0.4f +   gl_FragCoord.x * 1.3f) * 0.025f)             * 0.025f + 
    sin(time * 3.0 + (gl_FragCoord.y * 0.42f +  gl_FragCoord.x * 0.7f) * 0.025f)             * 0.025f + 
    sin(time * 2.4 - (gl_FragCoord.y * 0.54f -  gl_FragCoord.x * 5.1f) * 0.02f)              * 0.025f + 
    sin(time * 4.0 - (gl_FragCoord.y * 0.6f +   gl_FragCoord.x * 0.2f) * 0.045f)             * 0.025f + 
    sin(time * 1.5 + (gl_FragCoord.y * 0.8f +   gl_FragCoord.x * 0.42f) * 0.045f)            * 0.025f + 
    sin(time * 2.5 - (gl_FragCoord.y * 0.35f -  gl_FragCoord.x * 0.7f) * 0.025f)             * 0.025f + 
    sin(time * 3.0 + (gl_FragCoord.y * 0.4f -   gl_FragCoord.x * 1.88f) * 0.01f)             * 0.025f + 
    sin(time * 1.9 - (gl_FragCoord.y * 1.6f +   gl_FragCoord.x * 0.48f) * 0.01f)             * 0.025f + 
    sin(time * 2.6 + (gl_FragCoord.y * 1.0f +   gl_FragCoord.x * 1.3f) * 0.125f)             * 0.025f + 
    sin(time * 0.5 - (gl_FragCoord.y * 1.6f -   gl_FragCoord.x * 1.62f) * 0.066f)            * 0.025f + 
    sin(time * 0.6 + (gl_FragCoord.y * 0.6f -   gl_FragCoord.x * 1.03f) * 0.065f)            * 0.025f + 
    0.5f;

    float sparcles = 
    sin(time * 3.0f + (gl_FragCoord.y * 0.74f +  gl_FragCoord.x * 0.7f) * 0.075f + 6.1f)      * 0.1f + 
    sin(time * 3.0f - (gl_FragCoord.y * 0.6f +   gl_FragCoord.x * 0.8f) * 0.04f + 3.7f)       * 0.1f + 
    sin(time * 2.8f + (gl_FragCoord.y * 0.6f +   gl_FragCoord.x * 0.2f) * 0.045f)             * 0.1f + 
    sin(time * 2.3f - (gl_FragCoord.y * 0.54f +  gl_FragCoord.x * 0.7f) * 0.045f + 2.8f)      * 0.1f + 
    sin(time * 2.0f + (gl_FragCoord.y * 0.35f -   gl_FragCoord.x * 0.7f) * 0.045f)            * 0.1f + 
    sin(time * 3.0f + (gl_FragCoord.y * 0.4f -   gl_FragCoord.x * 0.03f) * 0.045f)            * 0.1f + 
    sin(time * 1.9f - (gl_FragCoord.y * 1.6f +   gl_FragCoord.x * 1.48f) * 0.71f)             * 0.1f + 
    sin(time * 1.6f + (gl_FragCoord.y * 1.9f +   gl_FragCoord.x * 1.3f) * 0.55f)              * 0.1f + 
    sin(time * 1.5f - (gl_FragCoord.y * 1.6f -   gl_FragCoord.x * 1.62f) * 0.061f)            * 0.1f + 
    sin(time * 1.6f + (gl_FragCoord.y * 1.4f -   gl_FragCoord.x * 1.53f) * 0.065f)            * 0.1f;

    FragColor = vec4(0.9f, 1.0f, 1.0f, float(pulse >= 0.65f) * 0.1f * (sparcles +  0.5f) +          // main water blobs
    float(pulse <= 0.52f && pulse >= 0.48f && sparcles >= 0.56f) * 0.5f +                           // little water outlines
    float(pulse <= 0.515f && pulse >= 0.485f) * sparcles * 0.25f);                                  // sparcles
}