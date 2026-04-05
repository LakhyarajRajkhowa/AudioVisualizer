#pragma once

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
            Init();
        }
        void Init();
        void Render(
            const float bass,
            const float mid,
            const float treble,
            const std::vector<float>& smoothedSpectrum,
            VisualMode mode
        );


        uint32_t GetFinalImage() const
        {
            return mainFramebuffer->GetColorAttachment(0);
        }
    private:
        std::unique_ptr<Framebuffer> mainFramebuffer;
        RenderGraph renderGraph;
        RenderContext ctx;

        void CreateFrameBuffers();
        void BuildGraph();
    };
}