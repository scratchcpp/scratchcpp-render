// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <scratchcpp/iextension.h>

namespace scratchcpprender
{

class PenExtension : public libscratchcpp::IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;

        void registerSections(libscratchcpp::IEngine *engine) override;
};

} // namespace scratchcpprender
