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
        m_listModel->setList(list);
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
