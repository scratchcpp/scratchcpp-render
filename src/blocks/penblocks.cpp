#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/target.h>
#include <scratchcpp/ispritehandler.h>
#include <scratchcpp/istagehandler.h>

#include "penblocks.h"
#include "penlayer.h"
#include "spritemodel.h"
#include "stagemodel.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

std::string PenBlocks::name() const
{
    return "Pen";
}

std::string PenBlocks::description() const
{
    return name() + " blocks";
}

Rgb PenBlocks::color() const
{
    return rgb(15, 189, 140);
}

void PenBlocks::registerBlocks(IEngine *engine)
{
    engine->addCompileFunction(this, "pen_clear", &compileClear);
    engine->addCompileFunction(this, "pen_stamp", &compileStamp);
}

CompilerValue *PenBlocks::compileClear(Compiler *compiler)
{
    compiler->addFunctionCallWithCtx("pen_clear");
    return nullptr;
}

CompilerValue *PenBlocks::compileStamp(Compiler *compiler)
{
    compiler->addTargetFunctionCall("pen_stamp");
    return nullptr;
}

BLOCK_EXPORT void pen_clear(ExecutionContext *ctx)
{
    IEngine *engine = ctx->engine();
    IPenLayer *penLayer = PenLayer::getProjectPenLayer(engine);

    if (penLayer) {
        penLayer->clear();
        engine->requestRedraw();
    }
}

BLOCK_EXPORT void pen_stamp(Target *target)
{
    IEngine *engine = target->engine();
    IPenLayer *penLayer = PenLayer::getProjectPenLayer(engine);

    if (penLayer) {
        IRenderedTarget *renderedTarget = nullptr;

        if (target->isStage()) {
            IStageHandler *iface = static_cast<Stage *>(target)->getInterface();
            renderedTarget = static_cast<StageModel *>(iface)->renderedTarget();
        } else {
            ISpriteHandler *iface = static_cast<Sprite *>(target)->getInterface();
            renderedTarget = static_cast<SpriteModel *>(iface)->renderedTarget();
        }

        penLayer->stamp(renderedTarget);
        engine->requestRedraw();
    }
}
