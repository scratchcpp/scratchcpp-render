#include <QtTest/QSignalSpy>
#include <scratchcpp/list.h>
#include <listmonitorlistmodel.h>

#include "../common.h"

using namespace scratchcpprender;
using namespace libscratchcpp;

TEST(ListMonitorListModelTest, Constructors)
{
    ListMonitorListModel model1;
    ListMonitorListModel model2(&model1);
    ASSERT_EQ(model2.parent(), &model1);
}

TEST(ListMonitorListModelTest, LoadData)
{
    ListMonitorListModel model;
    QSignalSpy dataChangedSpy(&model, &ListMonitorListModel::dataChanged);
    QSignalSpy aboutToResetSpy(&model, &ListMonitorListModel::modelAboutToBeReset);
    QSignalSpy resetSpy(&model, &ListMonitorListModel::modelReset);
    QSignalSpy aboutToInsertSpy(&model, &ListMonitorListModel::rowsAboutToBeInserted);
    QSignalSpy insertSpy(&model, &ListMonitorListModel::rowsInserted);
    QSignalSpy aboutToRemoveSpy(&model, &ListMonitorListModel::rowsAboutToBeRemoved);
    QSignalSpy removeSpy(&model, &ListMonitorListModel::rowsRemoved);

    List list1("", "");
    list1.append(1);
    list1.append(2);
    model.setList(&list1);
    ASSERT_TRUE(dataChangedSpy.empty());
    ASSERT_EQ(aboutToResetSpy.count(), 1);
    ASSERT_EQ(resetSpy.count(), 1);
    ASSERT_TRUE(aboutToInsertSpy.empty());
    ASSERT_TRUE(insertSpy.empty());
    ASSERT_TRUE(aboutToRemoveSpy.empty());
    ASSERT_TRUE(removeSpy.empty());

    model.setList(&list1);
    ASSERT_EQ(dataChangedSpy.count(), 2);
    ASSERT_EQ(aboutToResetSpy.count(), 1);
    ASSERT_EQ(resetSpy.count(), 1);
    ASSERT_TRUE(aboutToInsertSpy.empty());
    ASSERT_TRUE(insertSpy.empty());
    ASSERT_TRUE(aboutToRemoveSpy.empty());
    ASSERT_TRUE(removeSpy.empty());

    auto args = dataChangedSpy.at(0);
    QModelIndex arg1 = args.at(0).value<QModelIndex>();
    QModelIndex arg2 = args.at(1).value<QModelIndex>();
    ASSERT_EQ(arg1.row(), 0);
    ASSERT_EQ(arg1.column(), 0);
    ASSERT_EQ(arg2.row(), 0);
    ASSERT_EQ(arg2.column(), 0);
    ASSERT_TRUE(args.at(2).toList().isEmpty());

    args = dataChangedSpy.at(1);
    arg1 = args.at(0).value<QModelIndex>();
    arg2 = args.at(1).value<QModelIndex>();
    ASSERT_EQ(arg1.row(), 1);
    ASSERT_EQ(arg1.column(), 0);
    ASSERT_EQ(arg2.row(), 1);
    ASSERT_EQ(arg2.column(), 0);
    ASSERT_TRUE(args.at(2).toList().isEmpty());

    List list2("", "");
    model.setList(&list2);
    ASSERT_EQ(dataChangedSpy.count(), 2);
    ASSERT_EQ(aboutToResetSpy.count(), 2);
    ASSERT_EQ(resetSpy.count(), 2);
    ASSERT_TRUE(aboutToInsertSpy.empty());
    ASSERT_TRUE(insertSpy.empty());
    ASSERT_TRUE(aboutToRemoveSpy.empty());
    ASSERT_TRUE(removeSpy.empty());
}

