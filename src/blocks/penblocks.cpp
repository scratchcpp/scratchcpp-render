// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>

#include "penblocks.h"
#include "penlayer.h"
#include "penstate.h"
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
        setOrChangeColorParam(ColorParam::COLOR, colorChange, model->penState(), true, true);
    }

    return 1;
}

unsigned int PenBlocks::setPenColorToColor(libscratchcpp::VirtualMachine *vm)
{
    SpriteModel *model = getSpriteModel(vm);

    if (model) {
        const Value *value = vm->getInput(0, 1);
        std::string stringValue;
        PenState &penState = model->penState();
        QColor newColor;

        if (value->isString())
            stringValue = value->toString();

        if (!stringValue.empty() && stringValue[0] == '#') {
            bool valid = false;

            if (stringValue.size() <= 7) // #RRGGBB
            {
                newColor = QColor::fromString(stringValue);
                valid = newColor.isValid();
            }

            if (!valid)
                newColor = Qt::black;

        } else {
            newColor = QColor::fromRgba(static_cast<QRgb>(value->toLong()));

            if (newColor.alpha() == 0)
                newColor.setAlpha(255);
        }

        penState.setColor(newColor);

        // Set the legacy "shade" value the same way Scratch 2 did.
        penState.shade = penState.brightness / 2;
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

void PenBlocks::setOrChangeColorParam(ColorParam param, double value, PenState &penState, bool change, bool legacy)
{
    switch (param) {
        case ColorParam::COLOR:
            penState.color = wrapClamp(value + (change ? penState.color : 0), 0, 100);
            break;
    }

    penState.updateColor();

    if (legacy) {
        // https://github.com/scratchfoundation/scratch-vm/blob/8dbcc1fc8f8d8c4f1e40629fe8a388149d6dfd1c/src/extensions/scratch3_pen/index.js#L750-L767
        // Create the new color in RGB using the scratch 2 "shade" model
        QColor rgb = penState.penAttributes.color.toRgb();
        const double shade = (penState.shade > 100) ? 200 - penState.shade : penState.shade;

        if (shade < 50)
            rgb = mixRgb(QColor(Qt::black), rgb, (10 + shade) / 60);
        else
            rgb = mixRgb(rgb, QColor(Qt::white), (shade - 50) / 60);

        // Update the pen state according to new color
        const QColor hsv = rgb.toHsv();
        penState.setColor(hsv);
    }
}

double PenBlocks::wrapClamp(double n, double min, double max)
{
    // TODO: Move this to a separate class
    const double range = max - min + 1;
    return n - (std::floor((n - min) / range) * range);
}

QColor PenBlocks::mixRgb(const QColor &rgb0, const QColor &rgb1, double fraction1)
{
    // https://github.com/scratchfoundation/scratch-vm/blob/a4f095db5e03e072ba222fe721eeeb543c9b9c15/src/util/color.js#L192-L201
    if (fraction1 <= 0)
        return rgb0;
    if (fraction1 >= 1)
        return rgb1;

    const double fraction0 = 1 - fraction1;
    return QColor((fraction0 * rgb0.red()) + (fraction1 * rgb1.red()), (fraction0 * rgb0.green()) + (fraction1 * rgb1.green()), (fraction0 * rgb0.blue()) + (fraction1 * rgb1.blue()));
}
