#include <iostream>
#include <picomath.hpp>

using PM = picomath::PicoMath;

auto main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) -> int {
    PM               pm;
    auto             expression = pm.evalMultiExpression("2 * 4, 2, .5 * .5 * .5, (100.003 * 10), pow(2,8)");
    picomath::Result result;
    while (expression.evalNext(&result)) {
        if (result.isOk()) {
            std::cout << "Result: " << result.getResult() << std::endl;
        } else {
            std::cout << result.getError() << std::endl;
        }
    };
}