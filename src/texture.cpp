// SPDX-License-Identifier: LGPL-3.0-or-later

#include "texture.h"

using namespace scratchcpprender;

Texture::Texture() :
    m_size(QSize(0, 0))
{
}

Texture::Texture(GLuint texture, const QSize &size) :
    m_handle(texture),
    m_isValid(true),
    m_size(size)
{
}

Texture::Texture(GLuint texture, int width, int height) :
    Texture(texture, QSize(width, height))
{
}

GLuint Texture::handle() const
{
    return m_handle;
}

bool Texture::isValid() const
{
    return m_isValid;
}

const QSize &Texture::size() const
{
    return m_size;
}

int Texture::width() const
{
    return m_size.width();
}

int Texture::height() const
{
    return m_size.height();
}

QImage Texture::toImage() const
{
    if (!m_isValid)
        return QImage();

    QOpenGLContext *context = QOpenGLContext::currentContext();

    if (!context || !context->isValid())
        return QImage();

    QOpenGLExtraFunctions glF(context);
    glF.initializeOpenGLFunctions();

    // Create offscreen surface
    QOffscreenSurface surface;
    surface.setFormat(context->format());
    surface.create();
    Q_ASSERT(surface.isValid());

    // Save old surface
    QSurface *oldSurface = context->surface();

    // Make context active on the surface
    context->makeCurrent(&surface);

    const QRectF drawRect(0, 0, m_size.width(), m_size.height());
    const QSize drawRectSize = drawRect.size().toSize();

    // Create FBO, but attach the texture to it
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    QOpenGLFramebufferObject fbo(drawRectSize, format);

    // Create a custom FBO with the texture
    unsigned int textureFbo;
    glF.glGenFramebuffers(1, &textureFbo);
    glF.glBindFramebuffer(GL_FRAMEBUFFER, textureFbo);
    glF.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_handle, 0);

    if (glF.glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        qWarning() << "error: framebuffer incomplete when generating texture image";
        glF.glDeleteFramebuffers(1, &textureFbo);
        return QImage();
    }

    // Blit the FBO to the Qt FBO
    glF.glBindFramebuffer(GL_READ_FRAMEBUFFER, textureFbo);
    glF.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.handle());
    glF.glBlitFramebuffer(0, 0, m_size.width(), m_size.height(), 0, 0, fbo.width(), fbo.height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glF.glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glF.glDeleteFramebuffers(1, &textureFbo);

    // Get the image
    QImage image = fbo.toImage();

    // Restore old surface
    context->doneCurrent();

    if (oldSurface)
        context->makeCurrent(oldSurface);

    return image;
}

void Texture::release()
{
    QOpenGLContext *context = QOpenGLContext::currentContext();

    if (m_isValid && context) {
        QOpenGLExtraFunctions glF(context);
        glF.initializeOpenGLFunctions();
        glF.glDeleteTextures(1, &m_handle);
        m_isValid = false;
    }
}

bool Texture::operator==(const Texture &texture) const
{
    return (!m_isValid && !texture.m_isValid) || (m_isValid && texture.m_isValid && m_handle == texture.m_handle);
}

bool scratchcpprender::Texture::operator!=(const Texture &texture) const
{
    return !(*this == texture);
}
