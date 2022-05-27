#ifndef PICOMATH_HPP
#define PICOMATH_HPP

#include <cmath>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace picomath {

#ifndef PM_MAX_ARGUMENTS
#define PM_MAX_ARGUMENTS 4
#endif

class Result;

#ifdef PM_USE_FLOAT
using number_t = float;
#else
using number_t = double;
#endif
using argument_list_t   = std::array<number_t, PM_MAX_ARGUMENTS>;
using Error             = std::string;
using custom_function_t = std::function<Result(size_t argc, const argument_list_t &list)>;

class Result {
    friend class PicoMath;

    number_t               result;
    std::unique_ptr<Error> error;

  public:
    Result(number_t result) : result(result) { // NOLINT
    }

    Result(std::string &&description) : result(0) { // NOLINT
        error = std::make_unique<Error>(std::move(description));
    }

    [[nodiscard]] auto isError() const -> bool {
        return error != nullptr;
    }

    [[nodiscard]] auto isOk() const -> bool {
        return error == nullptr;
    }

    [[nodiscard]] auto getError() const -> const char * {
        return error->c_str();
    }

    [[nodiscard]] auto getResult() const -> number_t {
        return result;
    }
};

#define PM_FUNCTION_1(fun)                                                                                             \
    [](size_t argc, const argument_list_t &args) -> Result {                                                           \
        if (argc != 1) {                                                                                               \
            return {"One argument needed"};                                                                            \
        }                                                                                                              \
        return fun(args[0]);                                                                                           \
    };

#define PM_FUNCTION_2(fun)                                                                                             \
    [](size_t argc, const argument_list_t &args) -> Result {                                                           \
        if (argc != 2) {                                                                                               \
            return {"Two arguments needed"};                                                                           \
        }                                                                                                              \
        return fun(args[0], args[1]);                                                                                  \
    };

class PicoMath {

    const char *                                          originalStr{};
    const char *                                          str{};
    std::map<std::string, number_t, std::less<>>          variables;
    std::map<std::string, number_t, std::less<>>          units;
    std::map<std::string, custom_function_t, std::less<>> functions;

  public:
    auto addVariable(const std::string &name) -> number_t & {
        return variables[name];
    }

    auto addUnit(const std::string &name) -> number_t & {
        return units[name];
    }

    auto addFunction(const std::string &name) -> custom_function_t & {
        return functions[name];
    }

    auto parseMultiExpression(const char *expression) -> Result {
        originalStr = str = expression;
        return parseExpression();
    }

    auto parseNext(Result &result) -> bool {
        consumeSpace();
        if (peek() == 0) {
            return false;
        }
        if (peek() == ',') {
            consume();
            consumeSpace();
        }
        result = parseExpression();
        return true;
    }

    auto parseExpression(const char *expression) -> Result {
        originalStr = str = expression;
        Result ret        = parseExpression();
        consumeSpace();
        if (peek() == 0) {
            return ret;
        }
        std::string err = "Invalid characters after expression:";
        return {err + str};
    }

    PicoMath() {
        // Constants
        addVariable("pi") = static_cast<number_t>(M_PI);
        addVariable("e")  = static_cast<number_t>(M_E);

        // Built-in functions
        addFunction("abs")   = PM_FUNCTION_1(std::abs);
        addFunction("ceil")  = PM_FUNCTION_1(std::ceil);
        addFunction("floor") = PM_FUNCTION_1(std::floor);
        addFunction("round") = PM_FUNCTION_1(std::round);
        addFunction("round") = PM_FUNCTION_1(std::round);
        addFunction("ln")    = PM_FUNCTION_1(std::log);
        addFunction("log")   = PM_FUNCTION_1(std::log10);
        addFunction("cos")   = PM_FUNCTION_1(std::cos);
        addFunction("sin")   = PM_FUNCTION_1(std::sin);
        addFunction("acos")  = PM_FUNCTION_1(std::acos);
        addFunction("asin")  = PM_FUNCTION_1(std::asin);
        addFunction("cosh")  = PM_FUNCTION_1(std::cosh);
        addFunction("sinh")  = PM_FUNCTION_1(std::sinh);
        addFunction("tan")   = PM_FUNCTION_1(std::tan);
        addFunction("tanh")  = PM_FUNCTION_1(std::tanh);
        addFunction("sqrt")  = PM_FUNCTION_1(std::sqrt);
        addFunction("atan2") = PM_FUNCTION_2(std::atan2);
        addFunction("pow")   = PM_FUNCTION_2(std::pow);
        addFunction("min")   = [](size_t argc, const argument_list_t &args) -> Result {
            number_t result = std::numeric_limits<number_t>::max();
            size_t   i      = 0;
            while (i < argc) {
                result = std::min(args[i], result);
                i++;
            }
            return result;
        };
        addFunction("max") = [](size_t argc, const argument_list_t &args) -> Result {
            number_t result = std::numeric_limits<number_t>::min();
            size_t   i      = 0;
            while (i < argc) {
                result = std::max(args[i], result);
                i++;
            }
            return result;
        };
    }

