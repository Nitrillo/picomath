#include <benchmark/benchmark.h>
#include <picomath.hpp>

static void BM_simpleExpression(benchmark::State &state) // NOLINT google-runtime-references
{
    picomath::PicoMath ctx;
    while (state.KeepRunning()) {
        benchmark::DoNotOptimize(ctx.parseExpression("2 + 2"));
        benchmark::ClobberMemory();
    }
}

static void BM_functions(benchmark::State &state) // NOLINT google-runtime-references
{
    picomath::PicoMath ctx;
    while (state.KeepRunning()) {
        benchmark::DoNotOptimize(ctx.parseExpression("cos(pi) + sin(pi)"));
        benchmark::ClobberMemory();
    }
}

static void BM_customUnit(benchmark::State &state) // NOLINT google-runtime-references
{
    picomath::PicoMath ctx;
    ctx.addUnit("px") = 2.0;
    while (state.KeepRunning()) {
        benchmark::DoNotOptimize(ctx.parseExpression("100px * 2"));
        benchmark::ClobberMemory();
    }
}

static void BM_complexExpression(benchmark::State &state) // NOLINT google-runtime-references
{
    picomath::PicoMath ctx;
    ctx.addVariable("x") = 2.0;
    ctx.addVariable("y") = 3.0;
    ctx.addVariable("z") = 5.0;
    ctx.addVariable("w") = 7.0;
    while (state.KeepRunning()) {
        benchmark::DoNotOptimize(ctx.parseExpression("((((x-(y/(z*w)))/(((x-y)*z)-w))/((((x+y)*z)-w)-((x+y)-(z*w))))/"
                                                     "(((((x-y)*z)-w)*((x+y)-(z/w)))*(((x+y)-(z*w))+((x/y)+(z+w)))))"));
        benchmark::ClobberMemory();
    }
}

auto main(int argc, char *argv[]) -> int {
    benchmark::RegisterBenchmark("Simple expression",
                                 BM_simpleExpression); // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
    benchmark::RegisterBenchmark("Built-in functions",
                                 BM_functions); // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
    benchmark::RegisterBenchmark("Custom units",
                                 BM_customUnit); // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
    benchmark::RegisterBenchmark("Complex expression",
                                 BM_complexExpression); // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;
}