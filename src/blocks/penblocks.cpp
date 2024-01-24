// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>

#include "penblocks.h"
#include "penlayer.h"
#include "spritemodel.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

// Pen size range: https://github.com/scratchfoundation/scratch-vm/blob/8dbcc1fc8f8d8c4f1e40629fe8a388149d6dfd1c/src/extensions/scratch3_pen/index.js#L100-L102
static const double PEN_SIZE_MIN = 1;
static const double PEN_SIZE_MAX = 1200;

std::string PenBlocks::name() const
{
    return "Pen";
}

void PenBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "pen_clear", &compileClear);
    engine->addCompileFunction(this, "pen_penDown", &compilePenDown);
    engine->addCompileFunction(this, "pen_penUp", &compilePenUp);
    engine->addCompileFunction(this, "pen_setPenColorToColor", &compileSetPenColorToColor);
    engine->addCompileFunction(this, "pen_changePenSizeBy", &compileChangePenSizeBy);
    engine->addCompileFunction(this, "pen_setPenSizeTo", &compileSetPenSizeTo);
    engine->addCompileFunction(this, "pen_changePenHueBy", &compileChangePenHueBy);

    // Inputs
    engine->addInput(this, "COLOR", COLOR);
    engine->addInput(this, "SIZE", SIZE);
    engine->addInput(this, "HUE", HUE);
}

void PenBlocks::compileClear(Compiler *compiler)
{
    compiler->addFunctionCall(&clear);
}

void PenBlocks::compilePenDown(Compiler *compiler)
{
    compiler->addFunctionCall(&penDown);
}

void PenBlocks::compilePenUp(Compiler *compiler)
{
    compiler->addFunctionCall(&penUp);
}

void PenBlocks::compileSetPenColorToColor(libscratchcpp::Compiler *compiler)
{
    compiler->addInput(COLOR);
    compiler->addFunctionCall(&setPenColorToColor);
}

void PenBlocks::compileChangePenSizeBy(libscratchcpp::Compiler *compiler)
{
    compiler->addInput(SIZE);
    compiler->addFunctionCall(&changePenSizeBy);
}

void PenBlocks::compileSetPenSizeTo(libscratchcpp::Compiler *compiler)
{
    compiler->addInput(SIZE);
    compiler->addFunctionCall(&setPenSizeTo);
}

void PenBlocks::compileChangePenHueBy(libscratchcpp::Compiler *compiler)
{
    compiler->addInput(HUE);
    compiler->addFunctionCall(&changePenHueBy);
}

unsigned int PenBlocks::clear(VirtualMachine *vm)
{
    IPenLayer *penLayer = PenLayer::getProjectPenLayer(vm->engine());

    if (penLayer) {
        penLayer->clear();
        vm->engine()->requestRedraw();
    }

    return 0;
}

unsigned int PenBlocks::penDown(VirtualMachine *vm)
{
    SpriteModel *model = getSpriteModel(vm);

    if (model)
        model->setPenDown(true);

    return 0;
}

unsigned int PenBlocks::penUp(libscratchcpp::VirtualMachine *vm)
{
    SpriteModel *model = getSpriteModel(vm);

    if (model)
        model->setPenDown(false);

    return 0;
}

unsigned int PenBlocks::changePenSizeBy(libscratchcpp::VirtualMachine *vm)
{
    SpriteModel *model = getSpriteModel(vm);

    if (model)
        model->penAttributes().diameter = std::clamp(model->penAttributes().diameter + vm->getInput(0, 1)->toDouble(), PEN_SIZE_MIN, PEN_SIZE_MAX);

    return 1;
}

unsigned int PenBlocks::setPenSizeTo(libscratchcpp::VirtualMachine *vm)
{
    SpriteModel *model = getSpriteModel(vm);

    if (model)
        model->penAttributes().diameter = std::clamp(vm->getInput(0, 1)->toDouble(), PEN_SIZE_MIN, PEN_SIZE_MAX);

    return 1;
}

unsigned int PenBlocks::changePenHueBy(libscratchcpp::VirtualMachine *vm)
{
    SpriteModel *model = getSpriteModel(vm);

    if (model) {
        const double colorChange = vm->getInput(0, 1)->toDouble() / 2;
        setOrChangeColorParam(model->penAttributes(), ColorParam::COLOR, colorChange, true);
    }

    return 1;
}

unsigned int PenBlocks::setPenColorToColor(libscratchcpp::VirtualMachine *vm)
{
    SpriteModel *model = getSpriteModel(vm);

    if (model) {
        const Value *value = vm->getInput(0, 1);
        std::string stringValue;
        PenAttributes &attributes = model->penAttributes();

        if (value->isString())
            stringValue = value->toString();

        if (!stringValue.empty() && stringValue[0] == '#') {
            bool valid = false;

            if (stringValue.size() <= 7) // #RRGGBB
            {
                attributes.color = QColor::fromString(stringValue);
                valid = attributes.color.isValid();
            }

            if (!valid)
                attributes.color = QColor(0, 0, 0);

        } else {
            attributes.color = QColor::fromRgba(static_cast<QRgb>(value->toLong()));

            if (attributes.color.alpha() == 0)
                attributes.color.setAlpha(255);
        }
    }

    return 1;
}

SpriteModel *PenBlocks::getSpriteModel(libscratchcpp::VirtualMachine *vm)
{
    Target *target = vm->target();

    if (!target || target->isStage())
        return nullptr;

    Sprite *sprite = static_cast<Sprite *>(target);
    SpriteModel *model = static_cast<SpriteModel *>(sprite->getInterface());
    return model;
}

void PenBlocks::setOrChangeColorParam(PenAttributes &penAttributes, ColorParam param, double value, bool change)
{
    PenState penState(penAttributes.color);

    switch (param) {
        case ColorParam::COLOR:
            penState.color = wrapClamp(value + (change ? penState.color : 0), 0, 100);
            break;
    }

    const int h = std::round(std::fmod(penState.color * 360 / 100, 360.0));
    const int s = std::round(penState.saturation * 2.55);
    const int v = std::round(penState.brightness * 2.55);
    const int a = std::round((100 - penState.transparency) * 2.55);
    penAttributes.color = QColor::fromHsv(h, s, v, a);
}

double PenBlocks::wrapClamp(double n, double min, double max)
{
    // TODO: Move this to a separate class
    const double range = max - min + 1;
    return n - (std::floor((n - min) / range) * range);
}
