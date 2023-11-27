// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <scratchcpp/iimageformat.h>
#include <scratchcpp/iengine.h>
#include <qnanoquickitem.h>
#include <qnanoimage.h>
#include <QBuffer>
#include <QMutex>

Q_MOC_INCLUDE("stagemodel.h");
Q_MOC_INCLUDE("spritemodel.h");

namespace libscratchcpp
{
class Costume;
}

namespace scratchcppgui
{

class StageModel;
class SpriteModel;

class RenderedTarget : public QNanoQuickItem
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(libscratchcpp::IEngine *engine READ engine WRITE setEngine NOTIFY engineChanged)
        Q_PROPERTY(StageModel *stageModel READ stageModel WRITE setStageModel NOTIFY stageModelChanged)
        Q_PROPERTY(SpriteModel *spriteModel READ spriteModel WRITE setSpriteModel NOTIFY spriteModelChanged)
        Q_PROPERTY(bool mirrorHorizontally READ mirrorHorizontally NOTIFY mirrorHorizontallyChanged)

    public:
        RenderedTarget(QNanoQuickItem *parent = nullptr);
        ~RenderedTarget();

        Q_INVOKABLE void loadProperties();
        void loadCostume(libscratchcpp::Costume *costume);
        Q_INVOKABLE void updateProperties();

        libscratchcpp::IEngine *engine() const;
        void setEngine(libscratchcpp::IEngine *newEngine);

        StageModel *stageModel() const;
        void setStageModel(StageModel *newStageModel);

        SpriteModel *spriteModel() const;
        void setSpriteModel(SpriteModel *newSpriteModel);

        libscratchcpp::Target *scratchTarget() const;

        unsigned char *svgBitmap() const;
        QBuffer *bitmapBuffer();
        const QString &bitmapUniqueKey() const;

        void lockCostume();
        void unlockCostume();

        bool mirrorHorizontally() const;

    signals:
        void engineChanged();
        void stageModelChanged();
        void spriteModelChanged();

        void mirrorHorizontallyChanged();

    protected:
        QNanoQuickItemPainter *createItemPainter() const override;

    private:
        void calculateSize(libscratchcpp::Target *target, double costumeWidth, double costumeHeight);

        libscratchcpp::IEngine *m_engine = nullptr;
        libscratchcpp::Costume *m_costume = nullptr;
        StageModel *m_stageModel = nullptr;
        SpriteModel *m_spriteModel = nullptr;
        unsigned char *m_svgBitmap = nullptr;
        QBuffer m_bitmapBuffer;
        QString m_bitmapUniqueKey;
        QMutex m_costumeMutex;
        QMutex mutex;
        bool m_imageChanged = false;
        bool m_visible = true;
        double m_width = 0;
        double m_height = 0;
        double m_x = 0;
        double m_y = 0;
        double m_z = 0;
        double m_rotation = 0;
        bool m_mirrorHorizontally = false;
        bool m_newMirrorHorizontally = false;
        double m_originX = 0;
        double m_originY = 0;
};

} // namespace scratchcppgui
