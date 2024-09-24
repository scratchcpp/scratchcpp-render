// SPDX-License-Identifier: LGPL-3.0-or-later

#include "graphicseffect.h"

using namespace scratchcpprender;

static std::unordered_map<ShaderManager::Effect, std::pair<double, double>> EFFECT_RANGE = {
    { ShaderManager::Effect::Ghost, { 0, 100 } },
    { ShaderManager::Effect::Brightness, { -100, 100 } }
};

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


double GraphicsEffect::clamp(double value) const
{
    // https://github.com/scratchfoundation/scratch-vm/blob/8dbcc1fc8f8d8c4f1e40629fe8a388149d6dfd1c/src/blocks/scratch3_looks.js#L523-L538
    if(m_effect == ShaderManager::Effect::Ghost || m_effect == ShaderManager::Effect::Brightness)
            return std::clamp(value, EFFECT_RANGE[m_effect].first, EFFECT_RANGE[m_effect].second);

    return value;
}