TEST(ListMonitorListModelTest, AddRows)
{
    ListMonitorListModel model;
    QSignalSpy dataChangedSpy(&model, &ListMonitorListModel::dataChanged);
    QSignalSpy aboutToResetSpy(&model, &ListMonitorListModel::modelAboutToBeReset);
    QSignalSpy resetSpy(&model, &ListMonitorListModel::modelReset);
    QSignalSpy aboutToInsertSpy(&model, &ListMonitorListModel::rowsAboutToBeInserted);
    QSignalSpy insertSpy(&model, &ListMonitorListModel::rowsInserted);
    QSignalSpy aboutToRemoveSpy(&model, &ListMonitorListModel::rowsAboutToBeRemoved);
    QSignalSpy removeSpy(&model, &ListMonitorListModel::rowsRemoved);

    List list1("", "");
    list1.append(1);
    list1.append(2);
    model.setList(&list1);
    ASSERT_TRUE(dataChangedSpy.empty());
    ASSERT_EQ(aboutToResetSpy.count(), 1);
    ASSERT_EQ(resetSpy.count(), 1);
    ASSERT_TRUE(aboutToInsertSpy.empty());
    ASSERT_TRUE(insertSpy.empty());
    ASSERT_TRUE(aboutToRemoveSpy.empty());
    ASSERT_TRUE(removeSpy.empty());

    list1.append(9);
    list1.append(8);
    list1.append(7);
    model.setList(&list1);
    ASSERT_EQ(dataChangedSpy.count(), 2);
    ASSERT_EQ(aboutToResetSpy.count(), 1);
    ASSERT_EQ(resetSpy.count(), 1);
    ASSERT_EQ(aboutToInsertSpy.count(), 1);
    ASSERT_EQ(insertSpy.count(), 1);
    ASSERT_TRUE(aboutToRemoveSpy.empty());
    ASSERT_TRUE(removeSpy.empty());

    auto args = dataChangedSpy.at(0);
    QModelIndex arg1 = args.at(0).value<QModelIndex>();
    QModelIndex arg2 = args.at(1).value<QModelIndex>();
    ASSERT_EQ(arg1.row(), 0);
    ASSERT_EQ(arg1.column(), 0);
    ASSERT_EQ(arg2.row(), 0);
    ASSERT_EQ(arg2.column(), 0);
    ASSERT_TRUE(args.at(2).toList().isEmpty());

    args = dataChangedSpy.at(1);
    arg1 = args.at(0).value<QModelIndex>();
    arg2 = args.at(1).value<QModelIndex>();
    ASSERT_EQ(arg1.row(), 1);
    ASSERT_EQ(arg1.column(), 0);
    ASSERT_EQ(arg2.row(), 1);
    ASSERT_EQ(arg2.column(), 0);
    ASSERT_TRUE(args.at(2).toList().isEmpty());

    args = aboutToInsertSpy.at(0);
    arg1 = args.at(0).value<QModelIndex>();
    ASSERT_EQ(arg1, QModelIndex());
    ASSERT_EQ(args.at(1).toInt(), 2);
    ASSERT_EQ(args.at(2).toInt(), 4);
}

TEST(ListMonitorListModelTest, RemoveRows)
{
    ListMonitorListModel model;
    QSignalSpy dataChangedSpy(&model, &ListMonitorListModel::dataChanged);
    QSignalSpy aboutToResetSpy(&model, &ListMonitorListModel::modelAboutToBeReset);
    QSignalSpy resetSpy(&model, &ListMonitorListModel::modelReset);
    QSignalSpy aboutToInsertSpy(&model, &ListMonitorListModel::rowsAboutToBeInserted);
    QSignalSpy insertSpy(&model, &ListMonitorListModel::rowsInserted);
    QSignalSpy aboutToRemoveSpy(&model, &ListMonitorListModel::rowsAboutToBeRemoved);
    QSignalSpy removeSpy(&model, &ListMonitorListModel::rowsRemoved);

    List list1("", "");
    list1.append(1);
    list1.append(2);
    list1.append(3);
    model.setList(&list1);
    ASSERT_TRUE(dataChangedSpy.empty());
    ASSERT_EQ(aboutToResetSpy.count(), 1);
    ASSERT_EQ(resetSpy.count(), 1);
    ASSERT_TRUE(aboutToInsertSpy.empty());
    ASSERT_TRUE(insertSpy.empty());
    ASSERT_TRUE(aboutToRemoveSpy.empty());
    ASSERT_TRUE(removeSpy.empty());

    list1.removeAt(2);
    list1.removeAt(0);
    model.setList(&list1);
    ASSERT_EQ(dataChangedSpy.count(), 1);
    ASSERT_EQ(aboutToResetSpy.count(), 1);
    ASSERT_EQ(resetSpy.count(), 1);
    ASSERT_TRUE(aboutToInsertSpy.empty());
    ASSERT_TRUE(insertSpy.empty());
    ASSERT_EQ(aboutToRemoveSpy.count(), 1);
    ASSERT_EQ(removeSpy.count(), 1);

    auto args = dataChangedSpy.at(0);
    QModelIndex arg1 = args.at(0).value<QModelIndex>();
    QModelIndex arg2 = args.at(1).value<QModelIndex>();
    ASSERT_EQ(arg1.row(), 0);
    ASSERT_EQ(arg1.column(), 0);
    ASSERT_EQ(arg2.row(), 0);
    ASSERT_EQ(arg2.column(), 0);
    ASSERT_TRUE(args.at(2).toList().isEmpty());

    args = aboutToRemoveSpy.at(0);
    arg1 = args.at(0).value<QModelIndex>();
    ASSERT_EQ(arg1, QModelIndex());
    ASSERT_EQ(args.at(1).toInt(), 1);
    ASSERT_EQ(args.at(2).toInt(), 2);
}

TEST(ListMonitorListModelTest, RowCount)
{
    ListMonitorListModel model;
    List list("", "");
    list.append(1);
    list.append(2);
    list.append(3);
    model.setList(&list);
    ASSERT_EQ(model.rowCount(QModelIndex()), list.size());
}

TEST(ListMonitorListModelTest, Data)
{
    ListMonitorListModel model;
    List list("", "");
    list.append(1);
    list.append(2);
    list.append(3);
    model.setList(&list);
    ASSERT_EQ(model.data(model.index(0), 0).toString(), "1");
    ASSERT_EQ(model.data(model.index(1), 0).toString(), "2");
    ASSERT_EQ(model.data(model.index(2), 0).toString(), "3");
}

TEST(ListMonitorListModelTest, RoleNames)
{
    ListMonitorListModel model;
    QHash<int, QByteArray> names({ { 0, "value" } });
    ASSERT_EQ(model.roleNames(), names);
}
