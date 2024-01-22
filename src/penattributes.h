// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QColor>

namespace scratchcpprender
{

struct PenAttributes
{
        QColor color = QColor(0, 0, 255);
        double diameter = 1;
};

} // namespace scratchcpprender
