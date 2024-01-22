// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/compiler.h>
#include <scratchcpp/sprite.h>

#include "penblocks.h"
#include "penlayer.h"
#include "spritemodel.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

std::string PenBlocks::name() const
{
    return "Pen";
}

void PenBlocks::registerBlocks(IEngine *engine)
{
    // Blocks
    engine->addCompileFunction(this, "pen_clear", &compileClear);
    engine->addCompileFunction(this, "pen_penDown", &compilePenDown);
}

void PenBlocks::compileClear(Compiler *compiler)
{
    compiler->addFunctionCall(&clear);
}

void PenBlocks::compilePenDown(Compiler *compiler)
{
    compiler->addFunctionCall(&penDown);
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
    Target *target = vm->target();

    if (!target || target->isStage())
        return 0;

    Sprite *sprite = static_cast<Sprite *>(target);
    SpriteModel *model = static_cast<SpriteModel *>(sprite->getInterface());

    if (model)
        model->setPenDown(true);

    return 0;
}

