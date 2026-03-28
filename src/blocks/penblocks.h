#include <scratchcpp/iextension.h>

namespace scratchcpprender
{

class PenBlocks : public libscratchcpp::IExtension
{
    public:
        std::string name() const override;
        std::string description() const override;
        libscratchcpp::Rgb color() const override;

        void registerBlocks(libscratchcpp::IEngine *engine) override;

    private:
        static libscratchcpp::CompilerValue *compileClear(libscratchcpp::Compiler *compiler);
        static libscratchcpp::CompilerValue *compileStamp(libscratchcpp::Compiler *compiler);
        static libscratchcpp::CompilerValue *compilePenDown(libscratchcpp::Compiler *compiler);
        static libscratchcpp::CompilerValue *compilePenUp(libscratchcpp::Compiler *compiler);
        static libscratchcpp::CompilerValue *compileSetPenColorToColor(libscratchcpp::Compiler *compiler);
        static void compileSetOrChangePenColorParam(libscratchcpp::Compiler *compiler, bool change);
        static libscratchcpp::CompilerValue *compileChangePenColorParamBy(libscratchcpp::Compiler *compiler);
        static libscratchcpp::CompilerValue *compileSetPenColorParamTo(libscratchcpp::Compiler *compiler);
        static libscratchcpp::CompilerValue *compileChangePenSizeBy(libscratchcpp::Compiler *compiler);
        static libscratchcpp::CompilerValue *compileSetPenSizeTo(libscratchcpp::Compiler *compiler);
};

} // namespace scratchcpprender
