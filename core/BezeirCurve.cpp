#include "BezeirCurve.h"
#include "BernsteinBase.h"
#include "Vector.h"
#include <vector>

BezeirCurve::BezeirCurve(std::vector<Vector> &vertices) : vertices(vertices) {
}

Vector BezeirCurve::eval(double t) {
    Vector res{};
    int n = vertices.size() - 1;
    for (auto i = 0; i <= n; i++) {
        res += B(i, n)(t) * vertices[i];
    }
    return res;
}
