// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <scratchcpp/ispritehandler.h>

Q_MOC_INCLUDE("renderedtarget.h");

namespace scratchcppgui
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

        void onCloned(libscratchcpp::Sprite *clone) override;

        void onCostumeChanged(libscratchcpp::Costume *costume) override;

        void onVisibleChanged(bool visible) override;
        void onXChanged(double x) override;
        void onYChanged(double y) override;
        void onSizeChanged(double size) override;
        void onDirectionChanged(double direction) override;
        void onRotationStyleChanged(libscratchcpp::Sprite::RotationStyle rotationStyle) override;

        libscratchcpp::Sprite *sprite() const;

        IRenderedTarget *renderedTarget() const;
        void setRenderedTarget(IRenderedTarget *newRenderedTarget);

    signals:
        void renderedTargetChanged();

    private:
        libscratchcpp::Sprite *m_sprite = nullptr;
        IRenderedTarget *m_renderedTarget = nullptr;
};

} // namespace scratchcppgui
