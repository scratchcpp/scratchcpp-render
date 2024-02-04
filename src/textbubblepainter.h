// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <qnanoquickitempainter.h>

namespace scratchcpprender
{

class TextBubbleShape;

class TextBubblePainter : public QNanoQuickItemPainter
{
    public:
        void paint(QNanoPainter *painter) override;
        void synchronize(QNanoQuickItem *item) override;

    private:
        TextBubbleShape *m_item = nullptr;
};

} // namespace scratchcpprender
