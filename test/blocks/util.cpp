#include <scratchcpp/iengine.h>
#include <scratchcpp/compiler.h>
#include <scratchcpp/string_functions.h>

#include "util.h"

using namespace libscratchcpp;

static bool conditionReturnValue = false;

void registerBlocks(IEngine *engine, IExtension *extension)
{
    engine->addCompileFunction(extension, "test_const_string", [](Compiler *compiler) -> CompilerValue * {
        auto input = compiler->addInput("STRING");
        return compiler->addFunctionCall("test_const_string", Compiler::StaticType::String, { Compiler::StaticType::String }, { input });
    });
}

extern "C" void test_const_string(StringPtr *ret, const StringPtr *str)
{
    string_assign(ret, str);
}
