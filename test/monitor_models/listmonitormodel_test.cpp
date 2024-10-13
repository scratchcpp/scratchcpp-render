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
    QSignalSpy dataChangedSpy(listModel, &ListMonitorListModel::dataChanged);
    VirtualMachine vm;

    List list1("", "");
    list1.append(1);
    list1.append(2);

    List list2("", "");
    list2.append(1);
    list2.append(2);
    list2.append(3);
    list2.append(4);

    List list3("", "");
    list3.append(1);
    list3.append(2);
    list3.append(3);

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
    vm.addReturnValue(2);
    model.setMinIndex(0);
    model.setMaxIndex(3);
    model.onValueChanged(&vm);
    ASSERT_EQ(listModel->rowCount(QModelIndex()), 3);
    ASSERT_EQ(dataChangedSpy.count(), 4);

    dataChangedSpy.clear();
    vm.reset();
    vm.addReturnValue(2);
    model.setMinIndex(1);
    model.setMaxIndex(2);
    model.onValueChanged(&vm);
    ASSERT_EQ(listModel->rowCount(QModelIndex()), 3);
    ASSERT_EQ(dataChangedSpy.count(), 2);

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

TEST(ListMonitorModelTest, MinIndex)
{
    ListMonitorModel model;
    ASSERT_EQ(model.minIndex(), 0);

    model.setMinIndex(2);
    ASSERT_EQ(model.minIndex(), 2);
}

TEST(ListMonitorModelTest, MaxIndex)
{
    ListMonitorModel model;
    ASSERT_EQ(model.maxIndex(), 0);

    model.setMaxIndex(17);
    ASSERT_EQ(model.maxIndex(), 17);
}
