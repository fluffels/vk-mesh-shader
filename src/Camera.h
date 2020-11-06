#pragma once

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include "MathLib.h"

using glm::mat4;
using glm::vec3;

struct Camera {
    Vec3 at;
    Vec3 down;
    Vec3 eye;

    float fov;
    float ar;

    float nearz;
    float farz;
    uint32_t width;
    uint32_t height;

    Quaternion rotation;

    void get(float* m);

    void setAR(uint32_t, uint32_t);
    void setFOV(float);

    void back(float);
    void forward(float);
    void left(float);
    void right(float);
    
    void rotateY(float);
    void rotateX(float);
};
