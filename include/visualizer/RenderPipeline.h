#pragma once

#include <unordered_map>
#include "framebuffers/Framebuffer.h"

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080

namespace Lengine {

    enum class VisualMode{
        MODE_1 = 1
    };

	struct RenderContext {

        float bass;
        float mid;
        float treble;

        std::vector<float> smoothedSpectrum;

        VisualMode mode = VisualMode::MODE_1;
        
	};

    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;
        virtual void Execute(RenderContext& ctx) = 0;
    };

    class RenderGraph
    {
    public:
        RenderGraph() = default;
        ~RenderGraph() = default;

        void AddPass(std::unique_ptr<RenderPass> pass)
        {
            passes.push_back(std::move(pass));
        }

        void Clear()
        {
            passes.clear();
        }

        void Execute(RenderContext& context)
        {
            for (auto& pass : passes)
            {
                pass->Execute(context);
            }
        }

    private:
        std::vector<std::unique_ptr<RenderPass>> passes;
    };

    class Mode_1_Pass : public RenderPass
    {
    public:
        Mode_1_Pass(
            Framebuffer& target)
            :  target(target) {}

        void Execute(RenderContext& ctx) override
        {
            target.Bind();
            glClearColor(1, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // here goes actual rendering

            target.Unbind();
        }

    private:
        Framebuffer& target;
    };

    class RenderPipeline
    {
    public:
        RenderPipeline() {
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
        }
        void Init(const int id);

        void Render(
            const int id,
            const float bass,
            const float mid,
            const float treble,
            const std::vector<float>& smoothedSpectrum,
            VisualMode mode
        );


        uint32_t GetFinalImage(const int id) const
        {
            return frameBuffers.at(id)->GetColorAttachment(0);
        }
    private:
        std::unordered_map<int, std::unique_ptr<Framebuffer>> frameBuffers;
        std::unordered_map<int, RenderGraph> renderGraphs;
        std::unordered_map<int, RenderContext> renderContexts;

        std::unordered_map<int, bool> initializedPipelines;

        void CreateFrameBuffer(const int id);
        void BuildGraph(const int id);
    };
}