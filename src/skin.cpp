// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/costume.h>

#include "skin.h"
#include "texture.h"

using namespace scratchcpprender;

Skin::Skin(libscratchcpp::Costume *costume)
{
    if (!costume)
        return;
}

Texture Skin::createAndPaintTexture(int width, int height, bool multisampled)
{
    QOpenGLContext *context = QOpenGLContext::currentContext();

    if (!context || !context->isValid() || (width <= 0 || height <= 0))
        return Texture();

    // Create offscreen surface
    QOffscreenSurface surface;
    surface.setFormat(context->format());
    surface.create();
    Q_ASSERT(surface.isValid());

    // Save old surface
    QSurface *oldSurface = context->surface();

    // Make context active on the surface
    context->makeCurrent(&surface);

    const QRectF drawRect(0, 0, width, height);
    const QSize drawRectSize = drawRect.size().toSize();

    // Create multisampled FBO (if the multisampled parameter is set)
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    if (multisampled)
        format.setSamples(16);

    QOpenGLFramebufferObject fbo(drawRectSize, format);
    fbo.bind();

    // Create paint device
    QOpenGLPaintDevice device(drawRectSize);
    QPainter painter(&device);
    painter.beginNativePainting();
    painter.setRenderHint(QPainter::Antialiasing, false);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Call the skin-specific paint method
    paint(&painter);

    // Done with the painting
    painter.endNativePainting();
    painter.end();
    fbo.release();

    GLuint textureHandle;

    if (multisampled) {
        // Create non-multisampled FBO (we can't take the texture from the multisampled FBO)
        format.setSamples(0);

        QOpenGLFramebufferObject targetFbo(drawRectSize, format);
        targetFbo.bind();

        // Blit the multisampled FBO to target FBO
        QOpenGLFramebufferObject::blitFramebuffer(&targetFbo, &fbo);

        // Take the texture (will call targetFbo.release())
        textureHandle = targetFbo.takeTexture();
    } else {
        // Take the texture
        textureHandle = fbo.takeTexture();
    }

    // Restore old surface
    context->doneCurrent();

    if (oldSurface)
        context->makeCurrent(oldSurface);

    return Texture(textureHandle, drawRectSize);
}
