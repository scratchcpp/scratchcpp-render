// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QColor>

#include "shadermanager.h"

namespace scratchcpprender
{

class EffectTransform
{
    public:
        EffectTransform() = delete;

        static QRgb transformColor(const std::unordered_map<ShaderManager::Effect, double> &effectValues, QRgb color);
        static void transformPoint(const std::unordered_map<ShaderManager::Effect, double> &effectValues, const QVector2D &vec, QVector2D &dst);
};

} // namespace scratchcpprender
