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

        void updateColor()
        {
            int h = color * 360 / 100;
            h %= 360;

            if (h < 0)
                h += 360;

            const int s = saturation * 255 / 100;
            const int v = brightness * 255 / 100;
            const int a = 255 - transparency * 255 / 100;

            penAttributes.color = QNanoColor::fromQColor(QColor::fromHsv(h, s, v, a));
        }
};

} // namespace scratchcpprender
