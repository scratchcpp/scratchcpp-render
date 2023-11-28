#pragma once

#include <QCoreApplication>
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);
    std::filesystem::current_path(DATA_DIR);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
