// SPDX-License-Identifier: LGPL-3.0-or-later

#include "penlayerpainter.h"
#include "penlayer.h"

using namespace scratchcpprender;

PenLayerPainter::PenLayerPainter(QOpenGLFramebufferObject *fbo)
{
    m_targetFbo = fbo;
}

void PenLayerPainter::paint(QNanoPainter *painter)
{
    if (QThread::currentThread() != qApp->thread())
        qFatal("Error: Rendering must happen in the GUI thread to work correctly. Did you initialize the library using scratchcpprender::init()?");

    QOpenGLContext *context = QOpenGLContext::currentContext();
    Q_ASSERT(context);

    if (!context || !m_fbo)
        return;

    // Custom FBO - only used for testing
    QOpenGLFramebufferObject *targetFbo = m_targetFbo ? m_targetFbo : framebufferObject();

    // Blit the FBO to the item FBO
    QOpenGLFramebufferObject::blitFramebuffer(targetFbo, m_fbo);
}

void PenLayerPainter::synchronize(QNanoQuickItem *item)
{
    IPenLayer *penLayer = dynamic_cast<IPenLayer *>(item);
    Q_ASSERT(penLayer);

    if (penLayer)
        m_fbo = penLayer->framebufferObject();
}
