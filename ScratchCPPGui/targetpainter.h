// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <qnanoquickitempainter.h>
namespace scratchcppgui
{

class RenderedTarget;

class TargetPainter : public QNanoQuickItemPainter
{
    public:
        TargetPainter();
        ~TargetPainter();

        void paint(QNanoPainter *painter) override;
        void synchronize(QNanoQuickItem *item) override;

    private:
        RenderedTarget *m_target = nullptr;
};

} // namespace scratchcppgui
