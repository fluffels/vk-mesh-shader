#include "Render.h"
#include "RenderMesh.h"
#include "RenderText.h"

#include "MathLib.cpp"

vector<VkCommandBuffer> meshCmds;
vector<VkCommandBuffer> textCmds;

void renderInit(Vulkan& vk, Uniforms& uniforms) {
    auto fov = toRadians(45);
    auto height = vk.swap.extent.height;
    auto width = vk.swap.extent.width;
    auto nearz = 1.f;
    auto farz = 10.f;
    matrixProjection(width, height, fov, farz, nearz, uniforms.proj);

    uniforms.eye = { 0, -2.f, -5.f, 0 };

    quaternionInit(uniforms.rotation);

    renderMesh(vk, meshCmds);
}

void renderFrame(Vulkan& vk, char* debugString) {
    recordTextCommandBuffers(vk, textCmds, debugString);
    vector<vector<VkCommandBuffer>> cmdss;
    cmdss.push_back(meshCmds);
    cmdss.push_back(textCmds);
    present(vk, cmdss);
    resetTextCommandBuffers(vk, textCmds);
}
