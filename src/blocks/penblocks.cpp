#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/input.h>
#include <scratchcpp/executioncontext.h>
#include <scratchcpp/target.h>
#include <scratchcpp/ispritehandler.h>
#include <scratchcpp/istagehandler.h>
#include <scratchcpp/value.h>
#include <scratchcpp/compilerconstant.h>
#include <scratchcpp/stringptr.h>
#include <scratchcpp/string_pool.h>
#include <scratchcpp/string_functions.h>

#include "penblocks.h"
#include "penlayer.h"
#include "spritemodel.h"
#include "stagemodel.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

// Pen size range: https://github.com/scratchfoundation/scratch-vm/blob/8dbcc1fc8f8d8c4f1e40629fe8a388149d6dfd1c/src/extensions/scratch3_pen/index.js#L100-L102
static const double PEN_SIZE_MIN = 1;
static const double PEN_SIZE_MAX = 1200;

static const double COLOR_PARAM_MIN = 0;
static const double COLOR_PARAM_MAX = 100;

inline double wrapClamp(double n, double min, double max)
{
    // TODO: Move this to a separate class
    const double range = max - min /*+ 1*/;
    return n - (std::floor((n - min) / range) * range);
}

inline QColor pen_convert_from_numeric_color(long color)
{
    return QColor::fromRgba(static_cast<QRgb>(color));
}

static void pen_convert_color(const ValueData *color, QColor &dst)
{
    StringPtr *str = string_pool_new();

    if (value_isString(color)) {
        value_toStringPtr(color, str);

        if (str->size > 0 && str->data[0] == u'#') {
            if (str->size <= 7) // #RRGGBB
            {
                dst = QColor::fromString(str->data);

                if (!dst.isValid())
                    dst = Qt::black;
            } else
                dst = Qt::black;
        } else
            dst = pen_convert_from_numeric_color(value_toLong(color));
    } else
        dst = pen_convert_from_numeric_color(value_toLong(color));

    string_pool_free(str);
}

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
    engine->addCompileFunction(this, "pen_changePenColorParamBy", &compileChangePenColorParamBy);
    engine->addCompileFunction(this, "pen_setPenColorParamTo", &compileSetPenColorParamTo);
    engine->addCompileFunction(this, "pen_changePenSizeBy", &compileChangePenSizeBy);
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

CompilerValue *PenBlocks::compileSetPenColorToColor(Compiler *compiler)
{
    CompilerValue *color = compiler->addInput("COLOR");

    if (color->isConst()) {
        // Convert color constant at compile time
        const ValueData *value = &dynamic_cast<CompilerConstant *>(color)->value().data();
        QColor converted;
        pen_convert_color(value, converted);

        QColor hsv = converted.toHsv();
        CompilerValue *h = compiler->addConstValue((hsv.hue() / 360.0) * 100);
        CompilerValue *s = compiler->addConstValue(hsv.saturationF() * 100);
        CompilerValue *b = compiler->addConstValue(hsv.valueF() * 100);
        CompilerValue *transparency;

        if (converted.alpha() > 0)
            transparency = compiler->addConstValue(100 * (1 - converted.alpha() / 255.0));
        else
            transparency = compiler->addConstValue(0);

        compiler->addTargetFunctionCall(
            "pen_setPenColorToHsbColor",
            Compiler::StaticType::Void,
            { Compiler::StaticType::Number, Compiler::StaticType::Number, Compiler::StaticType::Number, Compiler::StaticType::Number },
            { h, s, b, transparency });
    } else
        compiler->addTargetFunctionCall("pen_setPenColorToColor", Compiler::StaticType::Void, { Compiler::StaticType::Unknown }, { color });

    return nullptr;
}

void PenBlocks::compileSetOrChangePenColorParam(Compiler *compiler, bool change)
{
    Input *paramInput = compiler->input("COLOR_PARAM");
    CompilerValue *param = compiler->addInput(paramInput);
    CompilerValue *value = compiler->addInput("VALUE");
    CompilerValue *changeValue = compiler->addConstValue(change);

    if (paramInput->pointsToDropdownMenu()) {
        static const std::unordered_set<std::string> options = { "color", "saturation", "brightness", "transparency" };
        std::string option = paramInput->selectedMenuItem();

        if (options.find(option) != options.cend()) {
            std::string f = "pen_set_or_change_" + option;
            compiler->addTargetFunctionCall(f, Compiler::StaticType::Void, { Compiler::StaticType::Number, Compiler::StaticType::Bool }, { value, changeValue });
        }
    } else {
        compiler->addTargetFunctionCall(
            "pen_set_or_change_color_param",
            Compiler::StaticType::Void,
            { Compiler::StaticType::String, Compiler::StaticType::Number, Compiler::StaticType::Bool },
            { param, value, changeValue });
    }
}

