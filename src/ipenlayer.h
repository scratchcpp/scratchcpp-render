// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <qnanoquickitem.h>

namespace libscratchcpp
{

class IEngine;

}

namespace scratchcpprender
{

struct PenAttributes;

class IPenLayer : public QNanoQuickItem
{
    public:
        IPenLayer(QNanoQuickItem *parent = nullptr) :
            QNanoQuickItem(parent)
        {
        }

        virtual ~IPenLayer() { }

        virtual libscratchcpp::IEngine *engine() const = 0;
        virtual void setEngine(libscratchcpp::IEngine *newEngine) = 0;

        virtual void clear() = 0;
        virtual void drawPoint(const PenAttributes &penAttributes, double x, double y) = 0;
        virtual void drawLine(const PenAttributes &penAttributes, double x0, double y0, double x1, double y1) = 0;

        virtual QOpenGLFramebufferObject *framebufferObject() const = 0;
};

} // namespace scratchcpprender
