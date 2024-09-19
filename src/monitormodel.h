// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QColor>
#include <scratchcpp/imonitorhandler.h>

namespace libscratchcpp
{

class IExtension;

}

namespace scratchcpprender
{

class MonitorModel
    : public QObject
    , public libscratchcpp::IMonitorHandler
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QString name READ name NOTIFY nameChanged)
        Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
        Q_PROPERTY(Type type READ type NOTIFY typeChanged)
        Q_PROPERTY(QColor color READ color NOTIFY colorChanged)
        Q_PROPERTY(int x READ x NOTIFY xChanged)
        Q_PROPERTY(int y READ y NOTIFY yChanged)
        Q_PROPERTY(unsigned int width READ width WRITE setWidth NOTIFY widthChanged)
        Q_PROPERTY(unsigned int height READ height WRITE setHeight NOTIFY heightChanged)

    public:
        enum class Type
        {
            Invalid,
            Value,
            List
        };

        Q_ENUM(Type)

        MonitorModel(QObject *parent = nullptr);
        MonitorModel(libscratchcpp::IExtension *extension, QObject *parent = nullptr);

        void init(libscratchcpp::Monitor *monitor) override final;

        virtual void onValueChanged(const libscratchcpp::VirtualMachine *vm) override { }
        void onVisibleChanged(bool visible) override final;
        void onXChanged(int x) override final;
        void onYChanged(int y) override final;

        libscratchcpp::Monitor *monitor() const;

        QString name() const;

        bool visible() const;

        virtual Type type() const { return Type::Invalid; }

        const QColor &color() const;

        int x() const;

        int y() const;

        unsigned int width() const;
        void setWidth(unsigned int newWidth);

        unsigned int height() const;
        void setHeight(unsigned int newHeight);

    signals:
        void visibleChanged();
        void typeChanged();
        void colorChanged();
        void xChanged();
        void yChanged();
        void widthChanged();
        void heightChanged();
        void nameChanged();

    private:
        libscratchcpp::Monitor *m_monitor = nullptr;
        QColor m_color = Qt::green;
};

} // namespace scratchcpprender
