#include "visualizer/RenderPipeline.h"

using namespace Lengine;

void RenderPipeline::Init(const int id){

    CreateFrameBuffer(id);
    BuildGraph(id);

    initializedPipelines[id] = true;
}

void RenderPipeline::Destroy(const int id) {
    msaaFramebuffers[id].reset();
    resolveFramebuffers[id].reset();

    renderGraphs[id].Clear();
   
    initializedPipelines[id] = false;
}

void RenderPipeline::CreateFrameBuffer(const int id) {
    msaaFramebuffers[id].reset();
    resolveFramebuffers[id].reset();

    FramebufferSpecification msaaSpec;
    msaaSpec.width = FRAME_WIDTH;
    msaaSpec.height = FRAME_HEIGHT;
    msaaSpec.samples = 4;

    FramebufferSpecification resolveSpec;
    resolveSpec.width = FRAME_WIDTH;
    resolveSpec.height = FRAME_HEIGHT;
    resolveSpec.samples = 1;

    msaaFramebuffers[id] = std::make_unique<Framebuffer>(msaaSpec);
    resolveFramebuffers[id] = std::make_unique<Framebuffer>(resolveSpec);

}

void RenderPipeline::BuildGraph(const int id) {
    renderGraphs[id].Clear();

    Framebuffer& src =
        *msaaFramebuffers[id];

    Framebuffer& dest =
        *resolveFramebuffers[id];

    if (renderContexts[id].mode == RenderMode::SPHERICAL_WAVES) {

        renderGraphs[id].AddPass(
            std::make_unique<SPHEREICAL_WAVES>(
                src,
                resourceManager
            )
        );
    }

    
    renderGraphs[id].AddPass(
        std::make_unique<ResolvePass>(
            src,
            dest
        )
    );
}

void RenderPipeline::Render(
    const int id,
    const float bass,
    const float mid,
    const float treble,
    const std::vector<float>& smoothedSpectrum,
    RenderMode mode,
    const float time
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
    renderContexts[id].time = time;

    renderGraphs[id].Execute(renderContexts[id]);
}