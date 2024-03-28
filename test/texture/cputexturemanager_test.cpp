#include <cputexturemanager.h>
#include <texture.h>
#include <qnanopainter.h>

#include "../common.h"

using namespace scratchcpprender;

class CpuTextureManagerTest : public testing::Test
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

class ImagePainter
{
    public:
        ImagePainter(QNanoPainter *painter, const QString &fileName)
        {
            QOpenGLFramebufferObjectFormat format;
            format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

            // Begin painting
            m_fbo = std::make_unique<QOpenGLFramebufferObject>(4, 6, format);
            m_fbo->bind();
            painter->beginFrame(m_fbo->width(), m_fbo->height());

            // Paint
            QNanoImage image = QNanoImage::fromCache(painter, fileName);
            painter->drawImage(image, 0, 0);
            painter->endFrame();
        }

        ~ImagePainter() { m_fbo->release(); }

        QOpenGLFramebufferObject *fbo() const { return m_fbo.get(); };

    private:
        std::unique_ptr<QOpenGLFramebufferObject> m_fbo;
};

TEST_F(CpuTextureManagerTest, TextureDataAndHullPoints)
{
    static const GLubyte refData1[] = {
        0, 0, 0, 0, 0,   0, 0,   0,   0,   0,   0,   0,   0,   0,   0, 0,   0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 255, 255, 255, 128, 128, 255, 0, 0, 0, 0, 0, 0, 128, 255, 0, 0, 0, 0, 87, 149, 87, 149,
        0, 0, 0, 0, 128, 0, 128, 255, 128, 128, 255, 255, 128, 128, 0, 255, 0, 0, 0, 0, 0, 0, 0,   0,   0,   0, 0,   0,   0,   0,   0,   0,   0, 0, 0, 0, 0, 0, 0,   0,   0, 0, 0, 0, 0,  0,   0,  0
    };

    static const GLubyte refData2[] = {
        0,  0, 57, 255, 10, 0, 50, 255, 43, 0, 35, 255, 60,  0,   28,  255, 0,  0, 55, 255, 39, 15, 73, 255, 137, 85,  133, 255, 207, 142, 182, 255,
        10, 0, 50, 255, 23, 4, 50, 255, 4,  0, 7,  255, 204, 204, 196, 255, 11, 0, 35, 255, 59, 46, 76, 255, 135, 146, 140, 255, 99,  123, 99,  255,
        4,  0, 12, 255, 1,  0, 7,  255, 0,  1, 0,  255, 0,   3,   0,   255, 0,  0, 0,  255, 0,  0,  0,  255, 0,   0,   0,   255, 0,   0,   0,   255
    };

    static const std::vector<QPoint> refHullPoints1 = { { 1, 1 }, { 2, 1 }, { 3, 1 }, { 1, 2 }, { 3, 2 }, { 1, 3 }, { 2, 3 }, { 3, 3 } };

    static const std::vector<QPoint> refHullPoints2 = {
        { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 }, { 3, 1 }, { 0, 2 }, { 1, 2 }, { 2, 2 }, { 3, 2 },
        { 0, 3 }, { 1, 3 }, { 2, 3 }, { 3, 3 }, { 0, 4 }, { 1, 4 }, { 2, 4 }, { 3, 4 }, { 0, 5 }, { 1, 5 }, { 2, 5 }, { 3, 5 }
    };

    // Create OpenGL context
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);

    // Paint images
    QNanoPainter painter;
    ImagePainter imgPainter1(&painter, "image.png");
    ImagePainter imgPainter2(&painter, "image.jpg");

    // Read texture data
    CpuTextureManager manager;

    for (int i = 0; i < 2; i++) {
        Texture texture1(imgPainter1.fbo()->texture(), imgPainter1.fbo()->size());
        GLubyte *data = manager.getTextureData(texture1);
        ASSERT_EQ(memcmp(data, refData1, 96), 0);
        const auto &hullPoints1 = manager.getTextureConvexHullPoints(texture1);
        ASSERT_EQ(hullPoints1, refHullPoints1);

        Texture texture2(imgPainter2.fbo()->texture(), imgPainter2.fbo()->size());
        data = manager.getTextureData(texture2);
        ASSERT_EQ(memcmp(data, refData2, 96), 0);
        const auto &hullPoints2 = manager.getTextureConvexHullPoints(texture2);
        ASSERT_EQ(hullPoints2, refHullPoints2);
    }

    // Cleanup
    context.doneCurrent();
}
