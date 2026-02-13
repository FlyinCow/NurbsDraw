#include "core/BernsteinBase.h"
#include <cassert>
#include <cmath>

B::B(int j, int n) : j(j), n(n) {
}

double B::operator()(double t) {
    assert(t >= 0 && t <= 1);
    if (j < 0 || j > n) {
        return 0;
    }
    if (n == 0) {
        return 1.0;
    }
    if (j == 0) {
        return std::pow(1 - t, n);
    }
    if (j == n) {
        return std::pow(t, n);
    }
    return (1 - t) * B(j, n - 1)(t) + t * B(j - 1, n - 1)(t);
}