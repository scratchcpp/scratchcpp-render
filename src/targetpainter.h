// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <qnanoquickitempainter.h>

namespace scratchcpprender
{

class IRenderedTarget;
class Skin;

class TargetPainter : public QNanoQuickItemPainter
{
    public:
        TargetPainter(QOpenGLFramebufferObject *fbo = nullptr);
        ~TargetPainter();

        void paint(QNanoPainter *painter) override;
        void synchronize(QNanoQuickItem *item) override;

    private:
        QOpenGLFramebufferObject *m_fbo = nullptr;
        IRenderedTarget *m_target = nullptr;
};

} // namespace scratchcpprender
