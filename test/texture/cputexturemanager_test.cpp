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

            m_fbo = std::make_unique<QOpenGLFramebufferObject>(4, 6, format);
            paint(painter, fileName);
        }

        void paint(QNanoPainter *painter, const QString &fileName)
        {
            // Begin painting
            m_fbo->bind();
            painter->beginFrame(m_fbo->width(), m_fbo->height());
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // Paint
            QNanoImage image = QNanoImage::fromCache(painter, fileName);
            painter->drawImage(image, 0, 0);
            painter->endFrame();

            m_fbo->release();
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

    static const std::vector<QPoint> refHullPoints1 = { { 1, 1 }, { 1, 3 }, { 3, 3 }, { 3, 1 } };
    static const std::vector<QPoint> refHullPoints2 = { { 0, 0 }, { 0, 5 }, { 3, 5 }, { 3, 0 } };
    static const std::vector<QPoint> refHullPoints3 = { { 1, 0 }, { 1, 3 }, { 2, 3 }, { 3, 2 }, { 3, 0 } };

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
    std::vector<QPoint> hullPoints;

    for (int i = 0; i < 2; i++) {
        Texture texture1(imgPainter1.fbo()->texture(), imgPainter1.fbo()->size());
        GLubyte *data = manager.getTextureData(texture1);
        ASSERT_EQ(memcmp(data, refData1, 96), 0);
        manager.getTextureConvexHullPoints(texture1, QSize(), ShaderManager::Effect::NoEffect, {}, hullPoints);
        ASSERT_EQ(hullPoints, refHullPoints1);

        Texture texture2(imgPainter2.fbo()->texture(), imgPainter2.fbo()->size());
        data = manager.getTextureData(texture2);
        ASSERT_EQ(memcmp(data, refData2, 96), 0);
        manager.getTextureConvexHullPoints(texture2, QSize(), ShaderManager::Effect::NoEffect, {}, hullPoints);
        ASSERT_EQ(hullPoints, refHullPoints2);

        // Shape-changing effects
        auto mask = ShaderManager::Effect::Fisheye | ShaderManager::Effect::Whirl;
        const std::unordered_map<ShaderManager::Effect, double> effects = { { ShaderManager::Effect::Fisheye, 20 }, { ShaderManager::Effect::Whirl, 50 } };
        manager.getTextureConvexHullPoints(texture1, texture1.size(), mask, effects, hullPoints);
        ASSERT_EQ(hullPoints, refHullPoints3);
    }

    // Test removeTexture()
    imgPainter1.paint(&painter, "image.jpg");
    Texture texture(imgPainter1.fbo()->texture(), imgPainter1.fbo()->size());
    GLubyte *data = manager.getTextureData(texture);
    ASSERT_EQ(memcmp(data, refData1, 96), 0);
    manager.getTextureConvexHullPoints(texture, QSize(), ShaderManager::Effect::NoEffect, {}, hullPoints);
    ASSERT_EQ(hullPoints, refHullPoints1);

    manager.removeTexture(texture);
    data = manager.getTextureData(texture);
    ASSERT_EQ(memcmp(data, refData2, 96), 0);
    manager.getTextureConvexHullPoints(texture, QSize(), ShaderManager::Effect::NoEffect, {}, hullPoints);
    ASSERT_EQ(hullPoints, refHullPoints2);

    // Cleanup
    context.doneCurrent();
}

TEST_F(CpuTextureManagerTest, GetPointColor)
{
    // Create OpenGL context
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);

    // Paint
    QNanoPainter painter;
    ImagePainter imgPainter(&painter, "image.png");

    // Read texture data
    Texture texture(imgPainter.fbo()->texture(), imgPainter.fbo()->size());

    // Test
    CpuTextureManager manager;
    auto mask = ShaderManager::Effect::NoEffect;
    ASSERT_EQ(manager.getPointColor(texture, 0, 0, mask, {}), qRgba(0, 0, 0, 0));
    ASSERT_EQ(manager.getPointColor(texture, 1, 0, mask, {}), qRgba(0, 0, 0, 0));
    ASSERT_EQ(manager.getPointColor(texture, 2, 0, mask, {}), qRgba(0, 0, 0, 0));
    ASSERT_EQ(manager.getPointColor(texture, 3, 0, mask, {}), qRgba(0, 0, 0, 0));

    ASSERT_FALSE(manager.textureContainsPoint(texture, { 0, 1 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 1, 1 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 1.4, 1.25 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 2, 1 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 3, 1 }, mask, {}));

    ASSERT_EQ(manager.getPointColor(texture, 0, 1, mask, {}), qRgba(0, 0, 0, 0));
    ASSERT_EQ(manager.getPointColor(texture, 1, 1, mask, {}), qRgb(0, 0, 255));
    ASSERT_EQ(manager.getPointColor(texture, 2, 1, mask, {}), qRgb(255, 0, 255));
    ASSERT_EQ(manager.getPointColor(texture, 3, 1, mask, {}), qRgb(255, 128, 128));

    std::unordered_map<ShaderManager::Effect, double> effects = { { ShaderManager::Effect::Color, 50 } };
    mask = ShaderManager::Effect::Color;
    ASSERT_EQ(manager.getPointColor(texture, 1, 1, mask, effects), qRgb(255, 0, 128));
    ASSERT_EQ(manager.getPointColor(texture, 2, 1, mask, effects), qRgb(255, 128, 0));
    ASSERT_EQ(manager.getPointColor(texture, 3, 1, mask, effects), qRgb(192, 255, 128));

    // TODO: Test point transform (graphic effects that change shape)

    // Cleanup
    context.doneCurrent();
}

TEST_F(CpuTextureManagerTest, TextureContainsPoint)
{
    // Create OpenGL context
    QOpenGLContext context;
    QOffscreenSurface surface;
    createContextAndSurface(&context, &surface);

    // Paint
    QNanoPainter painter;
    ImagePainter imgPainter(&painter, "image.png");

    // Read texture data
    Texture texture(imgPainter.fbo()->texture(), imgPainter.fbo()->size());

    // Test
    CpuTextureManager manager;
    auto mask = ShaderManager::Effect::NoEffect;
    ASSERT_FALSE(manager.textureContainsPoint(texture, { 0, 0 }, mask, {}));
    ASSERT_FALSE(manager.textureContainsPoint(texture, { 1, 0 }, mask, {}));
    ASSERT_FALSE(manager.textureContainsPoint(texture, { 2, 0 }, mask, {}));
    ASSERT_FALSE(manager.textureContainsPoint(texture, { 3, 0 }, mask, {}));

    ASSERT_FALSE(manager.textureContainsPoint(texture, { 0, 1 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 1, 1 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 1.4, 1.25 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 2, 1 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 3, 1 }, mask, {}));

    ASSERT_TRUE(manager.textureContainsPoint(texture, { 1, 2 }, mask, {}));
    ASSERT_FALSE(manager.textureContainsPoint(texture, { 2, 2 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 3, 2 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 3.5, 2.1 }, mask, {}));

    ASSERT_TRUE(manager.textureContainsPoint(texture, { 1, 3 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 2, 3 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 3, 3 }, mask, {}));
    ASSERT_TRUE(manager.textureContainsPoint(texture, { 3.3, 3.5 }, mask, {}));

    // TODO: Test point transform (graphic effects that change shape)

    // Cleanup
    context.doneCurrent();
}
