#pragma once

#include "MathLib.h"

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
