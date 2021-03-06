#include <iostream>
#include <picomath.hpp>

using PM = picomath::PicoMath;

auto main() -> int {
    PM   pm;
    auto result = pm.evalExpression("2 * 4");
    if (result.isOk()) {
        std::cout << "Result: " << result.getResult() << std::endl;
    } else {
        std::cout << result.getError() << std::endl;
    }
}