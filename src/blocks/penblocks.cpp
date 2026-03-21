#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/executioncontext.h>

#include "penblocks.h"
#include "penlayer.h"

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
}

CompilerValue *PenBlocks::compileClear(Compiler *compiler)
{
    compiler->addFunctionCallWithCtx("pen_clear");
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
