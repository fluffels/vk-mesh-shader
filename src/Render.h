#pragma once

#include "MathLib.h"
#include "Uniforms.h"
#include "Vulkan.h"

void renderInit(Vulkan& vk, Uniforms& uniforms);
void renderFrame(Vulkan& vk, char* debugString);
