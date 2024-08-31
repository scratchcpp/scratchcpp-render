#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QSignalSpy>
#include <textbubbleshape.h>

#include "../common.h"

using namespace scratchcpprender;

class TextBubbleShapeTest : public testing::Test
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

TEST_F(TextBubbleShapeTest, Constructors)
{
    TextBubbleShape bubble1;
    TextBubbleShape bubble2(&bubble1);
    ASSERT_EQ(bubble2.parent(), &bubble1);
    ASSERT_EQ(bubble2.parentItem(), &bubble1);
}

TEST_F(TextBubbleShapeTest, Type)
{
    TextBubbleShape bubble;
    QSignalSpy spy(&bubble, &TextBubbleShape::typeChanged);
    ASSERT_EQ(bubble.type(), TextBubbleShape::Type::Say);

    bubble.setType(TextBubbleShape::Type::Think);
    ASSERT_EQ(bubble.type(), TextBubbleShape::Type::Think);
    ASSERT_EQ(spy.count(), 1);

    bubble.setType(TextBubbleShape::Type::Think);
    ASSERT_EQ(bubble.type(), TextBubbleShape::Type::Think);
    ASSERT_EQ(spy.count(), 1);

    bubble.setType(TextBubbleShape::Type::Say);
    ASSERT_EQ(bubble.type(), TextBubbleShape::Type::Say);
    ASSERT_EQ(spy.count(), 2);
}

TEST_F(TextBubbleShapeTest, OnSpriteRight)
{
    TextBubbleShape bubble;
    QSignalSpy spy(&bubble, &TextBubbleShape::onSpriteRightChanged);
    ASSERT_TRUE(bubble.onSpriteRight());

    bubble.setOnSpriteRight(false);
    ASSERT_FALSE(bubble.onSpriteRight());
    ASSERT_EQ(spy.count(), 1);

    bubble.setOnSpriteRight(false);
    ASSERT_FALSE(bubble.onSpriteRight());
    ASSERT_EQ(spy.count(), 1);

    bubble.setOnSpriteRight(true);
    ASSERT_TRUE(bubble.onSpriteRight());
    ASSERT_EQ(spy.count(), 2);
}

TEST_F(TextBubbleShapeTest, StageScale)
{
    TextBubbleShape bubble;
    QSignalSpy spy(&bubble, &TextBubbleShape::stageScaleChanged);
    ASSERT_EQ(bubble.stageScale(), 1);

    bubble.setStageScale(6.48);
    ASSERT_EQ(bubble.stageScale(), 6.48);
    ASSERT_EQ(spy.count(), 1);
}

TEST_F(TextBubbleShapeTest, NativeWidth)
{
    TextBubbleShape bubble;
    QSignalSpy spy(&bubble, &TextBubbleShape::nativeWidthChanged);
    ASSERT_EQ(bubble.nativeWidth(), 0);

    bubble.setNativeWidth(48.1);
    ASSERT_EQ(bubble.nativeWidth(), 48.1);
    ASSERT_EQ(spy.count(), 1);
}

TEST_F(TextBubbleShapeTest, NativeHeight)
{
    TextBubbleShape bubble;
    QSignalSpy spy(&bubble, &TextBubbleShape::nativeHeightChanged);
    ASSERT_EQ(bubble.nativeHeight(), 0);

    bubble.setNativeHeight(87.5);
    ASSERT_EQ(bubble.nativeHeight(), 87.5);
    ASSERT_EQ(spy.count(), 1);
}
