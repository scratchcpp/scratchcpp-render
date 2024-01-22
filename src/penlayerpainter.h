// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <qnanoquickitempainter.h>

#include "texture.h"

namespace scratchcpprender
{

class PenLayerPainter : public QNanoQuickItemPainter
{
    public:
        PenLayerPainter(QOpenGLFramebufferObject *fbo = nullptr);

        void paint(QNanoPainter *painter) override;
        void synchronize(QNanoQuickItem *item) override;

    private:
        QOpenGLFramebufferObject *m_targetFbo = nullptr;
        QOpenGLFramebufferObject *m_fbo = nullptr;
};

} // namespace scratchcpprender
