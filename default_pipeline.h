#include <Renderer.h>
#include <Shader.h>
#include <glm/glm.hpp>

class DefaultRenderer : public IndieGo::vkI::vkRenderer {
public:
    DefaultRenderer(GLFWwindow * w, bool initRenderPass = true) : vkRenderer(w, initRenderPass) {};
    void init() override;
    void drawCommands(VkCommandBuffer commandBuffer) override;
};


struct SpriteVertex {
    glm::vec3 pos;
    glm::vec2 texCoord;
};

template<typename UBO_struct>
class SpriteRenderer : public IndieGo::vkI::vkRenderer {
public:
    UBO_struct ubo {};
    std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
    };
    std::vector<SpriteVertex> vertices = {
        {{-1.f, -1.f, 0.0f},  {1.0f, 0.0f}},
        {{1.f, -1.f, 0.0f}, {0.0f, 0.0f}},
        {{1.f, 1.f, 0.0f}, {0.0f, 1.0f}},
        {{-1.f, 1.f, 0.0f}, {1.0f, 1.0f}}
    };
    std::vector<void*> unifromBuffersMappedMemory = {};
    SpriteRenderer(GLFWwindow * w, bool initRenderPass = true) : vkRenderer(w, initRenderPass) {};
    void init() override;
    void drawCommands(VkCommandBuffer commandBuffer) override;
};

// Contains memory for UI
class ScreenQuadShader : public Shader {
public:
    ScreenQuadShader( 
        VkDevice vkd, 
        int sImgsCnt,
        VkRenderPass rp,
        std::vector<VkBuffer> ubos = {},
        std::vector<VkImageView> * tivs = nullptr,
        std::vector<VkSampler> ts = {}
    ) : Shader(vkd, sImgsCnt, rp, ubos, tivs, ts) {
        fillDescrInitData();
    };

    VkVertexInputBindingDescription getBindingDescription() override;
    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() override;
    void fillDescrInitData();
};

class SpriteQuadShader : public Shader {
public:
    SpriteQuadShader( 
        VkDevice vkd, 
        int sImgsCnt,
        VkRenderPass rp,
        std::vector<VkBuffer> ubos = {},
        std::vector<VkImageView> * tivs = nullptr,
        std::vector<VkSampler> ts = {}
    ) : Shader(vkd, sImgsCnt, rp, ubos, tivs, ts) {
        fillDescrInitData();
    };

    VkVertexInputBindingDescription getBindingDescription() override;
    std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() override;
    void fillDescrInitData();
};