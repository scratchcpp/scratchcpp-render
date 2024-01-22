// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <scratchcpp/iblocksection.h>

namespace scratchcpprender
{

class PenBlocks : public libscratchcpp::IBlockSection
{
    public:
        enum Inputs
        {
        };

        std::string name() const override;

        void registerBlocks(libscratchcpp::IEngine *engine) override;

        static void compileClear(libscratchcpp::Compiler *compiler);

        static unsigned int clear(libscratchcpp::VirtualMachine *vm);
};

} // namespace scratchcpprender
