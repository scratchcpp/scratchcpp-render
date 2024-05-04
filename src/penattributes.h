// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <qnanocolor.h>

namespace scratchcpprender
{

struct PenAttributes
{
        QNanoColor color = QNanoColor(0, 0, 255);
        double diameter = 1;
};

} // namespace scratchcpprender
