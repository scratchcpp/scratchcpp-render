// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QOpenGLExtraFunctions>
#include <scratchcpp/target.h>
#include <scratchcpp/costume.h>

#include "targetpainter.h"
#include "irenderedtarget.h"
#include "spritemodel.h"
#include "bitmapskin.h"

using namespace scratchcpprender;

TargetPainter::TargetPainter(QOpenGLFramebufferObject *fbo) :
    m_fbo(fbo)
{
}

TargetPainter::~TargetPainter()
{
}

void TargetPainter::paint(QNanoPainter *painter)
{
    if (QThread::currentThread() != qApp->thread()) {
        qFatal("Error: Rendering must happen in the GUI thread to work correctly. Please disable threaded render loop using qputenv(\"QSG_RENDER_LOOP\", \"basic\") before constructing your "
               "application object.");
    }

    QOpenGLContext *context = QOpenGLContext::currentContext();
    Q_ASSERT(context);

    if (!context)
        return;

    // Custom FBO - only used for testing
    QOpenGLFramebufferObject *targetFbo = m_fbo ? m_fbo : framebufferObject();

    QOpenGLExtraFunctions glF(context);
    glF.initializeOpenGLFunctions();

    // Cancel current frame because we're using a custom FBO
    painter->cancelFrame();

    Texture texture = m_target->texture();

    if (!texture.isValid())
        return;

    // Create a FBO for the current texture
    unsigned int fbo;
    glF.glGenFramebuffers(1, &fbo);
    glF.glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glF.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.handle(), 0);

    if (glF.glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        qWarning() << "error: framebuffer incomplete (" + m_target->scratchTarget()->name() + ")";
        glF.glDeleteFramebuffers(1, &fbo);
        return;
    }

    // Blit the FBO to the Qt Quick FBO
    glF.glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glF.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFbo->handle());
    glF.glBlitFramebuffer(0, 0, texture.width(), texture.height(), 0, 0, targetFbo->width(), targetFbo->height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glF.glBindFramebuffer(GL_FRAMEBUFFER, targetFbo->handle());

    glF.glDeleteFramebuffers(1, &fbo);

    m_target->updateHullPoints(targetFbo);
}

void TargetPainter::synchronize(QNanoQuickItem *item)
{
    m_target = dynamic_cast<IRenderedTarget *>(item);
    Q_ASSERT(m_target);

    // Render costumes into textures
    if (!m_target->costumesLoaded()) {
        m_target->loadCostumes();
        invalidateFramebufferObject();
    }
}
