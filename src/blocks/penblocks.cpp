#include "penblocks.h"

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
}
