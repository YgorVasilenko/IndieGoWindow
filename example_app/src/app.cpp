#include <IndieGoWindow.h>
#include <IndieGoUI.h>
#include <default_pipeline.h>
#include <memory>

using namespace IndieGo;
using namespace IndieGo::vkI;
using namespace IndieGo::UI;
using namespace std;

#ifndef WIDTH 
#define WIDTH 1920
#endif

#ifndef HEIGHT 
#define HEIGHT 1080
#endif

class WindowExampleApp : public App {
    shared_ptr<DefaultRenderer> renderer;
    unique_ptr<ScreenQuadShader> shader;
    public:
        void initRenderingPipelines() override {
            renderer = make_shared<DefaultRenderer>(appWindow.window, false);
            renderer->init();
            shader = make_unique<ScreenQuadShader>(
                vkRenderer::device,
                vkRenderer::swapChainImagesCount,
                renderer->renderPass, 
                vector<VkBuffer>{},
                &renderer->textureImageViews,
                renderer->textureSamplers
            );
            string shader_path = home_dir.append("..").append("..").append("screen_quad").string();
            shader->load(
                (shader_path + "/vert.spv").c_str(), 
                (shader_path + "/frag.spv").c_str()
            );
            uiCanvHolder = renderer;
            renderer->tex_user_shader = shader.get();
        };

        void processFrame() override {
            appWindow.printOnScreen("FPS: " + to_string(appWindow.fps));
        };

        void drawAppData() override {
            // rendering of final image
            VkCommandBuffer frameCB = renderer->commandBuffers[vkRenderer::currFrame];
            renderer->beginRecordCommandBuffer(frameCB);
            renderer->beginRenderPass(frameCB, shader.get());
            renderer->drawCommands(frameCB);
            renderer->endRecordCommandBuffer(frameCB);
            renderer->submitQueue(frameCB, true);
        };
};

int main() {
    WindowExampleApp app;
    app.init();
    app.run();
    return 0;
}