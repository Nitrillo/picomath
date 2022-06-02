#include <picomath.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace picomath; // NOLINT

auto AreSame(number_t a, number_t b) -> bool {
    return fabs(a - b) < FLT_EPSILON;
}

TEST_CASE("Simple expressions") {
    PicoMath ctx;
    REQUIRE(AreSame(ctx.evalExpression("2 + 2").getResult(), 4.0));
    REQUIRE(AreSame(ctx.evalExpression("(2 + 2) * 4").getResult(), 16.0));
    REQUIRE(AreSame(ctx.evalExpression("4 * (2 + 2)").getResult(), 16.0));
    REQUIRE(AreSame(ctx.evalExpression("100 / 10").getResult(), 10.0));
    REQUIRE(AreSame(ctx.evalExpression("100 / (5 + 5)").getResult(), 10.0));
    REQUIRE(AreSame(ctx.evalExpression("(10 * 10) / (5 + 5)").getResult(), 10.0));
    REQUIRE(AreSame(ctx.evalExpression("2 + 2 * 4").getResult(), 10.0));
    REQUIRE(AreSame(ctx.evalExpression("2 + 2 * -4").getResult(), -6.0));
    REQUIRE(AreSame(ctx.evalExpression("2.003 * 1000").getResult(), 2003.0));
    REQUIRE(AreSame(ctx.evalExpression(".1234").getResult(), 0.1234));
    REQUIRE(AreSame(ctx.evalExpression("123.1234").getResult(), 123.1234));
    REQUIRE(AreSame(ctx.evalExpression("-(2+2)").getResult(), -4));
    REQUIRE(ctx.evalExpression("(2+2)").isOk());
    REQUIRE_FALSE(ctx.evalExpression("(2+2)").isError());
}

TEST_CASE("Built-in functions") {
    PicoMath ctx;
    REQUIRE(AreSame(ctx.evalExpression("cos(10) - cos(10) + 1").getResult(), 1.0));
    REQUIRE(AreSame(ctx.evalExpression("min(4, 3, 2, 1)").getResult(), 1.0));
    REQUIRE(AreSame(ctx.evalExpression("max(4, 3, 2, 1)").getResult(), 4.0));
    REQUIRE(AreSame(ctx.evalExpression("(sqrt(10000) * 4) / 100.00").getResult(), 4.0));
    REQUIRE(AreSame(ctx.evalExpression("max(cos(10) - cos(10) + 1, sqrt(10000))").getResult(), 100.0));
}

TEST_CASE("Custom functions") {
    PicoMath ctx;
    ctx.addFunction("multiply", [](size_t argc, const argument_list_t &args) -> Result {
        if (argc != 2) {
            return {"Invalid number of arguments"};
        }
        return {args[0] * args[1]};
    });

    REQUIRE(AreSame(ctx.evalExpression("multiply(8, 2)").getResult(), 16.0));
    REQUIRE(AreSame(ctx.evalExpression("multiply(2, 8)").getResult(), 16.0));
    REQUIRE(ctx.evalExpression("multiply(8)").isError());
    REQUIRE(ctx.evalExpression("multiply(8, 2, 3)").isError());
}

TEST_CASE("Custom units") {
    PicoMath ctx;
    ctx.addUnit("%") = 500.0 / 100.0;
    REQUIRE(AreSame(ctx.evalExpression("100%").getResult(), 500.0));
    REQUIRE(AreSame(ctx.evalExpression("50%").getResult(), 250.0));
    REQUIRE(AreSame(ctx.evalExpression("-1%").getResult(), -5.0));
    REQUIRE(AreSame(ctx.evalExpression("-200%").getResult(), -1000.0));

    ctx.addUnit("km") = 1000.0;
    REQUIRE(AreSame(ctx.evalExpression("1km").getResult(), 1000.0));
    REQUIRE(AreSame(ctx.evalExpression("100km").getResult(), 100000.0));
    REQUIRE(AreSame(ctx.evalExpression("100km - 10").getResult(), 100000.0 - 10.0));
}

TEST_CASE("Invalid expressions") {
    PicoMath ctx;
    REQUIRE(ctx.evalExpression("").isError());
    REQUIRE(ctx.evalExpression("(").isError());
    REQUIRE(ctx.evalExpression("*").isError());
    REQUIRE(ctx.evalExpression("3*").isError());
    REQUIRE(ctx.evalExpression("3 /").isError());
    REQUIRE(ctx.evalExpression("notfound").isError());
    REQUIRE(ctx.evalExpression("notfound()").isError());
    REQUIRE(ctx.evalExpression("notfound(").isError());
    REQUIRE(ctx.evalExpression("2px").isError());
    REQUIRE(ctx.evalExpression("cos(").isError());
    REQUIRE(ctx.evalExpression("cos(1,").isError());
    REQUIRE(ctx.evalExpression("((((((((()))").isError());
    REQUIRE(ctx.evalExpression("{").isError());
    REQUIRE(ctx.evalExpression("-23px").isError());
    REQUIRE(ctx.evalExpression("cos(1,2,3,4,5,6,7,8,9,10)").isError());
    REQUIRE(ctx.evalExpression("-(2*").isError());
}

TEST_CASE("Multi-expressions") {
    PicoMath ctx;
    auto     expression = ctx.evalMultiExpression("-2, 2 * 3, (10 - 5) * 5, sin(0), pi");
    Result   out;
    REQUIRE(expression.evalNext(&out));
    REQUIRE(AreSame(out.getResult(), -2));
    REQUIRE(expression.evalNext(&out));
    REQUIRE(AreSame(out.getResult(), 6));
    REQUIRE(expression.evalNext(&out));
    REQUIRE(AreSame(out.getResult(), 25));
    REQUIRE(expression.evalNext(&out));
    REQUIRE(AreSame(out.getResult(), 0));
    REQUIRE(expression.evalNext(&out));
    REQUIRE(AreSame(out.getResult(), M_PI));
}

TEST_CASE("Variables") {
    PicoMath ctx;
    auto &   x = ctx.addVariable("x");
    auto &   y = ctx.addVariable("y");

    x = 1;
    y = 1;
    REQUIRE(AreSame(ctx.evalExpression("x*x + y*y").getResult(), 2.0));

    x = 2;
    y = 1;
    REQUIRE(AreSame(ctx.evalExpression("x*x + y*y").getResult(), 5.0));

    x = 1;
    y = 2;
    REQUIRE(AreSame(ctx.evalExpression("x*x + y*y").getResult(), 5.0));

    x = 0;
    y = 2;
    REQUIRE(AreSame(ctx.evalExpression("x*x + y*y").getResult(), 4.0));
    REQUIRE(AreSame(ctx.evalExpression("max(x,y) * 2").getResult(), 4.0));
    REQUIRE(AreSame(ctx.evalExpression("min(x,y) * 2").getResult(), 0.0));
}
