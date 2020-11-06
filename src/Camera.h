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

    Quaternion rotation;

    void back(float);
    void forward(float);
    void left(float);
    void right(float);
    
    void rotateY(float);
    void rotateX(float);
};