  private:
    auto generateError(const char *error, std::string_view identifier = {}) const -> Result {
        std::string out = "In character " + std::to_string(static_cast<int>(str - originalStr - 1)) + ": ";
        out += error;
        if (identifier.empty()) {
            out += " found: ";
            if (peek() == 0) {
                out += "End of string";
            } else {
                out += str;
            }
        } else {
            out += ' ';
            out += '`';
            out += identifier;
            out += '`';
        }
        return out;
    }

    auto parseExpression() -> Result {
        consumeSpace();
        if (peek() == 0) {
            std::string err = "Unexpected end of the string";
            return {err + str};
        }
        return parseAddition();
    }

    inline auto consume() -> char {
        return *str++;
    }

    [[nodiscard]] inline auto peek() const -> char {
        return *str;
    }

    inline void consumeSpace() {
        while (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n') {
            consume();
        }
    }

    [[nodiscard]] inline auto isDigit() const -> bool {
        return *str >= '0' && *str <= '9';
    }

    [[nodiscard]] inline auto isAlpha() const -> bool {
        return (*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z') || (*str == '_');
    }

    [[nodiscard]] inline auto isUnitChar() const -> bool {
        return isAlpha() || *str == '%';
    }

    auto parseFunction(const std::string_view &identifier) -> Result {
        auto f = functions.find(identifier);
        if (f == functions.end()) {
            return generateError("Unknown function", identifier);
        }

        // Consume '('
        consume();
        consumeSpace();

        std::array<number_t, PM_MAX_ARGUMENTS> arguments{};
        size_t                                 argc = 0;

        if (peek() != ')') {
            while (true) {
                if (argc == PM_MAX_ARGUMENTS) {
                    return generateError("Too many arguments");
                }
                Result argument = parseExpression();
                if (argument.isError()) {
                    return argument;
                }
                arguments[argc] = argument.result;
                argc++;
                consumeSpace();
                if (peek() != ',') {
                    break;
                }
                consume();
            }
        }

        if (peek() != ')') {
            return generateError("Expected ')'");
        }
        consume();
        Result ret = f->second(argc, arguments);
        if (ret.isError()) {
            // Improve information in errors generated inside functions
            return generateError(ret.getError(), identifier);
        }
        return ret;
    }

    auto parseSubExpression() -> Result {
        if (isDigit() || peek() == '.') {
            // Number
            return parseNumber();
        } else if (peek() == '(') {
            // Parenthesized expression
            consume();
            consumeSpace();
            Result exp = parseExpression();
            if (exp.isError()) {
                return exp;
            }
            // consume )
            consumeSpace();
            if (peek() != ')') {
                return generateError("Expected ')'");
            }
            consume();
            return exp;
        } else if (peek() == '-' || peek() == '+') {
            // Prefix unary operator
            char op = consume();
            consumeSpace();
            Result unary = parseSubExpression();
            if (unary.isError()) {
                return unary;
            }
            if (op == '-') {
                unary.result = -unary.result;
            }
            return unary;
        } else if (isAlpha()) {
            // Variable
            const char *start = str;
            size_t      size  = 0;
            do {
                consume();
                size++;
            } while (isAlpha());

            std::string_view identifier{start, size};
            consumeSpace();
            if (peek() == '(') {
                // function call
                return parseFunction(identifier);
            }
            auto f = variables.find(identifier);
            if (f == variables.end()) {
                return generateError("Unknown variable", identifier);
            } else {
                return {f->second};
            }
        } else {
            return generateError("Invalid character");
        }
    }

    inline auto parseNumber() -> Result {
        number_t ret = 0;

        while (isDigit()) {
            ret *= 10;
            ret += *str - '0';
            consume();
        }
        // Decimal point
        if (peek() == '.') {
            consume();
            number_t weight = 1;
            while (isDigit()) {
                weight /= 10;
                ret += (*str - '0') * weight;
                consume();
            }
        }

        // Space before units
        consumeSpace();

        // Units and percentage
        if (isUnitChar()) {
            const char *start = str;
            size_t      size  = 0;
            do {
                consume();
                size++;
            } while (isUnitChar());

            std::string_view identifier{start, size};

            auto f = units.find(identifier);
            if (f == units.end()) {
                return generateError("Unknown unit", identifier);
            }
            ret = ret * f->second;
        }
        return ret;
    }

    auto parseAddition() -> Result {
        consumeSpace();
        Result left = parseMultiplication();
        if (left.isError()) {
            return left;
        }
        consumeSpace();
        while (*str == '+' || *str == '-') {
            char op = consume();
            consumeSpace();
            Result right = parseMultiplication();
            if (right.isError()) {
                return right;
            }
            if (op == '+') {
                left.result += right.result;
            } else {
                left.result -= right.result;
            }
            consumeSpace();
        }
        return left;
    }

    auto parseMultiplication() -> Result {
        consumeSpace();
        Result left = parseSubExpression();
        if (left.isError()) {
            return left;
        }
        consumeSpace();
        while (*str == '*' || *str == '/') {
            char op = consume();
            consumeSpace();
            Result right = parseSubExpression();
            if (right.isError()) {
                return right;
            }
            if (op == '*') {
                left.result *= right.result;
            } else if (op == '/') {
                left.result /= right.result;
            }
            consumeSpace();
        }
        return left;
    }
};

} // namespace picomath
#endif
