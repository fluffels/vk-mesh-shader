#pragma warning(disable: 4267)

#include <cstdlib>
#include <ctime>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "MathLib.h"
#include "RenderMesh.h"

struct Vertex {
    Vec4 position;
};

static VulkanMesh mesh;
static vector<Vertex> vertices;

const float distMin = 1.f;
const float cellSize = distMin / sqrtf(2.f);

const uint32_t gridWidth = 10;
const uint32_t gridHeight = 10;
const uint32_t gridSize = gridWidth * gridHeight;

void samplePoissonDisk(vector<Vertex>& vertices) {

    vertices.resize(gridSize);

    srand(1);

    for (uint32_t xi = 0; xi < gridWidth; xi++) {
        for (uint32_t zi = 0; zi < gridHeight; zi++) {
            auto i = zi * gridWidth + xi;
            auto& v = vertices[i];
            auto offsetX = rand() / (float)RAND_MAX;
            auto offsetZ = rand() / (float)RAND_MAX;
            v.position.x = xi * distMin + offsetX;
            v.position.y = 0;
            v.position.z = zi * distMin + offsetZ;
            v.position.w = 0;
        }
    }

    const float depthMax = 10.f;
    const float depthMin = -depthMax;
    const float widthMax = depthMax;
    const float widthMin = -widthMax;

    vector<Vertex> processList;
}

void renderMesh(
    Vulkan& vk,
    vector<VkCommandBuffer>& cmds
) {
    VulkanPipeline pipeline;
    initVKPipeline(
        vk,
        "default",
        pipeline
    );

    samplePoissonDisk(vertices);

    updateUniformBuffer(
        vk.device,
        pipeline.descriptorSet,
        0,
        vk.uniforms.handle
    );

    VulkanBuffer vertexBuffer = {};
    uploadStorageBuffer(
        vk.device,
        vk.memories,
        vk.queueFamily,
        vertices.data(),
        vertices.size() * sizeof(Vertex),
        vertexBuffer
    );

    updateStorageBuffer(
        vk.device,
        pipeline.descriptorSet,
        1,
        vertexBuffer.handle
    );

    uint32_t framebufferCount = vk.swap.images.size();
    cmds.resize(framebufferCount);
    createCommandBuffers(vk.device, vk.cmdPool, framebufferCount, cmds);
    for (size_t swapIdx = 0; swapIdx < framebufferCount; swapIdx++) {
        auto& cmd = cmds[swapIdx];
        beginFrameCommandBuffer(cmd);

        VkClearValue colorClear;
        colorClear.color = {};
        VkClearValue depthClear;
        depthClear.depthStencil = { 1.f, 0 };
        VkClearValue clears[] = { colorClear, depthClear };

        VkRenderPassBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.clearValueCount = 2;
        beginInfo.pClearValues = clears;
        beginInfo.framebuffer = vk.swap.framebuffers[swapIdx];
        beginInfo.renderArea.extent = vk.swap.extent;
        beginInfo.renderArea.offset = {0, 0};
        beginInfo.renderPass = vk.renderPass;

        vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline.handle
        );
        vkCmdBindDescriptorSets(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipeline.layout,
            0,
            1,
            &pipeline.descriptorSet,
            0,
            nullptr
        );

        vk.cmdDrawMeshTasksNV(cmd, gridSize, 0);

        vkCmdEndRenderPass(cmd);

        checkSuccess(vkEndCommandBuffer(cmd));
    }
}
