#include <scratchcpp/costume.h>
#include <svgskin.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

class SVGSkinTest : public testing::Test
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

            Costume costume("", "", "");
            std::string costumeData = readFileStr("image.svg");
            char *data = (char *)malloc((costumeData.size() + 1) * sizeof(char));
            memcpy(data, costumeData.c_str(), (costumeData.size() + 1) * sizeof(char));
            costume.setData(costumeData.size(), data);
            m_skin = std::make_unique<SVGSkin>(&costume);
        }

        void TearDown() override
        {
            ASSERT_EQ(m_context.surface(), &m_surface);
            emit m_context.aboutToBeDestroyed();
            m_skin.reset();
            m_context.doneCurrent();
        }

        QOpenGLContext m_context;
        QOffscreenSurface m_surface;
        std::unique_ptr<Skin> m_skin;
};

TEST_F(SVGSkinTest, Textures)
{
    static const int INDEX_OFFSET = 8;

    for (int i = 0; i <= 18; i++) {
        double scale = std::pow(2, i - INDEX_OFFSET);
        Texture texture = m_skin->getTexture(scale);
        int dimension = static_cast<int>(13 * scale);
        ASSERT_TRUE(texture.isValid() || dimension == 0);

        if (!texture.isValid())
            continue;

        if (i > 15) {
            ASSERT_EQ(texture.width(), 1664);
            ASSERT_EQ(texture.height(), 1664);
            ASSERT_EQ(m_skin->getTextureScale(texture), 128);
        } else {
            ASSERT_EQ(texture.width(), dimension);
            ASSERT_EQ(texture.height(), dimension);
            ASSERT_EQ(m_skin->getTextureScale(texture), scale);
        }

        QBuffer buffer;
        texture.toImage().save(&buffer, "png");
        QFile ref("svg_texture_results/" + QString::number(std::min(i, 15)) + ".png");
        ref.open(QFile::ReadOnly);
        buffer.open(QBuffer::ReadOnly);
        ASSERT_EQ(buffer.readAll(), ref.readAll());
    }
}
