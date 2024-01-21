#include <penlayerpainter.h>
#include <penlayermock.h>

#include "../common.h"

using namespace scratchcpprender;

using ::testing::Return;
using ::testing::ReturnRef;

class PenLayerPainterTest : public testing::Test
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

TEST_F(PenLayerPainterTest, Paint)
{
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);

    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    // Begin painting reference
    QNanoPainter refPainter;
    QOpenGLFramebufferObject refFbo(40, 60, format);
    refFbo.bind();
    refPainter.beginFrame(refFbo.width(), refFbo.height());

    // Paint reference
    refPainter.setAntialias(0);
    refPainter.setStrokeStyle(QNanoColor(255, 0, 0, 128));
    refPainter.ellipse(refFbo.width() / 2, refFbo.height() / 2, refFbo.width() / 2, refFbo.height() / 2);
    refPainter.stroke();
    refPainter.endFrame();

    // Begin painting
    QNanoPainter painter;
    QOpenGLFramebufferObject fbo(40, 60, format);
    fbo.bind();
    painter.beginFrame(fbo.width(), fbo.height());

    // Create pen layer painter
    PenLayerPainter penLayerPainter(&fbo);
    PenLayerMock penLayer;

    EXPECT_CALL(penLayer, framebufferObject()).WillOnce(Return(&refFbo));
    penLayerPainter.synchronize(&penLayer);

    // Paint
    Texture texture(refFbo.texture(), refFbo.size());
    penLayerPainter.paint(&painter);
    painter.endFrame();

    // Compare resulting images
    ASSERT_EQ(fbo.toImage(), refFbo.toImage());

    // Release
    fbo.release();
    refFbo.release();

    context.doneCurrent();
}
