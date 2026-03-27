#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/target.h>
#include <scratchcpp/ispritehandler.h>
#include <scratchcpp/istagehandler.h>
#include <scratchcpp/value.h>
#include <scratchcpp/compilerconstant.h>

#include "penblocks.h"
#include "penlayer.h"
#include "spritemodel.h"
#include "stagemodel.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

std::string PenBlocks::name() const
{
    return "pen";
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
    engine->addCompileFunction(this, "pen_penDown", &compilePenDown);
    engine->addCompileFunction(this, "pen_penUp", &compilePenUp);
    engine->addCompileFunction(this, "pen_setPenColorToColor", &compileSetPenColorToColor);
}

CompilerValue *PenBlocks::compileSetPenColorToColor(Compiler *compiler)
{
    CompilerValue *color = compiler->addInput("COLOR");
    compiler->addTargetFunctionCall("pen_setPenColorToColor", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { color });
    return nullptr;
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

CompilerValue *PenBlocks::compilePenDown(Compiler *compiler)
{
    CompilerValue *arg = compiler->addConstValue(true);
    compiler->addTargetFunctionCall("pen_set_pen_down", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { arg });
    return nullptr;
}

CompilerValue *PenBlocks::compilePenUp(Compiler *compiler)
{
    CompilerValue *arg = compiler->addConstValue(false);
    compiler->addTargetFunctionCall("pen_set_pen_down", Compiler::StaticType::Void, { Compiler::StaticType::Bool }, { arg });
    return nullptr;
}

static TargetModel *getTargetModel(Target *target)
{
    if (target->isStage()) {
        Stage *stage = static_cast<Stage *>(target);
        return static_cast<StageModel *>(stage->getInterface());
    } else {
        Sprite *sprite = static_cast<Sprite *>(target);
        return static_cast<SpriteModel *>(sprite->getInterface());
    }
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

BLOCK_EXPORT void pen_set_pen_down(Target *target, bool down)
{
    getTargetModel(target)->setPenDown(down);
}

BLOCK_EXPORT void pen_setPenColorToColor(Target *target, const ValueData *color)
{
    TargetModel *model = getTargetModel(target);

    std::string stringValue;
    PenState &penState = model->penState();
    QColor newColor;

    if (value_isString(color))
        value_toString(color, &stringValue);

    if (!stringValue.empty() && stringValue[0] == '#') {
        bool valid = false;

        if (stringValue.size() <= 7) // #RRGGBB
        {
            newColor = QColor::fromString(stringValue);
            valid = newColor.isValid();
        }

        if (!valid)
            newColor = Qt::black;

    } else
        newColor = QColor::fromRgba(static_cast<QRgb>(value_toLong(color)));

    QColor hsv = newColor.toHsv();
    penState.color = (hsv.hue() / 360.0) * 100;
    penState.saturation = hsv.saturationF() * 100;
    penState.brightness = hsv.valueF() * 100;

    if (newColor.alpha() > 0)
        penState.transparency = 100 * (1 - newColor.alpha() / 255.0);
    else
        penState.transparency = 0;

    penState.updateColor();

    // Set the legacy "shade" value the same way Scratch 2 did.
    penState.shade = penState.brightness / 2;
}
