#include "visualizer/RenderPipeline.h"

using namespace Lengine;

void RenderPipeline::Init(const int id)
{
    CreateFrameBuffer(id);
    BuildGraph(id);
    initializedPipelines[id] = true;
}

void RenderPipeline::CreateFrameBuffer(const int id)
{
    msaaFramebuffers[id].reset();
    resolveFramebuffers[id].reset();

    FramebufferSpecification msaaSpec;
    msaaSpec.width = FRAME_WIDTH; msaaSpec.height = FRAME_HEIGHT; msaaSpec.samples = 4;

    FramebufferSpecification resolveSpec;
    resolveSpec.width = FRAME_WIDTH; resolveSpec.height = FRAME_HEIGHT; resolveSpec.samples = 1;

    msaaFramebuffers[id]    = std::make_unique<Framebuffer>(msaaSpec);
    resolveFramebuffers[id] = std::make_unique<Framebuffer>(resolveSpec);
}

void RenderPipeline::BuildGraph(const int id)
{
    renderGraphs[id].Clear();

    Framebuffer& src  = *msaaFramebuffers[id];
    Framebuffer& dest = *resolveFramebuffers[id];

    switch (renderContexts[id].mode)
    {
        case RenderMode::SPHERICAL_WAVES:
            renderGraphs[id].AddPass(std::make_unique<SPHEREICAL_WAVES>(src, resourceManager));
            break;

        case RenderMode::HOLOGRAM_WAVES:
            renderGraphs[id].AddPass(std::make_unique<HOLOGRAM_WAVES>(src, resourceManager));
            break;

        case RenderMode::RING_WAVES:                         // ← NEW
            renderGraphs[id].AddPass(std::make_unique<RING_WAVES>(src, resourceManager));
            break;

        default:
            renderGraphs[id].AddPass(std::make_unique<SPHEREICAL_WAVES>(src, resourceManager));
            break;
    }

    renderGraphs[id].AddPass(std::make_unique<ResolvePass>(src, dest));
}

void RenderPipeline::Render(
    const int id,
    const float bass, const float mid, const float treble,
    const std::vector<float>& smoothedSpectrum,
    RenderMode mode, const float time)
{
    bool needRebuild = !initializedPipelines[id] || renderContexts[id].mode != mode;

    renderContexts[id].bass             = bass;
    renderContexts[id].mid              = mid;
    renderContexts[id].treble           = treble;
    renderContexts[id].smoothedSpectrum = smoothedSpectrum;
    renderContexts[id].mode             = mode;
    renderContexts[id].time             = time;

    if (needRebuild) Init(id);

    renderGraphs[id].Execute(renderContexts[id]);
}