CompilerValue *PenBlocks::compileChangePenColorParamBy(Compiler *compiler)
{
    compileSetOrChangePenColorParam(compiler, true);
    return nullptr;
}

CompilerValue *PenBlocks::compileSetPenColorParamTo(Compiler *compiler)
{
    compileSetOrChangePenColorParam(compiler, false);
    return nullptr;
}

CompilerValue *PenBlocks::compileChangePenSizeBy(Compiler *compiler)
{
    CompilerValue *size = compiler->addInput("SIZE");
    compiler->addTargetFunctionCall("pen_changePenSizeBy", Compiler::StaticType::Void, { Compiler::StaticType::Number }, { size });
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

BLOCK_EXPORT void pen_setPenColorToHsbColor(Target *target, double h, double s, double b, double transparency)
{
    TargetModel *model = getTargetModel(target);

    PenState &penState = model->penState();
    penState.color = h;
    penState.saturation = s;
    penState.brightness = b;
    penState.transparency = transparency;

    penState.updateColor();

    // Set the legacy "shade" value the same way Scratch 2 did.
    penState.shade = penState.brightness / 2;
}

BLOCK_EXPORT void pen_setPenColorToColor(Target *target, const ValueData *color)
{
    QColor converted;
    pen_convert_color(color, converted);

    QColor hsv = converted.toHsv();
    double h = (hsv.hue() / 360.0) * 100;
    double s = hsv.saturationF() * 100;
    double b = hsv.valueF() * 100;
    double transparency;

    if (converted.alpha() > 0)
        transparency = 100 * (1 - converted.alpha() / 255.0);
    else
        transparency = 0;

    pen_setPenColorToHsbColor(target, h, s, b, transparency);
}

BLOCK_EXPORT void pen_set_or_change_color(Target *target, double value, bool change)
{
    PenState &penState = getTargetModel(target)->penState();
    penState.color = wrapClamp(value + (change ? penState.color : 0), 0, 100);
    penState.updateColor();
}

BLOCK_EXPORT void pen_set_or_change_saturation(Target *target, double value, bool change)
{
    PenState &penState = getTargetModel(target)->penState();
    penState.saturation = std::clamp(value + (change ? penState.saturation : 0), COLOR_PARAM_MIN, COLOR_PARAM_MAX);
    penState.updateColor();
}

BLOCK_EXPORT void pen_set_or_change_brightness(Target *target, double value, bool change)
{
    PenState &penState = getTargetModel(target)->penState();
    penState.brightness = std::clamp(value + (change ? penState.brightness : 0), COLOR_PARAM_MIN, COLOR_PARAM_MAX);
    penState.updateColor();
}

BLOCK_EXPORT void pen_set_or_change_transparency(Target *target, double value, bool change)
{
    PenState &penState = getTargetModel(target)->penState();
    penState.transparency = std::clamp(value + (change ? penState.transparency : 0), COLOR_PARAM_MIN, COLOR_PARAM_MAX);
    penState.updateColor();
}

BLOCK_EXPORT void pen_set_or_change_color_param(Target *target, const StringPtr *param, double value, bool change)
{
    static const StringPtr COLOR_PARAM("color");
    static const StringPtr SATURATION_PARAM("saturation");
    static const StringPtr BRIGHTNESS_PARAM("brightness");
    static const StringPtr TRANSPARENCY_PARAM("transparency");

    if (string_compare_case_sensitive(param, &COLOR_PARAM) == 0)
        pen_set_or_change_color(target, value, change);
    else if (string_compare_case_sensitive(param, &SATURATION_PARAM) == 0)
        pen_set_or_change_saturation(target, value, change);
    else if (string_compare_case_sensitive(param, &BRIGHTNESS_PARAM) == 0)
        pen_set_or_change_brightness(target, value, change);
    else if (string_compare_case_sensitive(param, &TRANSPARENCY_PARAM) == 0)
        pen_set_or_change_transparency(target, value, change);
}

BLOCK_EXPORT void pen_changePenSizeBy(Target *target, double value)
{
    PenAttributes &penAttributes = getTargetModel(target)->penAttributes();
    penAttributes.diameter = std::clamp(penAttributes.diameter + value, PEN_SIZE_MIN, PEN_SIZE_MAX);
}
