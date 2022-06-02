# PicoMath

PicoMath is a header only C++ library to evaluate math expressions.

Initially created to evaluate CSS-like expressions for the Shinobit Engine, a proprietary 2D game engine.

This library is used in the MMO browser game [GoBattle.io](http://gobattle.io)

![Build status](https://github.com/Nitrillo/picomath/actions/workflows/ci.yml/badge.svg)

## Features
* Header only C++ library: easy integration in your project
* Very simple and fast
* Zero allocations in the hot path
    * Uses C++17 `std::string_view` to lookup variables without copying strings
* Single pass: The evaluation is performed while parsing.
* Built-in math functions (e.g. `cos(pi)`)
* Custom units and percentages (e.g `100% - 10px`)
* User defined variables (e.g. `(x + y) * (x + y)`)
* User defined functions with multiple arguments (e.g. `avg(10, 20, 30)`)
* Uses standard C++ containers

## How to integrate in your project

Just copy the file `/include/picomath.hpp` in your project

## Usage

Simple evaluation:
```cpp
#include <picomath.hpp>

using namespace picomath;

PicoMath pm;

auto result = pm.evalExpression("sqrt(100 - 20)");
if (result.isOk()) {
    double r = result.getResult();
    ...
}

```

Using variables:
```cpp
#include <picomath.hpp>

using namespace picomath;

PicoMath pm;
auto &x = pm.addVariable("x");

x = 0.0;
while (x < 100.0) {
    // Same expression evaluated with different values of `x`
    auto result = pm.evalExpression("x * x * x");
    if (result.isOk()) {
        double r = result.getResult();
        ...
    }
    x += 1.0;
}

```

Using units:
```cpp
#include <picomath.hpp>

using namespace picomath;

PicoMath pm;
pm.addUnit("km") = 1000.0;
pm.addUnit("cm") = 0.01;

auto result = pm.evalExpression("0.5km + 20cm");
if (result.isOk()) {
    double r = result.getResult();
    ...
}

```



## Test

```shell
# build test binaries
make

# run tests
make test

# run bench tests
make bench
```

The default test binaries will be built in release mode. You can make Debug test binaries as well:

```shell
make clean
make debug
make test
```


## Benchmarks

PicoMath is pretty fast as it evaluates the expression without allocating memory.
Compared to a basic strtolower of the string `(2 + 2) * 4 / 10 - 20.02`, using the method
```
std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return std::tolower(c); }));
```
the evaluation of the same string takes only around 25% more time.
Please run the benchmark in your machine to validate the results.

```
Run on (12 X 2900 MHz CPU s)
CPU Caches:
  L1 Data 32K (x6)  
  L1 Instruction 32K (x6)
  L2 Unified 262K (x6)
  L3 Unified 12582K (x1)
----------------------------------------------------------
Benchmark                   Time           CPU Iterations
----------------------------------------------------------
Baseline tolower           43 ns         43 ns   16254876
Simple expression          54 ns         54 ns   12960515
```

PicoMath was designed to evaluate a expression in one pass, so to reevaluate the same expression requires parsing again the expression.

Some other libraries use bytecode or ASTs to improve the runtime cost of multiple evaluations of the same expression, but that increases complexity, size and number of allocations.
In my tests comparing PicoMath to TinyExpr, you need to evaluate at least three times the same expression to get some benefit of the extra steps.  


# Contributing and License

Contributors are welcome! :sparkles: please see [CONTRIBUTING](CONTRIBUTING.md) for more info.

PicoMath is licensed under a `BSD 3-Clause` license, see [LICENSE](LICENSE.md) for more info.

Please let me know if you use the library in other projects and I can add the link in the readme.
##  Attribution

This repository was created using the HPP-SKEL repository: https://github.com/mapbox/hpp-skel


