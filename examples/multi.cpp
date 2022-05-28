#include <iostream>
#include <picomath.hpp>

using PM = picomath::PicoMath;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    PM   pm;
    auto result = pm.parseMultiExpression("2 * 4, 2, .5 * .5 * .5, (100.003 * 10), pow(2,8)");
    do {
        if (result.isOk()) {
            std::cout << "Result: " << result.getResult() << std::endl;
        } else {
            std::cout << result.getError() << std::endl;
        }
    } while (pm.parseNext(&result));
}