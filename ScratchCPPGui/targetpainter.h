// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <qnanoquickitempainter.h>

namespace scratchcppgui
{

class IRenderedTarget;

class TargetPainter : public QNanoQuickItemPainter
{
    public:
        TargetPainter();
        ~TargetPainter();

        void paint(QNanoPainter *painter) override;
        void synchronize(QNanoQuickItem *item) override;

    private:
        IRenderedTarget *m_target = nullptr;
};

} // namespace scratchcppgui
