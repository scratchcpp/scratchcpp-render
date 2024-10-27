// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QOpenGLExtraFunctions>
#include <scratchcpp/target.h>
#include <scratchcpp/costume.h>

#include "targetpainter.h"
#include "irenderedtarget.h"
#include "spritemodel.h"
#include "bitmapskin.h"
#include "shadermanager.h"

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
    if (QThread::currentThread() != qApp->thread())
        qFatal("Error: Rendering must happen in the GUI thread to work correctly. Did you initialize the library using scratchcpprender::init()?");

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

    // Get the shader program for the current set of effects
    ShaderManager *shaderManager = ShaderManager::instance();

    const auto &effects = m_target->graphicEffects();
    QOpenGLShaderProgram *shaderProgram = shaderManager->getShaderProgram(effects);
    Q_ASSERT(shaderProgram);
    Q_ASSERT(shaderProgram->isLinked());

    // Set up vertex data and buffers for a quad
    float vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
    };

    GLuint VBO, VAO;
    glF.glGenVertexArrays(1, &VAO);
    glF.glGenBuffers(1, &VBO);

    glF.glBindVertexArray(VAO);

    glF.glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glF.glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glF.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glF.glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glF.glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glF.glEnableVertexAttribArray(1);

    // Render to the target framebuffer
    glF.glBindFramebuffer(GL_FRAMEBUFFER, targetFbo->handle());
    shaderProgram->bind();
    glF.glBindVertexArray(VAO);
    glF.glActiveTexture(GL_TEXTURE0);
    glF.glBindTexture(GL_TEXTURE_2D, texture.handle());
    shaderManager->setUniforms(shaderProgram, 0, m_target->cpuTexture().size(), effects); // set texture and effect uniforms
    glF.glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Process the resulting texture
    // NOTE: This must happen now, not later, because the alpha channel can be used here
    // Currently nothing is happening here...

    // Cleanup
    shaderProgram->release();
    glF.glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glF.glDeleteFramebuffers(1, &fbo);

    // Delete vertex array and buffer
    glF.glDeleteVertexArrays(1, &VAO);
    glF.glDeleteBuffers(1, &VBO);
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
