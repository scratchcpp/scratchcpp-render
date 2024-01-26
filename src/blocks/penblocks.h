// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QColor>
#include <scratchcpp/iblocksection.h>

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
            SHADE,
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
        static void compileChangePenShadeBy(libscratchcpp::Compiler *compiler);
        static void compileChangePenHueBy(libscratchcpp::Compiler *compiler);
        static void compileSetPenHueToNumber(libscratchcpp::Compiler *compiler);

        static unsigned int clear(libscratchcpp::VirtualMachine *vm);
        static unsigned int penDown(libscratchcpp::VirtualMachine *vm);
        static unsigned int penUp(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenColorToColor(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenSizeBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenSizeTo(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenShadeBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenHueBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenHueToNumber(libscratchcpp::VirtualMachine *vm);

    private:
        enum class ColorParam
        {
            COLOR
        };

        static SpriteModel *getSpriteModel(libscratchcpp::VirtualMachine *vm);
        static void setOrChangeColorParam(ColorParam param, double value, PenState &penState, bool change);
        static void setPenShade(int shade, PenState &penState);
        static void legacyUpdatePenColor(PenState &penState);
        static double wrapClamp(double n, double min, double max);
        static QRgb mixRgb(QRgb rgb0, QRgb rgb1, double fraction1);
};

} // namespace scratchcpprender
