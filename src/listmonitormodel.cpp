// SPDX-License-Identifier: LGPL-3.0-or-later

#include <scratchcpp/virtualmachine.h>

#include "listmonitormodel.h"
#include "listmonitorlistmodel.h"

using namespace scratchcpprender;

ListMonitorModel::ListMonitorModel(QObject *parent) :
    ListMonitorModel(nullptr, parent)
{
}

ListMonitorModel::ListMonitorModel(libscratchcpp::IExtension *extension, QObject *parent) :
    MonitorModel(extension, parent)
{
    m_listModel = new ListMonitorListModel(this);
}

void ListMonitorModel::onValueChanged(const libscratchcpp::VirtualMachine *vm)
{
    if (vm->registerCount() == 1) {
        long index = vm->getInput(0, 1)->toLong();
        libscratchcpp::List *list = vm->lists()[index];
        m_listModel->setList(list, m_minIndex, m_maxIndex);
    }
}

MonitorModel::Type ListMonitorModel::type() const
{
    return Type::List;
}

ListMonitorListModel *ListMonitorModel::listModel() const
{
    return m_listModel;
}

size_t ListMonitorModel::minIndex() const
{
    return m_minIndex;
}

void ListMonitorModel::setMinIndex(size_t newMinIndex)
{
    if (m_minIndex == newMinIndex)
        return;

    m_minIndex = newMinIndex;
    emit minIndexChanged();
}

size_t ListMonitorModel::maxIndex() const
{
    return m_maxIndex;
}

void ListMonitorModel::setMaxIndex(size_t newMaxIndex)
{
    if (m_maxIndex == newMaxIndex)
        return;

    m_maxIndex = newMaxIndex;
    emit maxIndexChanged();
}
