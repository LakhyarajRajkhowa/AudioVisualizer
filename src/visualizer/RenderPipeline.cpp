#include "visualizer/RenderPipeline.h"

using namespace Lengine;

void RenderPipeline::Init(const int id){

    CreateFrameBuffer(id);
    BuildGraph(id);

    initializedPipelines[id] = true;
}

void RenderPipeline::CreateFrameBuffer(const int id) {
    frameBuffers[id].reset();

    FramebufferSpecification mainBufferSpec;
    mainBufferSpec.width = FRAME_WIDTH;
    mainBufferSpec.height = FRAME_HEIGHT;
    mainBufferSpec.samples = 1;
    mainBufferSpec.colorFormats = { GL_RGBA8 };
    mainBufferSpec.colorAttachmentCount = 1;

    frameBuffers[id] = std::make_unique<Framebuffer>(mainBufferSpec);

}

void RenderPipeline::BuildGraph(const int id) {
    renderGraphs[id].Clear();

    if (renderContexts[id].mode == VisualMode::MODE_1) {

        Framebuffer& target =
            *frameBuffers.at(id);

        renderGraphs[id].AddPass(
            std::make_unique<Mode_1_Pass>(
                target
            )
        );
    }
}

void RenderPipeline::Render(
    const int id,
    const float bass,
    const float mid,
    const float treble,
    const std::vector<float>& smoothedSpectrum,
    VisualMode mode
) 
{
    if (!initializedPipelines[id]) {
        Init(id);
    }
    renderContexts[id].bass = bass;
    renderContexts[id].mid = mid;
    renderContexts[id].treble = treble;
    renderContexts[id].smoothedSpectrum = smoothedSpectrum;
    renderContexts[id].mode = mode;

    renderGraphs[id].Execute(renderContexts[id]);
}