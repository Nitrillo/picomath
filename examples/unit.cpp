#include <iostream>
#include <picomath.hpp>

using PM = picomath::PicoMath;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    PM pm;

    // Device pixel ratio: each virtual pixel = 2 pixels in screen
    auto &devicePixelRatio = pm.addUnit("px");
    devicePixelRatio       = 2.0;

    // Container size 350px
    pm.addUnit("%") = 350.0 * devicePixelRatio / 100.0;

    // Calculate inner width with a 5px padding in each side
    auto result = pm.parseExpression("100% -5px * 2");
    if (result.isOk()) {
        std::cout << "Result: " << result.getResult() << std::endl;
    } else {
        std::cout << result.getError() << std::endl;
    }

    // Calculate x position of 100px element centered in container
    result = pm.parseExpression("50% - 100px/2");
    if (result.isOk()) {
        std::cout << "Result: " << result.getResult() << std::endl;
    } else {
        std::cout << result.getError() << std::endl;
    }
}