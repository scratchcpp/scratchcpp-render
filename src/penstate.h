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

            penAttributes.color = color;
        }

        void updateColor()
        {
            int h = std::round(color * 360 / 100);
            h %= 360;

            if (h < 0)
                h += 360;

            const int s = std::round(saturation * 2.55);
            const int v = std::round(brightness * 2.55);
            const int a = std::round((100 - transparency) * 2.55);

            penAttributes.color = QColor::fromHsv(h, s, v, a);
        }
};

} // namespace scratchcpprender
