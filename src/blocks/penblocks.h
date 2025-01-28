// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QColor>
#include <scratchcpp/iextension.h>

namespace scratchcpprender
{

class SpriteModel;
class PenState;

class PenBlocks : public libscratchcpp::IExtension
{
    public:
        enum Inputs
        {
            COLOR,
            COLOR_PARAM,
            VALUE,
            SIZE,
            SHADE,
            HUE
        };

        std::string name() const override;
        std::string description() const override;
        libscratchcpp::Rgb color() const override;

        void registerBlocks(libscratchcpp::IEngine *engine) override;

        static void compileClear(libscratchcpp::Compiler *compiler);
        static void compileStamp(libscratchcpp::Compiler *compiler);
        static void compilePenDown(libscratchcpp::Compiler *compiler);
        static void compilePenUp(libscratchcpp::Compiler *compiler);
        static void compileSetPenColorToColor(libscratchcpp::Compiler *compiler);
        static void compileChangePenColorParamBy(libscratchcpp::Compiler *compiler);
        static void compileSetPenColorParamTo(libscratchcpp::Compiler *compiler);
        static void compileChangePenSizeBy(libscratchcpp::Compiler *compiler);
        static void compileSetPenSizeTo(libscratchcpp::Compiler *compiler);
        static void compileChangePenShadeBy(libscratchcpp::Compiler *compiler);
        static void compileSetPenShadeToNumber(libscratchcpp::Compiler *compiler);
        static void compileChangePenHueBy(libscratchcpp::Compiler *compiler);
        static void compileSetPenHueToNumber(libscratchcpp::Compiler *compiler);

        static unsigned int clear(libscratchcpp::VirtualMachine *vm);
        static unsigned int stamp(libscratchcpp::VirtualMachine *vm);
        static unsigned int penDown(libscratchcpp::VirtualMachine *vm);
        static unsigned int penUp(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenColorToColor(libscratchcpp::VirtualMachine *vm);

        static unsigned int changePenColorParamBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenColorBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenSaturationBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenBrightnessBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenTransparencyBy(libscratchcpp::VirtualMachine *vm);

        static unsigned int setPenColorParamTo(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenColorTo(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenSaturationTo(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenBrightnessTo(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenTransparencyTo(libscratchcpp::VirtualMachine *vm);

        static unsigned int changePenSizeBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenSizeTo(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenShadeBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenShadeToNumber(libscratchcpp::VirtualMachine *vm);
        static unsigned int changePenHueBy(libscratchcpp::VirtualMachine *vm);
        static unsigned int setPenHueToNumber(libscratchcpp::VirtualMachine *vm);

    private:
        enum class ColorParam
        {
            COLOR,
            SATURATION,
            BRIGHTNESS,
            TRANSPARENCY
        };

        static SpriteModel *getSpriteModel(libscratchcpp::VirtualMachine *vm);
        static void setOrChangeColorParam(ColorParam param, double value, PenState &penState, bool change);
        static void setPenShade(int shade, PenState &penState);
        static void legacyUpdatePenColor(PenState &penState);
        static double wrapClamp(double n, double min, double max);
        static QRgb mixRgb(QRgb rgb0, QRgb rgb1, double fraction1);

        static const std::unordered_map<std::string, ColorParam> COLOR_PARAM_MAP;
};

} // namespace scratchcpprender
