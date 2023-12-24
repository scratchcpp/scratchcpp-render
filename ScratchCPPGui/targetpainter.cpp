// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/target.h>
#include <scratchcpp/costume.h>

#include "targetpainter.h"
#include "irenderedtarget.h"
#include "spritemodel.h"

using namespace scratchcppgui;

TargetPainter::TargetPainter()
{
}

TargetPainter::~TargetPainter()
{
}

void TargetPainter::paint(QNanoPainter *painter)
{
    m_target->lockCostume();
    double width = m_target->width();
    double height = m_target->height();

    if (m_target->isSvg())
        m_target->paintSvg(painter);
    else {
        QOpenGLContext *context = QOpenGLContext::currentContext();
        Q_ASSERT(context);

        if (!context)
            return;

        QOffscreenSurface surface;
        surface.setFormat(context->format());
        surface.create();
        Q_ASSERT(surface.isValid());

        QSurface *oldSurface = context->surface();
        context->makeCurrent(&surface);

        painter->beginFrame(width, height);
        QNanoImage image = QNanoImage::fromCache(painter, m_target->bitmapBuffer(), m_target->bitmapUniqueKey());
        painter->drawImage(image, 0, 0, width, height);
        painter->endFrame();

        context->doneCurrent();
        context->makeCurrent(oldSurface);
    }

    m_target->updateHullPoints(framebufferObject());
    m_target->unlockCostume();
}

void TargetPainter::synchronize(QNanoQuickItem *item)
{
    m_target = dynamic_cast<IRenderedTarget *>(item);
    Q_ASSERT(m_target);
}
