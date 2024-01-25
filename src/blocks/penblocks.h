// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <scratchcpp/iblocksection.h>

class QColor;

namespace scratchcpprender
{

class SpriteModel;
class PenState;

class PenBlocks : public libscratchcpp::IBlockSection
{
    public:
        enum Inputs
        {
            COLOR,
            SIZE,
            HUE
        };

        std::string name() const override;

        void registerBlocks(libscratchcpp::IEngine *engine) override;

        static void compileClear(libscratchcpp::Compiler *compiler);
        static void compilePenDown(libscratchcpp::Compiler *compiler);
        static void compilePenUp(libscratchcpp::Compiler *compiler);
        static void compileSetPenColorToColor(libscratchcpp::Compiler *compiler);
        static void compileChangePenSizeBy(libscratchcpp::Compiler *compiler);
        static void compileSetPenSizeTo(libscratchcpp::Compiler *compiler);
        static void compileChangePenHueBy(libscratchcpp::Compiler *compiler);
        static void compileSetPenHueToNumber(libscratchcpp::Compiler *compiler);

        static unsigned int clear(libscratchcpp::VirtualMachine *vm);
        static unsigned int penDown(libscratchcpp::VirtualMachine *vm);
        static unsigned int penUp(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenColorToColor(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenSizeBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenSizeTo(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenHueBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenHueToNumber(libscratchcpp::VirtualMachine *vm);

    private:
        enum class ColorParam
        {
            COLOR
        };

        static SpriteModel *getSpriteModel(libscratchcpp::VirtualMachine *vm);
        static void setOrChangeColorParam(ColorParam param, double value, PenState &penState, bool change, bool legacy = false);
        static double wrapClamp(double n, double min, double max);
        static QColor mixRgb(const QColor &rgb0, const QColor &rgb1, double fraction1);
};

} // namespace scratchcpprender
