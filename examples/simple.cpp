#include <iostream>
#include <picomath.hpp>

using PM = picomath::PicoMath;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    PM   pm;
    auto result = pm.parseExpression("2 * 4");
    if (result.isOk()) {
        std::cout << "Result: " << result.getResult() << std::endl;
    } else {
        std::cout << result.getError() << std::endl;
    }
}