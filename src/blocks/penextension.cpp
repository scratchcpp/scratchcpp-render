// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/iengine.h>

#include "penextension.h"
#include "penblocks.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

std::string PenExtension::name() const
{
    return "pen";
}

std::string PenExtension::description() const
{
    return "Pen extension";
}

void PenExtension::registerSections(IEngine *engine)
{
    engine->registerSection(std::make_shared<PenBlocks>());
}
