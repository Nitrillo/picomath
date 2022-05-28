#include <algorithm>
#include <benchmark/benchmark.h>
#include <cctype>
#include <picomath.hpp>
#include <string>

static void BM_simpleExpression(benchmark::State &state) // NOLINT google-runtime-references
{
    picomath::PicoMath ctx;
    while (state.KeepRunning()) {
        benchmark::DoNotOptimize(ctx.parseExpression("(2 + 2) * 4 / 10 - 20.02"));
        benchmark::ClobberMemory();
    }
}

static void BM_baseLine(benchmark::State &state) // NOLINT google-runtime-references
{
    std::string data = "(2 + 2) * 4 / 10 - 20.02";
    while (state.KeepRunning()) {
        benchmark::DoNotOptimize(
            std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return std::tolower(c); }));
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
    benchmark::RegisterBenchmark("Baseline tolower", // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
                                 BM_baseLine); 
    benchmark::RegisterBenchmark("Simple expression", // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
                                 BM_simpleExpression); 
    benchmark::RegisterBenchmark("Built-in functions", // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
                                 BM_functions); 
    benchmark::RegisterBenchmark("Custom units", // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
                                 BM_customUnit); 
    benchmark::RegisterBenchmark("Complex expression", // NOLINT clang-analyzer-cplusplus.NewDeleteLeaks
                                 BM_complexExpression); 

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;
}