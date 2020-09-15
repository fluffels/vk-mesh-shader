#version 450
#extension GL_ARB_separate_shader_objects : enable

#include "uniforms.glsl"

layout(push_constant) uniform PushConstants {
    vec3 color;
} pushConstants;

layout(location=0) in vec3 inPosition;

layout(location=0) out vec3 outColor;

void main() {
    gl_Position = uniforms.mvp * vec4(inPosition, 1.0);
    outColor = pushConstants.color;
}
