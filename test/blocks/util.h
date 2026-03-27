#pragma once

namespace libscratchcpp
{

class IEngine;
class IExtension;

} // namespace libscratchcpp

void registerBlocks(libscratchcpp::IEngine *engine, libscratchcpp::IExtension *extension);
