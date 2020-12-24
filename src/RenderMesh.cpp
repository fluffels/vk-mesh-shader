#pragma warning(disable: 4267)

#include <cstdlib>
#include <ctime>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "MathLib.h"
#include "RenderMesh.h"

#pragma pack (push, 1)
struct Vertex {
    Vec4 position;
    Vec4 rotation;
};
#pragma pack (pop)

static VulkanMesh mesh;
static vector<Vertex> vertices;

const float distMin = 1.f;
const float cellSize = distMin / sqrtf(2.f);

const uint32_t gridWidth = 100;
const uint32_t gridHeight = gridWidth;
const uint32_t gridSize = gridWidth * gridHeight;

void generateVertices(vector<Vertex>& vertices) {
    vertices.resize(gridSize);

    srand(1);

    for (uint32_t xi = 0; xi < gridWidth; xi++) {
        for (uint32_t zi = 0; zi < gridHeight; zi++) {
            auto i = zi * gridWidth + xi;
            auto& v = vertices[i];
            auto offsetX = (rand() / (float)RAND_MAX) * cellSize;
            auto offsetZ = (rand() / (float)RAND_MAX) * cellSize;
            v.position.x = xi * cellSize + offsetX;
            v.position.y = -((rand() / (float)RAND_MAX) + .5f);
            v.position.z = zi * cellSize + offsetZ;
            v.rotation.y = (rand() / (float)RAND_MAX) * 2 * 3.14f;
            v.rotation.z = (rand() / (float)RAND_MAX) * 0.5f * 3.14f - (0.25f * 3.14f);
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
    initVKPipelineNoCull(
        vk,
        "default",
        pipeline
    );

    int x, y, n;
    uint8_t* data = stbi_load(
        "data/grass.png",
        &x, &y, &n, 4
    );
    uint32_t size = x * y * 4;
    VulkanSampler sampler = {};
    uploadTexture(
        vk.device,
        vk.memories,
        vk.queue,
        vk.queueFamily,
        vk.cmdPoolTransient,
        x, y, data, size,
        sampler
    );
    updateCombinedImageSampler(
        vk.device,
        pipeline.descriptorSet,
        2,
        &sampler,
        1
    );
    stbi_image_free(data);

    generateVertices(vertices);

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
