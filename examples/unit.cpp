#include <iostream>
#include <picomath.hpp>

using PM = picomath::PicoMath;

auto main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) -> int {
    PM pm;

    // Device pixel ratio: each virtual pixel = 2 pixels in screen
    auto &devicePixelRatio = pm.addUnit("px");
    devicePixelRatio       = 2.0;

    // Container size 350px
    auto &percentage = pm.addUnit("%");
    percentage       = 350.0 * devicePixelRatio / 100.0;

    // Calculate inner width with a 5px padding in each side
    auto result = pm.evalExpression("100% -5px * 2");
    if (result.isOk()) {
        std::cout << "Result: " << result.getResult() << std::endl;
    } else {
        std::cout << result.getError() << std::endl;
    }

    // Calculate x position of 100px element centered in container
    result = pm.evalExpression("50% - 100px/2");
    if (result.isOk()) {
        std::cout << "Result: " << result.getResult() << std::endl;
    } else {
        std::cout << result.getError() << std::endl;
    }
}