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
};

} // namespace scratchcpprender
