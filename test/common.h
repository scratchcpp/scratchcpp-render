#pragma once

#include <QGuiApplication>
#include <QImage>
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

std::string readFileStr(const std::string &fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cout << "Failed to open " + fileName << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

double fuzzyCompareImages(const QImage &a, const QImage &b)
{
    if (a.size() != b.size())
        return 1;

    int x, y, c = 0;

    for (y = 0; y < a.height(); y++) {
        for (x = 0; x < a.width(); x++) {
            if (a.pixelColor(x, y).rgba() != b.pixelColor(x, y).rgba())
                c++;
        }
    }

    return c / static_cast<double>((a.width() * a.height()));
}

int main(int argc, char **argv)
{
    QGuiApplication a(argc, argv);
    std::filesystem::current_path(DATA_DIR);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
