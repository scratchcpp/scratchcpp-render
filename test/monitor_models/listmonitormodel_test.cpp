#include <QtTest/QSignalSpy>
#include <scratchcpp/virtualmachine.h>
#include <scratchcpp/list.h>
#include <listmonitormodel.h>
#include <listmonitorlistmodel.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

TEST(ListMonitorModelTest, Constructors)
{
    {
        ListMonitorModel model1;
        ListMonitorModel model2(&model1);
        ASSERT_EQ(model2.parent(), &model1);
    }

    {
        ListMonitorModel model1;
        ListMonitorModel model2(nullptr, &model1);
        ASSERT_EQ(model2.parent(), &model1);
    }
}

TEST(ListMonitorModelTest, OnValueChanged)
{
    ListMonitorModel model;
    ListMonitorListModel *listModel = model.listModel();
    VirtualMachine vm;

    List list1("", "");
    list1.push_back(1);
    list1.push_back(2);

    List list2("", "");
    list2.push_back(1);
    list2.push_back(2);
    list2.push_back(3);
    list2.push_back(4);

    List list3("", "");
    list3.push_back(1);
    list3.push_back(2);
    list3.push_back(3);

    List *lists[] = { &list1, &list2, &list3 };
    vm.setLists(lists);

    vm.addReturnValue(1);
    model.onValueChanged(&vm);
    ASSERT_EQ(listModel->rowCount(QModelIndex()), 4);

    vm.reset();
    vm.addReturnValue(2);
    model.onValueChanged(&vm);
    ASSERT_EQ(listModel->rowCount(QModelIndex()), 3);

    vm.reset();
    vm.addReturnValue(0);
    model.onValueChanged(&vm);
    ASSERT_EQ(listModel->rowCount(QModelIndex()), 2);
}

TEST(ListMonitorModelTest, Type)
{
    ListMonitorModel model;
    ASSERT_EQ(model.type(), MonitorModel::Type::List);
}
