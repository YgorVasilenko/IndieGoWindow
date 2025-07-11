#include <Renderer.h>
#include <Shader.h>

class DefaultRenderer : public IndieGo::vkI::vkRenderer {
public:
    DefaultRenderer(GLFWwindow * w, bool initRenderPass = true) : vkRenderer(w, initRenderPass) {};
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