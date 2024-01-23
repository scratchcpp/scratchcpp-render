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
    engine->addCompileFunction(this, "pen_changePenSizeBy", &compileChangePenSizeBy);
    engine->addCompileFunction(this, "pen_setPenSizeTo", &compileSetPenSizeTo);

    // Inputs
    engine->addInput(this, "SIZE", SIZE);
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

SpriteModel *PenBlocks::getSpriteModel(libscratchcpp::VirtualMachine *vm)
{
    Target *target = vm->target();

    if (!target || target->isStage())
        return nullptr;

    Sprite *sprite = static_cast<Sprite *>(target);
    SpriteModel *model = static_cast<SpriteModel *>(sprite->getInterface());
    return model;
}
