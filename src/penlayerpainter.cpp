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
    if (QThread::currentThread() != qApp->thread()) {
        qFatal("Error: Rendering must happen in the GUI thread to work correctly. Please disable threaded render loop using qputenv(\"QSG_RENDER_LOOP\", \"basic\") before constructing your "
               "application object.");
    }

    QOpenGLContext *context = QOpenGLContext::currentContext();
    Q_ASSERT(context);

    if (!context || !m_fbo)
        return;

    // Custom FBO - only used for testing
    QOpenGLFramebufferObject *targetFbo = m_targetFbo ? m_targetFbo : framebufferObject();

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    // Blit the FBO to a temporary FBO first (multisampled FBOs can only be blitted to FBOs with the same size)
    QOpenGLFramebufferObject tmpFbo(m_fbo->size(), format);
    QOpenGLFramebufferObject::blitFramebuffer(&tmpFbo, m_fbo);
    QOpenGLFramebufferObject::blitFramebuffer(targetFbo, &tmpFbo);
}

void PenLayerPainter::synchronize(QNanoQuickItem *item)
{
    IPenLayer *penLayer = dynamic_cast<IPenLayer *>(item);
    Q_ASSERT(penLayer);

    if (penLayer)
        m_fbo = penLayer->framebufferObject();
}
