#pragma once

#include <unordered_map>
#include "framebuffers/Framebuffer.h"
#include "Renderer.h"
#include "WaveBarRenderer.h"
#include "RingRenderer.h"
#include "SphereMeshRenderer.h"

#define FRAME_WIDTH  1920
#define FRAME_HEIGHT 1080

namespace Lengine {

    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;
        virtual void Execute(RenderContext& ctx) = 0;
    };

    class RenderGraph
    {
    public:
        void AddPass(std::unique_ptr<RenderPass> pass) { passes.push_back(std::move(pass)); }
        void Clear()  { passes.clear(); }
        void Execute(RenderContext& ctx) { for (auto& p : passes) p->Execute(ctx); }
    private:
        std::vector<std::unique_ptr<RenderPass>> passes;
    };

    class SPHEREICAL_WAVES : public RenderPass
    {
    public:
        SPHEREICAL_WAVES(Framebuffer& target, ResourceManager& rm)
            : target(target), resourceManager(rm), renderer(rm)
        { renderer.Init(); }
        void Execute(RenderContext& ctx) override
        {
            target.Bind();
            glViewport(0, 0, FRAME_WIDTH, FRAME_HEIGHT);
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ctx.frameWidth  = target.GetWidth();
            ctx.frameHeight = target.GetHeight();
            renderer.Render(ctx);
            target.Unbind();
        }
    private:
        Framebuffer& target;
        ResourceManager& resourceManager;
        SpheremeshRenderer renderer;
    };

    class HOLOGRAM_WAVES : public RenderPass
    {
    public:
        HOLOGRAM_WAVES(Framebuffer& target, ResourceManager& rm)
            : target(target), resourceManager(rm), renderer(rm)
        { renderer.Init(); }
        void Execute(RenderContext& ctx) override
        {
            target.Bind();
            glViewport(0, 0, FRAME_WIDTH, FRAME_HEIGHT);
            glClearColor(0.01f, 0.01f, 0.04f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ctx.frameWidth  = target.GetWidth();
            ctx.frameHeight = target.GetHeight();
            renderer.Render(ctx);
            target.Unbind();
        }
    private:
        Framebuffer& target;
        ResourceManager& resourceManager;
        WaveBarRenderer renderer;
    };

    // ── RING_WAVES: concentric rotating arcs, heat-mapped green→red ──────────
    class RING_WAVES : public RenderPass
    {
    public:
        RING_WAVES(Framebuffer& target, ResourceManager& rm)
            : target(target), resourceManager(rm), renderer(rm)
        { renderer.Init(); }
        void Execute(RenderContext& ctx) override
        {
            target.Bind();
            glViewport(0, 0, FRAME_WIDTH, FRAME_HEIGHT);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // pure black for additive glow
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ctx.frameWidth  = target.GetWidth();
            ctx.frameHeight = target.GetHeight();
            renderer.Render(ctx);
            target.Unbind();
        }
    private:
        Framebuffer& target;
        ResourceManager& resourceManager;
        RingRenderer renderer;
    };

    class ResolvePass : public RenderPass
    {
    public:
        ResolvePass(Framebuffer& src, Framebuffer& dst) : source(src), destination(dst) {}
        void Execute(RenderContext& /*ctx*/) override
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, source.GetID());
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destination.GetID());
            glBlitFramebuffer(
                0, 0, source.GetWidth(),      source.GetHeight(),
                0, 0, destination.GetWidth(), destination.GetHeight(),
                GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST
            );
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    private:
        Framebuffer& source;
        Framebuffer& destination;
    };

    class RenderPipeline
    {
    public:
        explicit RenderPipeline(ResourceManager& rm) : resourceManager(rm)
        {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_MULTISAMPLE);
            glEnable(GL_DEBUG_OUTPUT);
        }

        void Init(const int id);
        void Render(
            const int id,
            const float bass, const float mid, const float treble,
            const std::vector<float>& smoothedSpectrum,
            RenderMode mode, float time
        );

        uint32_t GetFinalImage(int id)
        { return resolveFramebuffers[id]->GetColorAttachment(0); }

        const std::unordered_map<int, RenderContext>& GetRenderContexts() const { return renderContexts; }

    private:
        std::unordered_map<int, std::unique_ptr<Framebuffer>> msaaFramebuffers;
        std::unordered_map<int, std::unique_ptr<Framebuffer>> resolveFramebuffers;
        std::unordered_map<int, RenderGraph>   renderGraphs;
        std::unordered_map<int, RenderContext> renderContexts;
        std::unordered_map<int, bool>          initializedPipelines;

        void CreateFrameBuffer(const int id);
        void BuildGraph(const int id);

        ResourceManager& resourceManager;
    };

} // namespace Lengine
