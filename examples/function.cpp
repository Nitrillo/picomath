#include <iostream>
#include <picomath.hpp>

using PM = picomath::PicoMath;

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    PM pm;

    pm.addFunction("avg") = [](size_t argc, const picomath::argument_list_t &args) -> picomath::Result {
        picomath::number_t result{0};
        size_t             i = 0;
        while (i < argc) {
            result += args[i];
            i++;
        }
        return result / static_cast<picomath::number_t>(argc);
    };

    auto result = pm.parseExpression("avg(20, 40, -23, pow(1.4, 5.3), 7, cos(pi))");
    if (result.isOk()) {
        std::cout << "Result: " << result.getResult() << std::endl;
    } else {
        std::cout << result.getError() << std::endl;
    }

    result = pm.parseExpression("avg(50, 100)");
    if (result.isOk()) {
        std::cout << "Result: " << result.getResult() << std::endl;
    } else {
        std::cout << result.getError() << std::endl;
    }
}