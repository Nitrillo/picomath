#include <picomath.hpp>
#define CATCH_CONFIG_MAIN
#include <catch.hpp>

using namespace picomath;

auto AreSame(number_t a, number_t b) -> bool {
    return fabs(a - b) < FLT_EPSILON;
}

TEST_CASE("Simple expressions") {
    PicoMath ctx;
    REQUIRE(AreSame(ctx.parseExpression("2 + 2").getResult(), 4.0));
    REQUIRE(AreSame(ctx.parseExpression("(2 + 2) * 4").getResult(), 16.0));
    REQUIRE(AreSame(ctx.parseExpression("4 * (2 + 2)").getResult(), 16.0));
    REQUIRE(AreSame(ctx.parseExpression("100 / 10").getResult(), 10.0));
    REQUIRE(AreSame(ctx.parseExpression("100 / (5 + 5)").getResult(), 10.0));
    REQUIRE(AreSame(ctx.parseExpression("(10 * 10) / (5 + 5)").getResult(), 10.0));
    REQUIRE(AreSame(ctx.parseExpression("2 + 2 * 4").getResult(), 10.0));
    REQUIRE(AreSame(ctx.parseExpression("2 + 2 * -4").getResult(), -6.0));
    REQUIRE(AreSame(ctx.parseExpression("2.003 * 1000").getResult(), 2003.0));
}

TEST_CASE("Built-in functions") {
    PicoMath ctx;
    REQUIRE(AreSame(ctx.parseExpression("cos(10) - cos(10) + 1").getResult(), 1.0));
    REQUIRE(AreSame(ctx.parseExpression("min(4, 3, 2, 1)").getResult(), 1.0));
    REQUIRE(AreSame(ctx.parseExpression("max(4, 3, 2, 1)").getResult(), 4.0));
    REQUIRE(AreSame(ctx.parseExpression("(sqrt(10000) * 4) / 100.00").getResult(), 4.0));
}

TEST_CASE("Custom functions") {
    PicoMath ctx;
    ctx.addFunction("multiply") = [](size_t argc, const argument_list_t &args) -> Result {
        if (argc != 2) {
            return {
                "Invalid number of arguments"
            };
        }
        return {args[0] * args[1]};
    };

    REQUIRE(AreSame(ctx.parseExpression("multiply(8, 2)").getResult(), 16.0));
    REQUIRE(AreSame(ctx.parseExpression("multiply(2, 8)").getResult(), 16.0));
    REQUIRE(ctx.parseExpression("multiply(8)").isError());
    REQUIRE(ctx.parseExpression("multiply(8, 2, 3)").isError());
}

TEST_CASE("Custom units") {
    PicoMath ctx;

    ctx.addUnit("%") = 500.0 / 100.0;
    REQUIRE(AreSame(ctx.parseExpression("100%").getResult(), 500.0));
    REQUIRE(AreSame(ctx.parseExpression("50%").getResult(), 250.0));
    REQUIRE(AreSame(ctx.parseExpression("-1%").getResult(), -5.0));
    REQUIRE(AreSame(ctx.parseExpression("-200%").getResult(), -1000.0));

    ctx.addUnit("km") = 1000.0;
    REQUIRE(AreSame(ctx.parseExpression("1km").getResult(), 1000.0));
    REQUIRE(AreSame(ctx.parseExpression("100km").getResult(), 100000.0));
    REQUIRE(AreSame(ctx.parseExpression("100km - 10").getResult(), 100000.0 - 10.0));
}

TEST_CASE("Invalid expressions") {
    PicoMath ctx;
    REQUIRE(ctx.parseExpression("").isError());
    REQUIRE(ctx.parseExpression("(").isError());
    REQUIRE(ctx.parseExpression("*").isError());
    REQUIRE(ctx.parseExpression("3*").isError());
    REQUIRE(ctx.parseExpression("3 /").isError());
    REQUIRE(ctx.parseExpression("notfound").isError());
    REQUIRE(ctx.parseExpression("2px").isError());
    REQUIRE(ctx.parseExpression("cos(").isError());
    REQUIRE(ctx.parseExpression("cos(1,").isError());
}

TEST_CASE("Multi-expressions") {
    PicoMath ctx;
    REQUIRE(AreSame(ctx.parseMultiExpression("-2, 2 * 3, (10 - 5) * 5, sin(0), pi").getResult(), -2));
    Result out{0};
    REQUIRE(ctx.parseNext(out));
    REQUIRE(AreSame(out.getResult(), 6));
    REQUIRE(ctx.parseNext(out));
    REQUIRE(AreSame(out.getResult(), 25));
    REQUIRE(ctx.parseNext(out));
    REQUIRE(AreSame(out.getResult(), 0));
    REQUIRE(ctx.parseNext(out));
    REQUIRE(AreSame(out.getResult(), M_PI));
}