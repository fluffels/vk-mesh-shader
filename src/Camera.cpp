#include "Camera.h"
#include "MathLib.cpp"

void Camera::back(float d) {
    forward(-d);
}

void Camera::left(float d) {
    right(-d);
}

void Camera::forward(float d) {
    Quaternion dir = {};
    dir.z = 1;
    quaternionUnrotate(rotation, dir);
    eye.x += d * dir.x;
    eye.y += d * dir.y;
    eye.z += d * dir.z;
}

void Camera::right(float d) {
    Quaternion dir = {};
    dir.x = 1;
    quaternionUnrotate(rotation, dir);
    eye.x += d * dir.x;
    eye.y += d * dir.y;
    eye.z += d * dir.z;
}

void Camera::rotateY(float d) {
    auto rad = toRadians(d);
    auto delta = quaternionFromAngleAxis(0, -1, 0, rad);
    rotation = quaternionMultiply(delta, rotation);
    quaternionNormalize(rotation);
}

void Camera::rotateX(float d) {
}
