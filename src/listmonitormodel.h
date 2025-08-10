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
        Q_PROPERTY(size_t minIndex READ minIndex WRITE setMinIndex NOTIFY minIndexChanged FINAL);
        Q_PROPERTY(size_t maxIndex READ maxIndex WRITE setMaxIndex NOTIFY maxIndexChanged FINAL)

    public:
        ListMonitorModel(QObject *parent = nullptr);
        ListMonitorModel(libscratchcpp::IExtension *extension, QObject *parent = nullptr);

        void onValueChanged(const libscratchcpp::Value &value) override;

        Type type() const override;

        ListMonitorListModel *listModel() const;

        size_t minIndex() const;
        void setMinIndex(size_t newMinIndex);

        size_t maxIndex() const;
        void setMaxIndex(size_t newMaxIndex);

    signals:
        void colorChanged();
        void listModelChanged();
        void minIndexChanged();
        void maxIndexChanged();

    private:
        ListMonitorListModel *m_listModel = nullptr;
        size_t m_minIndex = 0;
        size_t m_maxIndex = 0;
};

} // namespace scratchcpprender
