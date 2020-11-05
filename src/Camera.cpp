#include "Camera.h"
#include "MathLib.cpp"

#include <glm/gtc/matrix_transform.hpp>

using glm::cross;
using glm::lookAt;
using glm::normalize;
using glm::perspective;
using glm::rotate;
using glm::vec4;

#define PI 3.14159265358979323846f

void Camera::get(float* m) {
    float view[16];
    matrixView(eye, at, down, view);

    float proj[16];
    matrixInit(proj);

    const float ar = width / (float)height;
    const float halfFOV = fov / 2.f;
    const float halfTanFOV = tanf(halfFOV);

    proj[0] = 1 / (ar * halfTanFOV);
    proj[5] = 1 / halfTanFOV;
    proj[10] = 1 / (farz - nearz);
    proj[11] = 1;
    proj[14] = -nearz / (farz - nearz);

    mat4 test(1.0);
    test = perspective(fov, ar, nearz, farz);

    matrixMultiply(proj, view, m);
}

void Camera::setAR(uint32_t w, uint32_t h) {
    ar = (float)w / (float)h;
}

void Camera::setFOV(float f) {
    fov = glm::radians(f);
}

void Camera::back(float d) {
    forward(-d);
}

void Camera::left(float d) {
    right(-d);
}

void Camera::forward(float d) {
    Vec3 z;
    vectorSub(at, eye, z);
    vectorNormalize(z);
    vectorScale(d, z);
    vectorAdd(at, z, at);
    vectorAdd(eye, z, eye);
}

void Camera::right(float d) {
    Vec3 z;
    vectorSub(at, eye, z);
    vectorNormalize(z);
    Vec3 right;
    vectorCross(down, z, right);
    vectorNormalize(right);
    vectorScale(d, right);
    vectorAdd(at, right, at);
    vectorAdd(eye, right, eye);
}

void Camera::rotateY(float d) {
}

void Camera::rotateX(float d) {
}
