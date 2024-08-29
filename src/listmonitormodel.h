// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QAbstractListModel>

#include "monitormodel.h"

Q_MOC_INCLUDE("listmonitorlistmodel.h")

namespace scratchcpprender
{

class ListMonitorListModel;

class ListMonitorModel : public MonitorModel
{
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(ListMonitorListModel *listModel READ listModel NOTIFY listModelChanged)

    public:
        ListMonitorModel(QObject *parent = nullptr);
        ListMonitorModel(libscratchcpp::IBlockSection *section, QObject *parent = nullptr);

        void onValueChanged(const libscratchcpp::VirtualMachine *vm) override;

        Type type() const override;

        ListMonitorListModel *listModel() const;

    signals:
        void colorChanged();
        void listModelChanged();

    private:
        ListMonitorListModel *m_listModel = nullptr;
};

} // namespace scratchcpprender
