#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding=2) uniform sampler2D sp;

layout(location=0) in vec2 uv;

layout(location=0) out vec4 outColor;

void main() {
    vec4 spColor = texture(sp, uv);
    if (spColor.a < .5) discard;
    outColor = vec4(spColor.rgb, 1);
}
