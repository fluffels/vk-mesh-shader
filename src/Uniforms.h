#pragma once

#pragma pack (push, 1)
struct Uniforms {
    float proj[16];
    Vec4 eye;
    Quaternion rotation;
};
#pragma pack (pop)
