#include <texture.h>

#include "../common.h"

using namespace scratchcpprender;

TEST(TextureTest, Constructors)
{
    {
        Texture tex;
        ASSERT_EQ(tex.handle(), 0);
        ASSERT_FALSE(tex.isValid());
        ASSERT_EQ(tex.size().width(), 0);
        ASSERT_EQ(tex.size().height(), 0);
        ASSERT_EQ(tex.width(), 0);
        ASSERT_EQ(tex.height(), 0);
    }

    {
        Texture tex(2, QSize(4, 2));
        ASSERT_EQ(tex.handle(), 2);
        ASSERT_TRUE(tex.isValid());
        ASSERT_EQ(tex.size().width(), 4);
        ASSERT_EQ(tex.size().height(), 2);
        ASSERT_EQ(tex.width(), 4);
        ASSERT_EQ(tex.height(), 2);
    }

    {
        Texture tex(2, 5, 8);
        ASSERT_EQ(tex.handle(), 2);
        ASSERT_TRUE(tex.isValid());
        ASSERT_EQ(tex.size().width(), 5);
        ASSERT_EQ(tex.size().height(), 8);
        ASSERT_EQ(tex.width(), 5);
        ASSERT_EQ(tex.height(), 8);
    }
}

TEST(TextureTest, ToImage)
{
    QOpenGLContext context;
    context.create();
    ASSERT_TRUE(context.isValid());

    QOffscreenSurface surface;
    surface.setFormat(context.format());
    surface.create();
    Q_ASSERT(surface.isValid());
    context.makeCurrent(&surface);

    QOpenGLExtraFunctions glF(&context);
    glF.initializeOpenGLFunctions();

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    QOpenGLFramebufferObject fbo(80, 60, format);
    fbo.bind();

    QOpenGLPaintDevice device(fbo.size());
    QPainter painter(&device);
    painter.beginNativePainting();
    painter.setRenderHint(QPainter::Antialiasing, false);
    glF.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glF.glClear(GL_COLOR_BUFFER_BIT);
    painter.drawEllipse(0, 0, fbo.width(), fbo.height());
    painter.endNativePainting();
    painter.end();

    QImage image = fbo.toImage();

    Texture tex(fbo.takeTexture(), fbo.width(), fbo.height());
    ASSERT_EQ(tex.toImage(), image);

    tex.release();
    context.doneCurrent();
}

TEST(TextureTest, Release)
{
    QOpenGLContext context;
    context.create();
    ASSERT_TRUE(context.isValid());

    QOffscreenSurface surface;
    surface.setFormat(context.format());
    surface.create();
    Q_ASSERT(surface.isValid());
    context.makeCurrent(&surface);

    QOpenGLExtraFunctions glF(&context);
    glF.initializeOpenGLFunctions();

    QOpenGLFramebufferObject fbo(1, 1);
    GLuint handle = fbo.takeTexture();
    ASSERT_TRUE(glF.glIsTexture(handle));

    Texture tex(handle, fbo.width(), fbo.height());
    ASSERT_TRUE(glF.glIsTexture(handle));

    tex.release();
    ASSERT_FALSE(glF.glIsTexture(handle));
    ASSERT_FALSE(tex.isValid());

    context.doneCurrent();
}

TEST(TextureTest, Operators)
{
    Texture t1;
    Texture t2;
    ASSERT_TRUE(t1 == t2);
    ASSERT_FALSE(t1 != t2);

    Texture t3(3, 10, 10);
    ASSERT_FALSE(t1 == t3);
    ASSERT_TRUE(t1 != t3);

    Texture t4(3, 10, 10);
    ASSERT_TRUE(t3 == t4);
    ASSERT_FALSE(t3 != t4);

    Texture t5(2, 10, 10);
    ASSERT_FALSE(t4 == t5);
    ASSERT_TRUE(t4 != t5);
}
