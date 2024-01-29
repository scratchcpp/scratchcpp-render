// SPDX-License-Identifier: LGPL-3.0-or-later

#include "graphicseffect.h"

using namespace scratchcpprender;

GraphicsEffect::GraphicsEffect(ShaderManager::Effect effect, const std::string &name) :
    m_effect(effect),
    m_name(name)
{
}

ShaderManager::Effect GraphicsEffect::effect() const
{
    return m_effect;
}

std::string GraphicsEffect::name() const
{
    return m_name;
}
