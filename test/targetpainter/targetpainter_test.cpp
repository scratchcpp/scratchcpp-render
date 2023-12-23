#include <targetpainter.h>
#include <renderedtargetmock.h>

#include "../common.h"

using namespace scratchcppgui;

using ::testing::Return;
using ::testing::ReturnRef;

class TargetPainterTest : public testing::Test
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

TEST_F(TargetPainterTest, PaintBitmap)
{
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);

    std::unordered_map<std::string, std::string> files = { { "image.jpg", "jpeg_result.png" }, { "image.png", "png_result.png" } };

    for (const auto &[inFile, outFile] : files) {
        // Create target painter
        TargetPainter targetPainter;
        QNanoPainter painter;
        RenderedTargetMock target;
        targetPainter.synchronize(&target);

        // Load the image
        QBuffer buffer;
        buffer.open(QBuffer::WriteOnly);
        std::string str = readFileStr(inFile);
        buffer.write(str.c_str(), str.size());
        buffer.close();

        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

        // Begin painting reference
        QNanoPainter refPainter;
        QOpenGLFramebufferObject refFbo(40, 60, format);
        refFbo.bind();
        refPainter.beginFrame(refFbo.width(), refFbo.height());

        // Paint reference
        QNanoImage refImage = QNanoImage::fromCache(&refPainter, &buffer, "abc");
        refPainter.drawImage(refImage, 0, 0, 40, 60);
        refPainter.endFrame();

        // Begin painting
        QOpenGLFramebufferObject fbo(40, 60, format);
        fbo.bind();
        painter.beginFrame(fbo.width(), fbo.height());

        // Paint
        EXPECT_CALL(target, lockCostume());
        EXPECT_CALL(target, width()).WillOnce(Return(40));
        EXPECT_CALL(target, height()).WillOnce(Return(60));
        EXPECT_CALL(target, isSvg()).WillOnce(Return(false));
        EXPECT_CALL(target, bitmapBuffer()).WillOnce(Return(&buffer));
        static const QString uniqueKey("abc");
        EXPECT_CALL(target, bitmapUniqueKey()).WillOnce(ReturnRef(uniqueKey));
        EXPECT_CALL(target, updateHullPoints);
        EXPECT_CALL(target, unlockCostume());
        targetPainter.paint(&painter);
        painter.endFrame();

        // Compare resulting images
        ASSERT_EQ(fbo.toImage(), refFbo.toImage());

        // Release
        fbo.release();
        refFbo.release();
    }

    context.doneCurrent();
}

TEST_F(TargetPainterTest, PaintSvg)
{
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);

    TargetPainter targetPainter;
    QNanoPainter painter;
    RenderedTargetMock target;
    targetPainter.synchronize(&target);

    EXPECT_CALL(target, lockCostume());
    EXPECT_CALL(target, width()).WillOnce(Return(40));
    EXPECT_CALL(target, height()).WillOnce(Return(60));
    EXPECT_CALL(target, isSvg()).WillOnce(Return(true));
    EXPECT_CALL(target, paintSvg(&painter));
    EXPECT_CALL(target, updateHullPoints);
    EXPECT_CALL(target, unlockCostume());
    targetPainter.paint(&painter);

    context.doneCurrent();
}
