#include <textbubblepainter.h>
#include <textbubbleshape.h>

#include "../common.h"

using namespace scratchcpprender;

class TextBubblePainterTest : public testing::Test
{
    public:
        void createContextAndSurface(QOpenGLContext *context, QOffscreenSurface *surface)
        {
            QSurfaceFormat surfaceFormat;
            surfaceFormat.setMajorVersion(4);
            surfaceFormat.setMinorVersion(3);

            context->setFormat(surfaceFormat);
            context->create();
            ASSERT_TRUE(context->isValid());

            surface->setFormat(surfaceFormat);
            surface->create();
            ASSERT_TRUE(surface->isValid());

            context->makeCurrent(surface);
            ASSERT_EQ(QOpenGLContext::currentContext(), context);
        }
};

TEST_F(TextBubblePainterTest, PaintSayBubble)
{
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);

    QOpenGLExtraFunctions glF(&context);
    glF.initializeOpenGLFunctions();

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    // Begin painting
    QNanoPainter painter;
    QOpenGLFramebufferObject fbo(425, 250, format);
    fbo.bind();
    glF.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glF.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    painter.beginFrame(fbo.width(), fbo.height());
    painter.setAntialias(0);

    // Create text bubble painter
    TextBubblePainter bubblePainter;
    TextBubbleShape bubble;
    bubble.setType(TextBubbleShape::Type::Say);
    bubble.setStageScale(2.5);
    bubble.setNativeWidth(fbo.width() / bubble.stageScale());
    bubble.setNativeHeight(fbo.height() / bubble.stageScale());
    bubblePainter.synchronize(&bubble);

    // Paint
    bubblePainter.paint(&painter);
    painter.endFrame();

    // Compare with reference image
    QBuffer buffer;
    fbo.toImage().save(&buffer, "png");
    QFile ref("say_bubble.png");
    ref.open(QFile::ReadOnly);
    buffer.open(QFile::ReadOnly);
    ASSERT_EQ(ref.readAll(), buffer.readAll());

    // Release
    fbo.release();
    context.doneCurrent();
}

TEST_F(TextBubblePainterTest, PaintThinkBubble)
{
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);

    QOpenGLExtraFunctions glF(&context);
    glF.initializeOpenGLFunctions();

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    // Begin painting
    QNanoPainter painter;
    QOpenGLFramebufferObject fbo(156, 117, format);
    fbo.bind();
    glF.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glF.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    painter.beginFrame(fbo.width(), fbo.height());
    painter.setAntialias(0);

    // Create text bubble painter
    TextBubblePainter bubblePainter;
    TextBubbleShape bubble;
    bubble.setType(TextBubbleShape::Type::Think);
    bubble.setStageScale(1.25);
    bubble.setNativeWidth(fbo.width() / bubble.stageScale());
    bubble.setNativeHeight(fbo.height() / bubble.stageScale());
    bubblePainter.synchronize(&bubble);

    // Paint
    bubblePainter.paint(&painter);
    painter.endFrame();

    // Compare with reference image
    QBuffer buffer;
    fbo.toImage().save(&buffer, "png");
    QFile ref("think_bubble.png");
    ref.open(QFile::ReadOnly);
    buffer.open(QFile::ReadOnly);
    ASSERT_EQ(ref.readAll(), buffer.readAll());

    // Release
    fbo.release();
    context.doneCurrent();
}
