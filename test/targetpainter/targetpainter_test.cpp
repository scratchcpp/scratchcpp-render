#include <targetpainter.h>
#include <renderedtargetmock.h>

#include "../common.h"

using namespace scratchcpprender;

using ::testing::Return;
using ::testing::ReturnRef;

class TargetPainterTest : public testing::Test
{
    public:
        void SetUp() override
        {
            m_context.create();
            ASSERT_TRUE(m_context.isValid());

            m_surface.setFormat(m_context.format());
            m_surface.create();
            Q_ASSERT(m_surface.isValid());
            m_context.makeCurrent(&m_surface);
        }

        void TearDown() override
        {
            ASSERT_EQ(m_context.surface(), &m_surface);
            m_context.doneCurrent();
        }

        QOpenGLContext m_context;
        QOffscreenSurface m_surface;
};

TEST_F(TargetPainterTest, Paint)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

    QOpenGLExtraFunctions glF(&m_context);
    glF.initializeOpenGLFunctions();

    // Begin painting reference
    QNanoPainter refPainter;
    QOpenGLFramebufferObject refFbo(40, 60, format);
    refFbo.bind();
    refPainter.beginFrame(refFbo.width(), refFbo.height());

    // Paint reference
    refPainter.setAntialias(0);
    refPainter.setStrokeStyle(QNanoColor(255, 0, 0, 128));
    refPainter.setFillStyle(QNanoColor(0, 0, 100, 150));
    refPainter.rect(5, 5, refFbo.width() - 10, refFbo.height() - 10);
    refPainter.stroke();
    refPainter.fill();
    refPainter.endFrame();

    // Begin painting
    QNanoPainter painter;
    QOpenGLFramebufferObject fbo(40, 60, format);
    fbo.bind();
    painter.beginFrame(fbo.width(), fbo.height());

    // Create target painter
    TargetPainter targetPainter(&fbo);
    RenderedTargetMock target;

    EXPECT_CALL(target, costumesLoaded()).WillOnce(Return(false));
    EXPECT_CALL(target, loadCostumes());
    targetPainter.synchronize(&target);

    EXPECT_CALL(target, costumesLoaded()).WillOnce(Return(true));
    EXPECT_CALL(target, loadCostumes()).Times(0);
    targetPainter.synchronize(&target);

    EXPECT_CALL(target, costumesLoaded()).WillOnce(Return(false));
    EXPECT_CALL(target, loadCostumes());
    targetPainter.synchronize(&target);

    // Paint
    Texture texture(refFbo.texture(), refFbo.size());
    std::unordered_map<ShaderManager::Effect, double> effects;
    EXPECT_CALL(target, texture()).WillOnce(Return(texture));
    EXPECT_CALL(target, costumeWidth()).WillOnce(Return(texture.width()));
    EXPECT_CALL(target, costumeHeight()).WillOnce(Return(texture.height()));
    EXPECT_CALL(target, graphicEffects()).WillOnce(ReturnRef(effects));
    targetPainter.paint(&painter);
    painter.endFrame();

    // Compare resulting images
    ASSERT_EQ(fbo.toImage(), refFbo.toImage());

    // Paint with color effects
    glF.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glF.glClear(GL_COLOR_BUFFER_BIT);
    effects[ShaderManager::Effect::Color] = 46;
    effects[ShaderManager::Effect::Brightness] = 20;
    effects[ShaderManager::Effect::Ghost] = 84;
    EXPECT_CALL(target, texture()).WillOnce(Return(texture));
    EXPECT_CALL(target, costumeWidth()).WillOnce(Return(texture.width()));
    EXPECT_CALL(target, costumeHeight()).WillOnce(Return(texture.height()));
    EXPECT_CALL(target, graphicEffects()).WillOnce(ReturnRef(effects));
    targetPainter.paint(&painter);
    painter.endFrame();
    effects.clear();

    // Compare with reference image
    ASSERT_LE(fuzzyCompareImages(fbo.toImage(), QImage("color_effects.png")), 0.07);

    // Paint with shape changing effects
    glF.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glF.glClear(GL_COLOR_BUFFER_BIT);
    effects.clear();
    effects[ShaderManager::Effect::Fisheye] = 46;
    effects[ShaderManager::Effect::Whirl] = 50;
    effects[ShaderManager::Effect::Pixelate] = 25;
    effects[ShaderManager::Effect::Mosaic] = 30;
    EXPECT_CALL(target, texture()).WillOnce(Return(texture));
    EXPECT_CALL(target, costumeWidth()).WillOnce(Return(texture.width()));
    EXPECT_CALL(target, costumeHeight()).WillOnce(Return(texture.height()));
    EXPECT_CALL(target, graphicEffects()).WillOnce(ReturnRef(effects));
    targetPainter.paint(&painter);
    painter.endFrame();
    effects.clear();

    // Compare with reference image
    ASSERT_LE(fuzzyCompareImages(fbo.toImage(), QImage("shape_changing_effects.png")), 0.04);

    // Release
    fbo.release();
    refFbo.release();
}
