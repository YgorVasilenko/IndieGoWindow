#include <default_pipeline.h>

void DefaultRenderer::init() {
    createRenderPass();
    recreateNoFramebufferTexImages = true;

    createSwapChainFramebuffers();

    // goes to UI for rendering
    createTextureImages(swapChainImagesCount);
    createTextureImageViews(textureImages);
    createTextureImageSamplers(swapChainImagesCount);

    createSyncObjects(swapChainImagesCount * 2, swapChainImagesCount);
    createCommandBuffers(swapChainImagesCount);
    initFramebuffers(swapChainFramebuffers);
};

void DefaultRenderer::drawCommands(VkCommandBuffer commandBuffer) {
    vkCmdDraw(commandBuffer, 6, 1, 0, 0);
};

// template<typename UBO_struct>
// void SpriteRenderer<UBO_struct>::init() {
//     // createRenderPass();

//     // // framebuffers
//     // createTextureImages(swapChainImagesCount);
//     // createTextureImageViews(textureImages);
//     // createTextureImageFramebuffers(textureImageViews);

//     // // Sampler for tilemap
//     // createTextureImageSamplers(1);

//     // createSyncObjects(swapChainImagesCount * 2, swapChainImagesCount);
//     // createCommandBuffers(swapChainImagesCount);
//     // initFramebuffers(textureFramebuffers);

//     // unifromBuffersMappedMemory.resize(swapChainImagesCount);
//     // createUnifromBuffers(unifromBuffersMappedMemory, sizeof(UBO_struct));

//     // initVertexBuffer(sizeof(SpriteVertex) * vertices.size(), vertices.data());
//     // initIndexBuffer(sizeof(uint16_t) * indices.size(), indices.data());
// };


// template<typename UBO_struct>
// void SpriteRenderer<UBO_struct>::drawCommands(VkCommandBuffer commandBuffer) {
//     VkDeviceSize offsets[] = {0};
//     vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers.data(), offsets);
//     vkCmdBindIndexBuffer(commandBuffer, indexBuffers[0], 0, VK_INDEX_TYPE_UINT16);

//     vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
// };


VkVertexInputBindingDescription ScreenQuadShader::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = UINT32_MAX;
    return bindingDescription;
};

    std::vector<VkVertexInputAttributeDescription> ScreenQuadShader::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    return attributeDescriptions;
};

void ScreenQuadShader::fillDescrInitData() {
    descriptorsInitData = {
        {
            descriptor_type::sampler,
            shader_stage::fragment,
            true,
            0
        }
    };
};


VkVertexInputBindingDescription SpriteQuadShader::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(SpriteVertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
};

std::vector<VkVertexInputAttributeDescription> SpriteQuadShader::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    attributeDescriptions.resize(2);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(SpriteVertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(SpriteVertex, texCoord);
    return attributeDescriptions;
};

void SpriteQuadShader::fillDescrInitData() {
    descriptorsInitData = {
        {
            descriptor_type::uniform,
            shader_stage::vertex,
            true,
            0
        }, {
            descriptor_type::sampler,
            shader_stage::fragment,
            false,
            0
        }
    };
};