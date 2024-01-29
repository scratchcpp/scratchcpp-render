// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <scratchcpp/igraphicseffect.h>

#include "shadermanager.h"

namespace scratchcpprender
{

class GraphicsEffect : public libscratchcpp::IGraphicsEffect
{
    public:
        GraphicsEffect(ShaderManager::Effect effect, const std::string &name);

        ShaderManager::Effect effect() const;
        std::string name() const override;

    private:
        ShaderManager::Effect m_effect = static_cast<ShaderManager::Effect>(0);
        std::string m_name;
};

} // namespace scratchcpprender
