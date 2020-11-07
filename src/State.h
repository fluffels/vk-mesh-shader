#pragma once

#include "Render.h"

void eventMoveForward(float d, Uniforms& u);
void eventMoveBackward(float d, Uniforms& u);
void eventMoveLeft(float d, Uniforms& u);
void eventMoveRight(float d, Uniforms& u);
void eventRotateY(float d, Uniforms& u);
void eventPositionReset(Uniforms& u);
