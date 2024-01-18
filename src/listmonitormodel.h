// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QAbstractListModel>
#include <QColor>

#include "monitormodel.h"

Q_MOC_INCLUDE("listmonitorlistmodel.h")

namespace libscratchcpp
{

class IBlockSection;

}

namespace scratchcpprender
{

class ListMonitorListModel;

class ListMonitorModel : public MonitorModel
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QColor color READ color NOTIFY colorChanged)
        Q_PROPERTY(ListMonitorListModel *listModel READ listModel NOTIFY listModelChanged)

    public:
        ListMonitorModel(QObject *parent = nullptr);
        ListMonitorModel(libscratchcpp::IBlockSection *section, QObject *parent = nullptr);

        void onValueChanged(const libscratchcpp::VirtualMachine *vm) override;

        Type type() const override;

        const QColor &color() const;

        ListMonitorListModel *listModel() const;

    signals:
        void colorChanged();
        void listModelChanged();

    private:
        QColor m_color = Qt::green;
        ListMonitorListModel *m_listModel = nullptr;
};

} // namespace scratchcpprender
