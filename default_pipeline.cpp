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