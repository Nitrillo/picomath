#include <iostream>
#include <picomath.hpp>

using PM = picomath::PicoMath;

auto main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) -> int {
    PM pm;

    pm.addFunction("avg", [](size_t argc, const picomath::argument_list_t &args) -> picomath::Result {
        picomath::number_t result{0};
        size_t             i = 0;
        while (i < argc) {
            result += args[i];
            i++;
        }
        return result / static_cast<picomath::number_t>(argc);
    });

    auto result = pm.evalExpression("avg(20, 40, -23, pow(1.4, 5.3), 7, cos(pi))");
    if (result.isOk()) {
        std::cout << "Result: " << result.getResult() << std::endl;
    } else {
        std::cout << result.getError() << std::endl;
    }

    result = pm.evalExpression("avg(50, 100)");
    if (result.isOk()) {
        std::cout << "Result: " << result.getResult() << std::endl;
    } else {
        std::cout << result.getError() << std::endl;
    }

    pm.addFunction("rand", [](picomath::number_t input) -> picomath::number_t { return input * rand() / RAND_MAX; });
    result = pm.evalExpression("rand(100)");
    if (result.isOk()) {
        std::cout << "Result: " << result.getResult() << std::endl;
    } else {
        std::cout << result.getError() << std::endl;
    }
}