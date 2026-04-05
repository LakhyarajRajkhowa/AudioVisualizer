#include "visualizer/RenderPipeline.h"

using namespace Lengine;

void RenderPipeline::Init(){
    // Depth
    glEnable(GL_DEPTH_TEST);

    // Face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // MSAA
    glEnable(GL_MULTISAMPLE);

    CreateFrameBuffers();
    BuildGraph();
}

void RenderPipeline::CreateFrameBuffers() {
    mainFramebuffer.reset();

    FramebufferSpecification mainBufferSpec;
    mainBufferSpec.width = FRAME_WIDTH;
    mainBufferSpec.height = FRAME_HEIGHT;
    mainBufferSpec.samples = 1;
    mainBufferSpec.colorFormats = { GL_RGBA8 };
    mainBufferSpec.colorAttachmentCount = 1;

    mainFramebuffer = std::make_unique<Framebuffer>(mainBufferSpec);

}

void RenderPipeline::BuildGraph() {
    renderGraph.Clear();

    if (ctx.mode == VisualMode::MODE_1) {

        Framebuffer& target =
            *mainFramebuffer;

        renderGraph.AddPass(
            std::make_unique<Mode_1_Pass>(
                target
            )
        );
    }
}

void RenderPipeline::Render(
    const float bass,
    const float mid,
    const float treble,
    const std::vector<float>& smoothedSpectrum,
    VisualMode mode
) 
{
    ctx.bass = bass;
    ctx.mid = mid;
    ctx.treble = treble;
    ctx.smoothedSpectrum = smoothedSpectrum;

    ctx.mode = mode;

    renderGraph.Execute(ctx);
}