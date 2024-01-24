// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QColor>
#include <scratchcpp/iblocksection.h>

namespace scratchcpprender
{

class SpriteModel;
class PenAttributes;

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

        static unsigned int clear(libscratchcpp::VirtualMachine *vm);
        static unsigned int penDown(libscratchcpp::VirtualMachine *vm);
        static unsigned int penUp(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenColorToColor(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenSizeBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenSizeTo(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenHueBy(libscratchcpp::VirtualMachine *vm);

    private:
        struct PenState
        {
                PenState(const QColor &color)
                {
                    QColor hsvColor = color.toHsv();
                    this->color = hsvColor.hue() * 100 / 360.0;
                    this->saturation = hsvColor.saturationF() * 100;
                    this->brightness = hsvColor.valueF() * 100;
                    this->transparency = 100 * (1 - hsvColor.alphaF());
                }

                double color = 0;
                double saturation = 0;
                double brightness = 0;
                double transparency = 0;
        };

        enum class ColorParam
        {
            COLOR
        };

        static SpriteModel *getSpriteModel(libscratchcpp::VirtualMachine *vm);
        static void setOrChangeColorParam(PenAttributes &penAttributes, ColorParam param, double value, bool change);
        static double wrapClamp(double n, double min, double max);
};

} // namespace scratchcpprender
