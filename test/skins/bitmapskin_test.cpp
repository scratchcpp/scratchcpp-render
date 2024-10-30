#include <scratchcpp/costume.h>
#include <bitmapskin.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

class BitmapSkinTest : public testing::Test
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

            Costume jpegCostume("", "", "");
            std::string costumeData = readFileStr("image.jpg");
            jpegCostume.setData(costumeData.size(), costumeData.data());
            m_jpegSkin = std::make_unique<BitmapSkin>(&jpegCostume);

            Costume pngCostume("", "", "");
            costumeData = readFileStr("image.png");
            pngCostume.setData(costumeData.size(), costumeData.data());
            m_pngSkin = std::make_unique<BitmapSkin>(&pngCostume);
        }

        void TearDown() override
        {
            ASSERT_EQ(m_context.surface(), &m_surface);
            emit m_context.aboutToBeDestroyed();
            m_jpegSkin.reset();
            m_pngSkin.reset();
            m_context.doneCurrent();
        }

        QOpenGLContext m_context;
        QOffscreenSurface m_surface;
        std::unique_ptr<Skin> m_jpegSkin;
        std::unique_ptr<Skin> m_pngSkin;
};

TEST_F(BitmapSkinTest, GetTexture)
{
    Texture texture = m_jpegSkin->getTexture(1);
    ASSERT_EQ(texture.width(), 4);
    ASSERT_EQ(texture.height(), 6);

    QBuffer jpegBuffer;
    texture.toImage().save(&jpegBuffer, "png");
    QFile jpegRef("jpeg_result.png");
    jpegRef.open(QFile::ReadOnly);
    jpegBuffer.open(QBuffer::ReadOnly);
    ASSERT_EQ(jpegBuffer.readAll(), jpegRef.readAll());

    texture = m_pngSkin->getTexture(1);
    ASSERT_EQ(texture.width(), 4);
    ASSERT_EQ(texture.height(), 6);

    QBuffer pngBuffer;
    texture.toImage().save(&pngBuffer, "png");
    QFile pngRef("png_result.png");
    pngRef.open(QFile::ReadOnly);
    pngBuffer.open(QBuffer::ReadOnly);
    ASSERT_EQ(pngBuffer.readAll(), pngRef.readAll());
}

TEST_F(BitmapSkinTest, GetTextureScale)
{
    ASSERT_EQ(m_jpegSkin->getTextureScale(Texture()), 1);
    ASSERT_EQ(m_pngSkin->getTextureScale(Texture()), 1);
}
