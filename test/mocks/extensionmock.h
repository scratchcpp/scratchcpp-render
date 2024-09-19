#pragma once

#include <scratchcpp/iextension.h>
#include <gmock/gmock.h>

using namespace libscratchcpp;

namespace scratchcpprender
{

class ExtensionMock : public IExtension
{
    public:
        MOCK_METHOD(std::string, name, (), (const, override));
        MOCK_METHOD(std::string, description, (), (const, override));

        MOCK_METHOD(void, registerBlocks, (IEngine * engine), (override));
        MOCK_METHOD(void, onInit, (IEngine * engine), (override));
};

} // namespace scratchcpprender
