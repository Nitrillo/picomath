#ifndef PICOMATH_HPP
#define PICOMATH_HPP

#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace picomath {

#ifndef PM_MAX_ARGUMENTS
#define PM_MAX_ARGUMENTS 8
#endif

#define PM_LIKELY(x)   __builtin_expect((x), 1)
#define PM_UNLIKELY(x) __builtin_expect((x), 0)
#define PM_INLINE      inline __attribute__((always_inline))

class Result;

#ifdef PM_USE_FLOAT
using number_t = float;
#else
using number_t = double;
#endif
using argument_list_t        = std::array<number_t, PM_MAX_ARGUMENTS>;
using error_t                = std::string;
using custom_function_many_t = Result (*)(size_t argc, const argument_list_t &list);
using custom_function_1_t    = number_t (*)(number_t);

class Result {
    friend class PicoMath;

    number_t                 result;
    std::unique_ptr<error_t> error;

  public:
    Result(number_t result) : result(result) { // NOLINT
    }

    Result(std::string &&description) : result(0) { // NOLINT
        error = std::make_unique<error_t>(std::move(description));
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

#define PM_FUNCTION_2(fun)                                                                                             \
    [](size_t argc, const argument_list_t &args) -> Result {                                                           \
        if (argc != 2) {                                                                                               \
            return {"Two arguments needed"};                                                                           \
        }                                                                                                              \
        return fun(args[0], args[1]);                                                                                  \
    }

class PicoMath {

    struct Function {
        enum Type
        {
            FunctionMany = 0,
            Function1    = 1
        } type;
        custom_function_many_t many;
        custom_function_1_t    f1;
    };

    const char *                                 originalStr{};
    const char *                                 str{};
    std::map<std::string, number_t, std::less<>> variables{};
    std::map<std::string, number_t, std::less<>> units{};
    std::map<std::string, Function, std::less<>> functions{};

  public:
    auto addVariable(const std::string &name) -> number_t & {
        return variables[name];
    }

    auto addUnit(const std::string &name) -> number_t & {
        return units[name];
    }

    auto addFunction(const std::string &name, custom_function_many_t func) {
        auto &function = functions[name];
        function.type  = Function::Type::FunctionMany;
        function.many = func;
    }

    auto addFunction(const std::string &name, custom_function_1_t func) {
        auto &function = functions[name];
        function.type  = Function::Type::Function1;
        function.f1 = func;
    }

    auto parseMultiExpression(const char *expression) -> Result {
        originalStr = str = expression;
        return parseExpression();
    }

    auto parseNext(Result *outResult) -> bool {
        consumeSpace();
        if (isEOF()) {
            return false;
        }
        if (peek() == ',') {
            consume();
            consumeSpace();
        }
        *outResult = parseExpression();
        return true;
    }

    auto parseExpression(const char *expression) -> Result {
        originalStr = str = expression;
        Result ret        = parseExpression();
        consumeSpace();
        if (PM_LIKELY(isEOF())) {
            return ret;
        }
        return generateError("Invalid characters after expression");
    }

    PicoMath() {
        // Constants
        addVariable("pi") = static_cast<number_t>(M_PI);
        addVariable("e")  = static_cast<number_t>(M_E);

        // Built-in functions
        addFunction("abs", std::abs);
        addFunction("ceil", std::ceil);
        addFunction("floor", std::floor);
        addFunction("round", std::round);
        addFunction("ln", std::log);
        addFunction("log", std::log10);
        addFunction("cos", std::cos);
        addFunction("sin", std::sin);
        addFunction("acos", std::acos);
        addFunction("asin", std::asin);
        addFunction("cosh", std::cosh);
        addFunction("sinh", std::sinh);
        addFunction("tan", std::tan);
        addFunction("tanh", std::tanh);
        addFunction("sqrt", std::sqrt);
        addFunction("atan2", PM_FUNCTION_2(std::atan2));
        addFunction("pow", PM_FUNCTION_2(std::pow));
        addFunction("min", [](size_t argc, const argument_list_t &args) -> Result {
            number_t result{std::numeric_limits<number_t>::max()};
            size_t   i = 0;
            while (i < argc) {
                result = std::min(args[i], result);
                i++;
            }
            return result;
        });
        addFunction("max", [](size_t argc, const argument_list_t &args) -> Result {
            number_t result{std::numeric_limits<number_t>::min()};
            size_t   i = 0;
            while (i < argc) {
                result = std::max(args[i], result);
                i++;
            }
            return result;
        });
    }

  private:
    auto generateError(const char *error, std::string_view identifier = {}) const -> Result {
        std::string out = "In character " + std::to_string(static_cast<int>(str - originalStr - 1)) + ": ";
        out += error;
        if (identifier.empty()) {
            out += " found: ";
            out += isEOF() ? "End of string" : str;
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
        if (PM_UNLIKELY(isEOF())) {
            return generateError("Unexpected end of the string");
        }
        return parseAddition();
    }

    PM_INLINE auto consume() -> char {
        return *str++;
    }

    [[nodiscard]] PM_INLINE auto peek() const -> char {
        return *str;
    }

    PM_INLINE void consumeSpace() {
        while (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n') {
            consume();
        }
    }

    [[nodiscard]] PM_INLINE auto isDigit() const -> bool {
        return *str >= '0' && *str <= '9';
    }

    [[nodiscard]] PM_INLINE auto isAlpha() const -> bool {
        return (*str >= 'a' && *str <= 'z') || (*str >= 'A' && *str <= 'Z') || (*str == '_');
    }

    [[nodiscard]] PM_INLINE auto isUnitChar() const -> bool {
        return isAlpha() || *str == '%';
    }

    [[nodiscard]] PM_INLINE auto isEOF() const -> bool {
        return *str == 0;
    }

    auto parseFunction(std::string_view identifier) -> Result {
        auto f = functions.find(identifier);
        if (PM_UNLIKELY(f == functions.end())) {
            return generateError("Unknown function", identifier);
        }

        // Consume '('
        consume();
        consumeSpace();

        std::array<number_t, PM_MAX_ARGUMENTS> arguments{};
        size_t                                 argc = 0;

        if (peek() != ')') {
            while (true) {
                if (PM_UNLIKELY(argc == PM_MAX_ARGUMENTS)) {
                    return generateError("Too many arguments");
                }
                Result argument = parseExpression();
                if (PM_UNLIKELY(argument.isError())) {
                    return argument;
                }
                arguments.at(argc) = argument.result;
                argc++;
                consumeSpace();
                if (peek() != ',') {
                    break;
                }
                consume();
            }
        }

        if (PM_UNLIKELY(peek() != ')')) {
            return generateError("Expected ')'");
        }
        consume();
        if (f->second.type == Function::Type::Function1) {
            if (PM_UNLIKELY(argc != 1)) {
                return generateError("One argument required");
            }
            return {f->second.f1(arguments[0])};
        } else {
            Result ret = f->second.many(argc, arguments);
            if (PM_UNLIKELY(ret.isError())) {
                // Improve information in errors generated inside functions
                return generateError(ret.getError(), identifier);
            }
            return ret;
        }
    }

    PM_INLINE auto parseParenthesized() -> Result {
        consume();
        consumeSpace();
        Result exp = parseExpression();
        if (PM_UNLIKELY(exp.isError())) {
            return exp;
        }
        consumeSpace();
        // consume ')'
        if (PM_UNLIKELY(peek() != ')')) {
            return generateError("Expected ')'");
        }
        consume();
        return exp;
    }

    PM_INLINE auto parsePrefixUnaryOperator() -> Result {
        char op = consume();
        consumeSpace();
        Result unary = parseSubExpression();
        if (PM_UNLIKELY(unary.isError())) {
            return unary;
        }
        if (op == '-') {
            unary.result = -unary.result;
        }
        return unary;
    }

    PM_INLINE auto parseVariableOrFunction() -> Result {
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
        if (PM_UNLIKELY(f == variables.end())) {
            return generateError("Unknown variable", identifier);
        }
        return {f->second};
    }

    PM_INLINE auto parseNumber() -> Result {
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
            if (PM_UNLIKELY(f == units.end())) {
                return generateError("Unknown unit", identifier);
            }
            ret = ret * f->second;
        }
        return ret;
    }

    PM_INLINE auto parseSubExpression() -> Result {
        if (isDigit() || peek() == '.') {
            // Number
            return parseNumber();
        }
        if (peek() == '(') {
            // Parenthesized expression
            return parseParenthesized();
        }
        if (peek() == '-' || peek() == '+') {
            // Prefix unary operator
            return parsePrefixUnaryOperator();
        }
        if (isAlpha()) {
            // Variable or function
            return parseVariableOrFunction();
        }
        return generateError("Invalid character");
    }

    auto parseAddition() -> Result {
        consumeSpace();
        Result left = parseMultiplication();
        if (PM_UNLIKELY(left.isError())) {
            return left;
        }
        consumeSpace();
        while (*str == '+' || *str == '-') {
            char op = consume();
            consumeSpace();
            Result right = parseMultiplication();
            if (PM_UNLIKELY(right.isError())) {
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
        if (PM_UNLIKELY(left.isError())) {
            return left;
        }
        consumeSpace();
        while (*str == '*' || *str == '/') {
            char op = consume();
            consumeSpace();
            Result right = parseSubExpression();
            if (PM_UNLIKELY(right.isError())) {
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
