// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QColor>

#include "penattributes.h"

namespace scratchcpprender
{

struct PenState
{
        bool penDown = false;
        double color = 66.66;
        double saturation = 100;
        double brightness = 100;
        double transparency = 0;
        double shade = 50; // for legacy blocks
        PenAttributes penAttributes;

        void setColor(const QColor &color)
        {
            QColor hsvColor = color.toHsv();
            this->color = hsvColor.hue() * 100 / 360.0;
            this->saturation = hsvColor.saturationF() * 100;
            this->brightness = hsvColor.valueF() * 100;
            this->transparency = 100 * (1 - hsvColor.alphaF());
        }
};

} // namespace scratchcpprender
