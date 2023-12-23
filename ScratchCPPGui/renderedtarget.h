// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <qnanoquickitem.h>
#include <QBuffer>
#include <QMutex>
#include <QtSvg/QSvgRenderer>

#include "irenderedtarget.h"

Q_MOC_INCLUDE("stagemodel.h");
Q_MOC_INCLUDE("spritemodel.h");

namespace scratchcppgui
{

class RenderedTarget : public IRenderedTarget
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(libscratchcpp::IEngine *engine READ engine WRITE setEngine NOTIFY engineChanged)
        Q_PROPERTY(StageModel *stageModel READ stageModel WRITE setStageModel NOTIFY stageModelChanged)
        Q_PROPERTY(SpriteModel *spriteModel READ spriteModel WRITE setSpriteModel NOTIFY spriteModelChanged)
        Q_PROPERTY(bool mirrorHorizontally READ mirrorHorizontally NOTIFY mirrorHorizontallyChanged)

    public:
        RenderedTarget(QNanoQuickItem *parent = nullptr);

        Q_INVOKABLE void loadProperties() override;
        void loadCostume(libscratchcpp::Costume *costume) override;
        Q_INVOKABLE void updateProperties() override;

        libscratchcpp::IEngine *engine() const override;
        void setEngine(libscratchcpp::IEngine *newEngine) override;

        StageModel *stageModel() const override;
        void setStageModel(StageModel *newStageModel) override;

        SpriteModel *spriteModel() const override;
        void setSpriteModel(SpriteModel *newSpriteModel) override;

        libscratchcpp::Target *scratchTarget() const override;

        qreal width() const override;
        void setWidth(qreal width) override;

        qreal height() const override;
        void setHeight(qreal height) override;

        QBuffer *bitmapBuffer() override;
        const QString &bitmapUniqueKey() const override;

        void lockCostume() override;
        void unlockCostume() override;

        bool mirrorHorizontally() const override;

        bool isSvg() const override;
        void paintSvg(QNanoPainter *painter) override;

        void updateHullPoints(QOpenGLFramebufferObject *fbo) override;
        const std::vector<QPointF> &hullPoints() const override;

        Q_INVOKABLE bool contains(const QPointF &point) const override;

    signals:
        void engineChanged();
        void stageModelChanged();
        void spriteModelChanged();

        void mirrorHorizontallyChanged();

    protected:
        QNanoQuickItemPainter *createItemPainter() const override;

    private:
        void updateCostumeData();
        void doLoadCostume();
        void calculateSize(libscratchcpp::Target *target, double costumeWidth, double costumeHeight);

        libscratchcpp::IEngine *m_engine = nullptr;
        libscratchcpp::Costume *m_costume = nullptr;
        StageModel *m_stageModel = nullptr;
        SpriteModel *m_spriteModel = nullptr;
        QSvgRenderer m_svgRenderer;
        QBuffer m_bitmapBuffer;
        QString m_bitmapUniqueKey;
        QMutex m_costumeMutex;
        QMutex mutex;
        bool m_loadCostume = false;
        bool m_costumeChanged = false;
        bool m_imageChanged = false;
        bool m_visible = true;
        double m_size = 1;
        double m_maxSize = 1;
        double m_x = 0;
        double m_y = 0;
        double m_z = 0;
        double m_width = 0;
        double m_height = 0;
        double m_rotation = 0;
        bool m_mirrorHorizontally = false;
        bool m_newMirrorHorizontally = false;
        double m_originX = 0;
        double m_originY = 0;
        qreal m_maximumWidth = std::numeric_limits<double>::infinity();
        qreal m_maximumHeight = std::numeric_limits<double>::infinity();
        std::vector<QPointF> m_hullPoints;
};

} // namespace scratchcppgui
