// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <scratchcpp/ispritehandler.h>

Q_MOC_INCLUDE("renderedtarget.h");

namespace scratchcpprender
{

class IRenderedTarget;

class SpriteModel
    : public QObject
    , public libscratchcpp::ISpriteHandler
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(IRenderedTarget *renderedTarget READ renderedTarget WRITE setRenderedTarget NOTIFY renderedTargetChanged)

    public:
        SpriteModel(QObject *parent = nullptr);

        void init(libscratchcpp::Sprite *sprite) override;
        void deinitClone() override;

        void onCloned(libscratchcpp::Sprite *clone) override;

        void onCostumeChanged(libscratchcpp::Costume *costume) override;

        void onVisibleChanged(bool visible) override;
        void onXChanged(double x) override;
        void onYChanged(double y) override;
        void onSizeChanged(double size) override;
        void onDirectionChanged(double direction) override;
        void onRotationStyleChanged(libscratchcpp::Sprite::RotationStyle rotationStyle) override;
        void onLayerOrderChanged(int layerOrder) override;

        void onGraphicsEffectChanged(libscratchcpp::IGraphicsEffect *effect, double value) override;
        void onGraphicsEffectsCleared() override;

        libscratchcpp::Rect boundingRect() const override;

        libscratchcpp::Sprite *sprite() const;

        IRenderedTarget *renderedTarget() const;
        void setRenderedTarget(IRenderedTarget *newRenderedTarget);

    signals:
        void renderedTargetChanged();
        void cloned(SpriteModel *cloneModel);
        void cloneDeleted(SpriteModel *clone);

    private:
        libscratchcpp::Sprite *m_sprite = nullptr;
        IRenderedTarget *m_renderedTarget = nullptr;
        SpriteModel *m_cloneRoot = nullptr;
};

} // namespace scratchcpprender
